
#include <memory>
#include <mutex>

template <typename T>

class ThreadSafeLinkedList {

private:
    struct Node {
        std::unique_ptr<T> value;
        std::shared_ptr<Node> next;

        mutable std::mutex mutex;
        
        // Constructor
        Node(const T& val): value(std::make_unique<T>(val)), next(nullptr) {}

        Node() : value(nullptr), next(nullptr) {} // For dummy nodes

    };

    std::shared_ptr<Node> dummy_head;
    std::shared_ptr<Node> dummy_tail;
    std::shared_ptr<Node> tail;

public: 

    ThreadSafeLinkedList() {
        dummy_head = std::make_shared<Node>();
        dummy_tail = std::make_shared<Node>();
        dummy_head->next = dummy_tail;
        tail = dummy_head;
    }


    // push_front: Insert at the beginning.
void push_front(const T& val) {
    // Create the new node with the given value.
    auto newNode = std::make_shared<Node>(val);
    
    {
        // Lock dummy_head and the node immediately after it (which might be dummy_tail).
        // This prevents other threads from modifying the front of the list concurrently.
        std::scoped_lock lock(dummy_head->mutex, dummy_head->next->mutex);
        
        // Determine if the list was empty (i.e. dummy_head->next is dummy_tail).
        bool wasEmpty = (dummy_head->next == dummy_tail);
        
        // Save the current first node in a temporary variable.
        auto oldFirst = dummy_head->next;
        
        // Insert newNode between dummy_head and oldFirst.
        newNode->next = oldFirst;
        
        // Update dummy_head to point to newNode.
        dummy_head->next = newNode;
        
        // If the list was not empty, update the old first node’s backward pointer.
        
        // If the list was empty, then tail (which is currently dummy_head) must be updated.
        if (wasEmpty) {
            // Since tail == dummy_head in an empty list, and dummy_head is already locked,
            // we can safely update tail to newNode without acquiring an additional lock.
            tail = newNode;
        }
    }
}



void push_back(const T& val) {
    auto newNode = std::make_shared<Node>(val);
    std::scoped_lock lock(tail->mutex, dummy_tail->mutex);
    tail->next = newNode;
    newNode->next = dummy_tail;
    tail = newNode;
}

bool remove_front() {
    // Lock dummy_head and the node after dummy_head.
    std::scoped_lock lock(dummy_head->mutex, dummy_head->next->mutex);
    auto toRemove = dummy_head->next;
    // If dummy_head->next is dummy_tail, then the list is empty.
    if (toRemove == dummy_tail) {
        return false;
    }
    // Remove the first node by linking dummy_head directly to toRemove->next.
    dummy_head->next = toRemove->next;
    // If, after removal, the list becomes empty, update tail to dummy_head.
    if (dummy_head->next == dummy_tail) {
        tail = dummy_head;
    }
    return true;
}


bool empty() const {
    std::scoped_lock lock(dummy_head->mutex);
    return dummy_head->next == dummy_tail;
}

};

// Issue w scoped lock:
// 1. cannot acquire the same lock twoce in a scoped lock
// 2. order still matters across seperate function calls

// Single vs. Multiple Lock Calls:
// When you use a single call like:
// the standard ensures that both mutexes are locked in a consistent order internally, regardless of the order in which you pass them.
// However, if one function locks (mutex1, mutex2) in one call and another function locks (mutex2, mutex1) in a separate call, the guarantee of std::scoped_lock does not extend across separate calls. This can allow one thread to lock mutex1 and then try to lock mutex2 while another thread does the opposite, causing a circular wait.

// Inconsistent Locking Order Across Functions:
// Even if each function uses a scoped_lock, if different functions acquire locks in different orders, then two threads executing different functions might end up holding one mutex each and waiting for the other. For example:

// Thread A calls a function that locks mutex1 then mutex2.

// Thread B calls another function that locks mutex2 then mutex1. If these two functions are executed concurrently, Thread A might acquire mutex1 and Thread B might acquire mutex2 before either can lock the second mutex. Now, A waits for mutex2 (held by B) and B waits for mutex1 (held by A), resulting in a deadlock.

// Locking the Same Mutex Twice:
// If due to the data structure invariants (for example, if a tail pointer sometimes equals the head pointer), a thread inadvertently tries to lock the same non-recursive mutex twice (even if in a single call it wouldn’t because of std::scoped_lock’s internal ordering), that will also lead to a deadlock or a runtime error. Non-recursive mutexes don’t allow the same thread to lock them multiple times.

// Future: thread safe Doubly singly list
// Could use a shared pointer for next (not unique as 2 nodes need to refer to it)
// and for prev we can use weak pointer instead of a shared pointer to avoid circular references
// might need to use .lock() to change the weak ptr to shared ptr though, when u access the prev node's data, modify the prev node's ptrs
// .lock() returns a temporary so this "new" shared ptr will be dealloacted when it goes out of scope



    



















