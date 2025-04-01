// Rule of 5 please

// Using a template here because unique pointer can be pointing to anything,

template <typename T>
class UniquePtr {

// private because no one outside the class can access the raw pointer
private:
    T* ptr // this is the raw array

public: 

    // rule of 5 Sai: Need to have all 5 (or delete them)

    // Default constructor

    // below, without explicit, compiler may implicitly convert a raw pointer to a unique pointer (as in create a new unique pointer using the raw ptr as an argument) when a raw pointer
    // is supplied to a func whose arguments require smart pointers, for example 

    explicit UniquePtr(T* p) : ptr(p) {}

    // Destructor 

    // Smart pointers are usually on stack, so when they go out of scope, the destructor is called
    // The destructor then calls delete ptr, what this does it it calls the destructor of the object ptr is pointing to and dealloactes that obj from memory
    // ptr here is also a local variable that will be popped off from the stack

    ~UniquePtr() {
        delete ptr;
    }   

    // Move constructor : Tells the compiler that the move constructor is guaranteed not to throw exceptions
    // so many standard containers like vectors than optimise operations if they know that moving ann object wont throw an exception
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
        // Theres no way to dlt a ptr without deleting the resource its pointing at in Cpp, so set it to null ptr
    }
    // takes in an Rvalue

    // Move assignment operator: cleans up current object, then takes over 'other's pointer.
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
        // enable chaining like a = std::move(b) = std::move(c);
    }    

    // Delete the copy constructor and copy assignment operator to enforce unique ownership.
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Dereference operator to access the object.
    T& operator*() const {
        return *ptr;
    }

    // Arrow operator to access the object's members.
    T* operator->() const {
        return ptr;
    }

    // get() returns the raw pointer without releasing ownership.
    T* get() const {
        return ptr;
    }

    // release() relinquishes control of the pointer and returns it.
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // reset() deletes the current object and takes ownership of the new pointer.
    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }
};        























}