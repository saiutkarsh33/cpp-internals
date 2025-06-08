#include <vector>
#include <stdexcept>   // for std::out_of_range
#include <concepts>    // for std::convertible_to


template<typename T>
class CircularBuffer {
private:
    std::vector<T> array;
    int capacity;
    // front is 0, back is 0, add at back 
    int front;
    int back;
    int size_;
public:

    CircularBuffer(int _capacity) {
        this->capacity = _capacity;
        this->front = 0;
        this->back = 0;
        this->size_ = 0;
        array.resize(capacity);
    }
    
    // noexcept just means this function does not throw an exception
    // you don't do bool& because that bool is just popped off the stack after the function, doesent exist in memory
    // and copying doesent take up much memory anyways
    // when you do bool b = is_full(), the is_full() function is returning a prvalue (temporary value) and b is initalised
    // from that prvalue (just copies its bit)
    // For this kind of value the compiler might optimise to theres no actual "copy" instruction as well

    bool is_full() const noexcept {
        return size_ == capacity;
        // or return size() == capacity;
    }

    bool is_empty() const noexcept {
        return front == back;
        // or return size_ == 0;
        // or return size() == 0;
    }
    // Pass by value when you need your own copy (e.g. to store or modify it).
    // Pass by const reference when you only need read-only access (no copy).
    // Pass by non-const reference only if you intend to mutate the caller’s object.

    // Normal functions take precendence over function templates
    // Here the templated function is used when you pass something convertible to T that isnt exactly T (bc of my other function overloaded calls)
    // eg only this can take in a const char* when T is a std::string
    // This is what perfect forwarding does in general:
    // U&& is a *forwarding reference*:
    // 1. If you pass an lvalue of type T (or const T) or the corresponding reference, U deduces to T& (or const T&), so U&& collapses to T&/const T&.
    //    std::forward<U>(v) yields an lvalue, and you get a copy‐assignment (or copy‐construction).
    // 2. If you pass an rvalue of type T or an rvalue reference of type T, U deduces to T, so U&& is T&&.
    //    std::forward<U>(v) yields an rvalue, and you get a move‐assignment (or move‐construction).
    // 3. If you pass something else convertible to T (e.g. a const char* into a CircularBuffer<std::string>),
    //    U deduces to that other type, and std::forward<U>(v) passes it through to T’s constructor or assignment.
    template<std::convertible_to<T> U>
    void push_back(U&& v) {
      if (is_full()) {
        throw out_of_range("Circularbuffer is full");
      }
      array[back] = std::forward<U>(v);
      back = (back + 1) % capacity;
      size_++;
    }

    // Overload for rvalues: binds to prvalues (e.g. T{}, temporaries) or xvalues
    // (e.g. std::move(x)), and moves from them into the buffer.
    void push_back(T&& v) {
      if (is_full()) {
        throw out_of_range("Circularbuffer is full");
      }
      // still have to move because you have to make the lvalue reference "v" which is the argument, into an rvalue reference
      // end of the day there is only one "v" in memory - inside the array, and no where else. Original object and argument v are left in a valid but unspecified state
      // This "state" refers to how the move is dealt with within the class T itself
      array[back] = std::move(v);
      back = (back + 1) % capacity;
      size_++;
    }

    // const lvalue allows u to enter non const lvalues, const lvalues and rvalues (altho i have a more specific rvalue push back here)
    // non const lvalue allows you to only take in non const lvalues only
    void push_back(const T& v) {
      if (is_full()) {
        throw out_of_range("Circularbuffer is full");
      }
      array[back] = v; // copy constructor
      back = (back + 1) % capacity;
      size_++;
    }
    
    void pop_front() {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 
        front = (front + 1) % capacity;
        size_--;
    }
    
    // Here the return type T is a copy, no need to overload on return type constness, just mark the method themselves const
    // Labelling the function here as const allow you to use this function on both a const and a non const caller eg

    //CircularBuffer<int> buf(10);   // buf is NOT const
    //buf.get_front();  // calls (1) the non-const version
    // and

    //const CircularBuffer<int> cbuf(10);  
    //cbuf.get_front(); // only (2) the const version is callable

    // But if the method was not const, only a non const caller can call it

    T get_back() const {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 

        return (back == 0) ? array[capacity - 1] : array[back-1];        
    }
    
    // Returns a reference, so here you can modify the T in the internal array outside
    // Can only be called by a non const buffer
    T& get_back() {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 

        return (back == 0) ? array[capacity - 1] : array[back-1];        
    }
    
    // Cannot edit the T in this array
    // returns a const reference
    // can be called by both const and non const buffers
    const T& get_back() const {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 

        return (back == 0) ? array[capacity - 1] : array[back-1];        
    }

    T get_front() const {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 
        return array[front];        
    }

    const T& get_front() const {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 
        return array[front];        
    }

    T& get_front() {
        if (is_empty()) {
            throw out_of_range("Circularbuffer is Empty");
        } 
        return array[front];        
    }

    int size() const noexcept {
        // Notice I also am using a size_ variable so I can just return size_ instead
        // Just having different implementations
        if (back == front) {
            return 0;
        }
        else if (back > front) {
            return back - front;
        }
        else {
            return capacity - front + back;
        }

    }
};

// https://www.geeksforgeeks.org/implement-circular-buffer-using-std-vector-in-cpp/

    












