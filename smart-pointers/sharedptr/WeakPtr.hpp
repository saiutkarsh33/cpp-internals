#include "SharedPtr.hpp" 

template <typename T>
class WeakPtr {
private:
    ControlBlock<T>* cb = nullptr;

    void release() {
        if (!cb) return;
        
        // Decrement weak count
        if (cb->weak_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            // Last weak reference - check if control block should be destroyed
            if (cb->strong_count.load(std::memory_order_acquire) == 0) {
                delete cb;
            }
        }
        cb = nullptr;
    }
    
    void swap(WeakPtr& other) noexcept {
        std::swap(cb, other.cb);
    }

public:
    // Default constructor
    WeakPtr() : cb(nullptr) {}
    
    // Constructor from SharedPtr
    WeakPtr(const SharedPtr<T>& shared) : cb(shared.cb) {
        if (cb) {
            cb->weak_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    // Copy constructor
    WeakPtr(const WeakPtr& other) : cb(other.cb) {
        if (cb) {
            cb->weak_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
    
    // Move constructor
    WeakPtr(WeakPtr&& other) noexcept : cb(other.cb) {
        other.cb = nullptr;
    }
    
    // Destructor
    ~WeakPtr() {
        release();
    }
    
    // Copy assignment
    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) return *this;
        
        WeakPtr temp(other);
        swap(temp);
        return *this;
    }
    
    // Move assignment
    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (this != &other) {
            release();
            cb = other.cb;
            other.cb = nullptr;
        }
        return *this;
    }
    
    // Assignment from SharedPtr
    WeakPtr& operator=(const SharedPtr<T>& shared) {
        WeakPtr temp(shared);
        swap(temp);
        return *this;
    }
    
    // Check if the object still exists
    bool expired() const {
        return !cb || cb->strong_count.load(std::memory_order_relaxed) == 0;
    }
    
    // Get strong reference count  
    size_t use_count() const {
        return cb ? cb->strong_count.load(std::memory_order_relaxed) : 0;
    }

    // Attempt to convert a WeakPtr a SharedPtr (returns empty SharedPtr if expired)
    // THIS IS THE ONLY WAY TO ACCESS THE STUFF THE WEAK PTR IS POINTING AT, U MUST TURN IT INTO A SHARED PTR FIRST

    // Why isn’t there a get() on WeakPtr?
    // A WeakPtr deliberately does not expose a raw T* because the object may already have been destroyed—the pointer would dangle. Instead, the only safe way to access the object is:

    // cpp
    // Copy
    // Edit
    // if (auto sp = myWeak.lock()) {
    //   // sp.get() is non-null, and you own a fresh SharedPtr
    //   // to keep the object alive while you use it
    // }
    // If you really need a direct T*, you can call lock() and then sp.get(). Exposing a WeakPtr::get() that returned cb->ptr without bumping the strong count would risk use-after-free, so it’s intentionally omitted.


    // Use case 1: You habd out WeakPtr to observers, they call lock() each time before touching the object and if its expired they bail out gracefully
    // Use case 2: Breaking cycles eg a <-> b, you only store WeakPtr for the "back-edges". When you actly need to use the parent u do auto p = child.parent.lock()
    // ^ Back edge is WeakPtr to prevent memory leaks
    SharedPtr<T> lock() const {
        if (!cb) return SharedPtr<T>();
        
        // Try to increment strong count if it's not zero
        size_t current_strong = cb->strong_count.load(std::memory_order_relaxed);

        // CAS algo
        
        do {
            if (current_strong == 0) {
                // Object has been destroyed
                return SharedPtr<T>();
            }
            // Use acquire ordering on success to synchronize with release operations
        } while (!cb->strong_count.compare_exchange_weak(
                    current_strong,            // your expected old value
                    current_strong + 1,        // the new value you want
                    std::memory_order_acquire, // if you succeed, take an acquire fence
                    std::memory_order_relaxed)); // if you fail, it’s just a relaxed read
        // Successfully incremented, create SharedPtr without additional increment
        return SharedPtr<T>(cb);
    }
    
    // Reset to empty state
    void reset() {
        release();
    }

};