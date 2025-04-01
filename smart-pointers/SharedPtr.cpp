#include <iostream>

template <typename T>
class SharedPtr {
private:
    T* ptr;   
    // have a ref count as a pointer so that all copies of shared pointer have same same ref cnt variable instead of their own copies  
    // atomic so thread safe      
    std::atomic<size_t>* ref_count; 

public:
    // Default constructor 
    SharedPtr() : ptr(nullptr), ref_count(new std::atomic<size_t>(0)) {}

    // Constructor that takes ownership of a raw pointer.
    // allocates a new std::atomic<size_t> on the heap, initializes it with the value 1, and returns a pointer to it
    explicit SharedPtr(T* p) : ptr(p), ref_count(new std::atomic<size_t>(1)) {}
    
    // Copy constructor: increment the reference count.
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        ++(*ref_count);
    }

    // Move constructor: transfer ownership from other to this.
    SharedPtr(SharedPtr&& other) noexcept 
        : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        // Set other's ref_count to a new counter of 0 to keep it valid.
        other.ref_count = new size_t(0);
    }

    // Destructor: decrement the reference count and delete managed object if needed.
    ~SharedPtr() {
        if(ptr && --(*ref_count) == 0) {
            delete ptr;
            delete ref_count;
        }
        else if(!ptr) { 
            // If this has been moved from 
            delete ref_count;
        }
    }

    // Copy assignment operator.
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            // Decrement current object's ref count and delete resource if needed.
            if(ptr && --(*ref_count) == 0) {
                delete ptr;
                delete ref_count;
            }
            // Copy data from other.
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this; // Return *this to allow chaining.
    }

    // Move assignment operator.
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            // Decrement current object's ref count and delete resource if needed.
            if(ptr && --(*ref_count) == 0) {
                delete ptr;
                delete ref_count;
            }
            // Transfer ownership from other.
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = new size_t(0);
        }
        return *this; // Return *this to allow chaining.
    }

    // Overload dereference operator.
    T& operator*() const {
        return *ptr;
    }

    // Overload arrow operator.
    T* operator->() const {
        return ptr;
    }

    // Return the raw pointer.
    T* get() const {
        return ptr;
    }

    // Return the current reference count.
    size_t get_count() const {
        return *ref_count;
    }
};

