#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

// Templates should be completely instantiated in header file so compiler can generate the appropriate instantiation

template<typename T>
class ThreadSafeQueue {
public:
    // default constructor
    ThreadSafeQueue() = default;
    // no copy constructor, copying a queue w a mutex / conditional variable migjt pose an issue, copies might interefere with each other
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    // cannot assign one thread safe queue to another
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // Move constructor
    ThreadSafeQueue(ThreadSafeQueue&& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        q = std::move(other.q);
         // dont have to acquire the other's conditional variable or mutex of course
    }

    // Move assignment operator
    ThreadSafeQueue& operator=(ThreadSafeQueue&& other) {
        if (this != &other) {
            std::scoped_lock lock(mtx, other.mtx);
            q = std::move(other.q);
            // dont have to acquire the other's conditional variable or mutex of course
        }
        return *this;
    }

    // Destructor (default is sufficient since RAII types, mutex, cv, queue clean up their resources)
    ~ThreadSafeQueue() = default;

    // Push a new item into the queue.
    // this is for when the caller indicates that the object remains valid state after this push
    void push(const T& value) {
        {   
            // immediately locks the mutex
            // lock guard is an RAII wrapper that automatically locks a mutex upon creation and unlocks it when goes out of scope
            std::lock_guard<std::mutex> lock(mtx);
            q.push(value);
        }
        // after value is pushed, this wakes up one thread that might be blocked in a wait_pop call
        cv.notify_one(); // Notify one waiting thread.
    }

    // Overloaded push for rvalue references.
    // this for when the caller indicates that the object can be safelty moved

    void push(T&& value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            // converts the rvalue to an xvalue, enabling the queue to "steal" /move the internal resources of value instead of copying them
            q.push(std::move(value));
        }
        cv.notify_one();
    }

    // Try to pop an item; returns false if the queue is empty.
    bool try_pop(T& result) {
        std::lock_guard<std::mutex> lock(mtx);
        if (q.empty())
            return false;
        result = std::move(q.front());
        q.pop();
        return true;
    }

    // Wait until an item is available, then pop it.
    // As you can see, there is no return value, you could use move smeantics in modern C++ to elimate most copy overhead
    // but this is consistent design for a thread safe queue implementation
    // I was thinking y not j return result then if used elsewhere use move semantics or just use the reference of the result but i guess its j a design pattern for cleaner code
    void wait_pop(T& result) {
        // locks the mutex to protect access to the underlying queue q
        // need to acquire lock bc want exclusive access to the queue when checking if empty, reading front and popping
        std::unique_lock<std::mutex> lock(mtx);
        // released mutex and puts thread to sleep until the cv has been notified.
        // if thread is woken up, automatically re acquires the mutex before proceeding
        // The lambda predicate ([this]() { return !q.empty(); }) is checked each time the thread wakes up. The thread only proceeds if the queue is not empty.
        // this prevents busy waiting
        cv.wait(lock, [this]() { return !q.empty(); });
        // efficient transfer the elements resources to result instead of copying it
        result = std::move(q.front());
        q.pop();
        // once the function exits, the std:: unique lock goes out of scope, the destructor automatically released te mutex

    }

    // Can also have a wait_pop_for, if there is a timeout, then u can wake and thread can see if it shld exit for example
    // also can have wait_pop_for if there is a need to perform some periodic tasks like logging, updating metrics or rebalancing work

    // Check if the queue is empty.
    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return q.empty();
    }

private:
    mutable std::mutex mtx;
    std::queue<T> q;
    std::condition_variable cv;
};