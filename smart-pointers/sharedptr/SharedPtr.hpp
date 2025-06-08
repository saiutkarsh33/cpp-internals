#include <atomic>

template<typename T>
struct ControlBlock {
    std::atomic<size_t> strong_count{1};
    std::atomic<size_t> weak_count{0};
    T* ptr;
    bool object_destroyed{false};

    ControlBlock(T* p) : ptr(p) {}
    
    ~ControlBlock() {
        // Only delete if not already destroyed
        destroy_object();
    }
    
    void destroy_object() {
        if (!object_destroyed && ptr) {
            delete ptr;
            ptr = nullptr;
            object_destroyed = true;
        }
    }
};

// We use the -> here because its inheriting from the parent class
// InplaceControlBlock<U> inherits counts from ControlBlock<U>
// and stores the U object inline in its own storage buffer.
template<typename U>
struct InplaceControlBlock : ControlBlock<U> {
    // Raw bytes, correctly aligned for U
    // Telling compiler to get a byte array of size sizeof(U thats also alliged on a byte boundary that U specifies

    // Why must U potentially align on a specific byte boundary? 

    // Certain hardware / CPU instructions require it like special aligned load / store

    // If not aligned, could span 2 different cache lines: This means greater cache misses (if eg only space for one of the 2 cache lines), need 2 different memory access instead of one
    // And also if a store spans 2 cahce lines, CPU may need to perform 2 seperate writes, one for each line. If one cache is busy being written back or invalidated, second store may stall, slowing down the pipeline
    
    // False sharing: If an object is misaligned and crosses cache line boundaries,
    // modifying it may cause TWO cache lines to be marked dirty.
    // Result: extra memory writes, worse cache performance.

    // Some atomic operations require certain alignment guarantees. If the variable isnt naturally aligned, the CPU can't do an atomic read / write in one instruction, breaking thread safety
    // Lock free structures especially, strict alignment is impt

    alignas(U) unsigned char storage[sizeof(U)];

    // Forward all constructor arguments into U’s constructor,
    // placement-new’ing it into our storage.
    template<typename... Args>
    explicit InplaceControlBlock(Args&&... args)
      : ControlBlock<U>(nullptr)           // base ptr(nullptr); counts start at 1
    {
        // Construct U in our buffer
        U* obj = new (&storage) U(std::forward<Args>(args)...);
        this->ptr = obj;                   // point base::ptr at it
    }

    // When the last SharedPtr goes away, destroy_object()
    // will call U’s destructor in place, not delete the block.
    void destroy_object() override {
        if (!this->object_destroyed && this->ptr) {
            reinterpret_cast<U*>(storage)->~U();
            this->ptr = nullptr;
            this->object_destroyed = true;
        }
    }

    // And when the block itself is deleted (after weak count hits 0),
    // we still ensure the object is destroyed exactly once.
    ~InplaceControlBlock() override {
        destroy_object();
    }
};


template <typename T>
class SharedPtr {
private:
    ControlBlock<T>* cb = nullptr;

    void swap(SharedPtr& other) noexcept {
        std::swap(cb, other.cb);
    }

    void release() {
        if (!cb) return;
        
        // Atomically decrement strong count
        if (cb->strong_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // Last strong reference - destroy the object
            cb->destroy_object();
            
            // Check if we should destroy control block
            if (cb->weak_count.load(std::memory_order_acquire) == 0) {
                delete cb;
            }
        }
        cb = nullptr;
    }

    // only WeakPtr can use this:
    explicit SharedPtr(ControlBlock<T>* block)
        : cb(block)
    {}

public:
    SharedPtr() : cb(nullptr) {}

    // Constructor with raw pointer
    explicit SharedPtr(T* p) : cb(p ? new ControlBlock<T>(p) : nullptr) {}

    // Copy constructor
    SharedPtr(const SharedPtr& o) : cb(o.cb) {
        if (cb) {
            // can do ++ instead
            // But There's a Catch!
            // The problem arises when you need to use the result or do conditional logic:
            // ❌ Race Condition Example:
            // cpp// UNSAFE: Read-then-act pattern
            // if (++counter == 1) {  // Race condition here!
            //     // First thread to increment?
            //     do_something();
            // }
            // Why it's unsafe:

            // Thread A: increments counter from 0 to 1, sees result 1
            // Thread B: increments counter from 1 to 2, sees result 2
            // Thread A: enters if-block thinking it's "first"
            // Thread C: increments counter from 0 to 1 (if A hadn't finished), sees result 1
            // Both A and C think they're "first"!
            cb->strong_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    // Move constructor
    SharedPtr(SharedPtr&& o) noexcept : cb(o.cb) {
        o.cb = nullptr;
    }

    // Destructor
    ~SharedPtr() {
        release();
    }
    
    // Copy assignment
    SharedPtr& operator=(const SharedPtr& o) {
        if (this == &o) return *this; // Self-assignment check
        
        // Create temporary to ensure exception safety
        SharedPtr temp(o);
        swap(temp);
        return *this;
    }

    // Move assignment, dont need ensure exception safety since move assn n move ctor r noexcept usually
    SharedPtr& operator=(SharedPtr&& o) noexcept {
        if (this != &o) {
            release();
            cb = o.cb;
            o.cb = nullptr;
        }
        return *this;
    }

    // Dereference operator
    T& operator*() const {
        return *cb->ptr;
    }

    // Arrow operator
    T* operator->() const {
        return cb->ptr;
    }

    // Get raw pointer
    T* get() const {
        return cb ? cb->ptr : nullptr;
    }
    
    // Check if pointer is valid
    explicit operator bool() const {
        return cb && cb->ptr;
    }
    
    // Get reference count
    size_t use_count() const {
        return cb ? cb->strong_count.load(std::memory_order_relaxed) : 0;
    }
    
    // Reset to empty state
    void reset() {
        release();
    }
    
    // Reset with new pointer
    void reset(T* p) {
        SharedPtr temp(p);
        swap(temp);
    }

    // gives weakptr access to the private the protected members of the shared class
    friend class WeakPtr<T>;

    // Why make_shared is btr

    // 1. Allocations

    // SharedPtr(new T…):

    // You do new T(args…)

    // Inside the SharedPtr(T*) ctor you do new ControlBlock<T>(raw)
    // ⇒ 2 separate heap allocations.

    // make_shared<T>(…):

    // One new InplaceControlBlock<T>(args…) which both reserves space for the control‐block and constructs the T in that same block.
    // ⇒ 1 single heap allocation.

    // 2. Cache locality

    // Two allocations → likely two different addresses in memory. Every time you go from your ref‐count data to the actual T object you jump around in RAM, causing extra cache misses.

    // One allocation → control counts and the T land side‐by‐side in memory. Fetching the count and then immediately dereferencing the object stays within the same cache line more often.

    // 3. Exception safety

    // With two allocations, if the second new ControlBlock throws, you already did the first new T, and unless you wrap it in a try/catch and manually delete raw, that first allocation leaks.

    // With one fused allocation, if the allocation or the in‐place construction of T throws, nothing was ever committed to the heap, so there’s nothing to clean up—no leaks, no special error handling needed.
    template<typename T, typename... Args>
    SharedPtr<T> make_shared(Args&&... args) {
        // You forward each argument perfectly as its passed in, eg if arg1 is lvalue and arg2 is rvalue, arg1 is copied and arg2 is moved 
        
        auto* block = new InplaceControlBlock<T>(std::forward<Args>(args)...);
        return SharedPtr<T>(block);
    }

};



class TestClass {
public:
    int value;
    TestClass(int v) : value(v) {
        std::cout << "TestClass(" << v << ") constructed\n";
    }
    ~TestClass() {
        std::cout << "TestClass(" << value << ") destroyed\n";
    }
};

int main() {
    // Test basic functionality
    {
        SharedPtr<TestClass> ptr1(new TestClass(42));
        std::cout << "ptr1->value: " << ptr1->value << std::endl;
        std::cout << "use_count: " << ptr1.use_count() << std::endl;
        
        {
            SharedPtr<TestClass> ptr2 = ptr1;  // Copy
            std::cout << "After copy, use_count: " << ptr1.use_count() << std::endl;
            
            SharedPtr<TestClass> ptr3 = std::move(ptr2);  // Move
            std::cout << "After move, use_count: " << ptr1.use_count() << std::endl;
        }
        
        std::cout << "After inner scope, use_count: " << ptr1.use_count() << std::endl;
    }
    std::cout << "After outer scope - object should be destroyed\n";
    
    return 0;
}