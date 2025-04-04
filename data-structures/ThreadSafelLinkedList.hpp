
#include <memory>
#include <mutex>

template <typename T>

// Doubly linked list

// Decently fine grained : For each node

class ThreadSafeLinkedList {

private:
    struct Node {
        std::shared_ptr<T> value;

// 'next' is a std::shared_ptr because it represents ownership of the next node in the list.
// - Shared ownership is needed here since multiple parts of the code 
//   might need to hold a reference to this node. (next and prev)
// - Using std::unique_ptr for 'next' ensures exclusive ownership along the forward chain, but if you also
//   tried to use a unique_ptr for 'prev', you’d end up with two unique_ptrs (one for 'next' and one for 'prev')
//   attempting to own the same node. Unique pointers cannot be copied, so this would lead to compilation errors
//   or require transferring ownership, which isn’t what you want in a doubly linked list.

// 'prev' is a std::weak_ptr because it provides a non-owning (backward) reference to the previous node.
// - This avoids creating a strong (shared) ownership cycle that would occur if 'prev' were a std::shared_ptr.
//   Such a cycle would cause a memory leak since the reference counts would never drop to zero.
// - A weak pointer lets you observe and access the previous node (by locking it to get a std::shared_ptr)
//   without affecting its lifetime, which is both safer and more efficient.
// - Compared to a raw pointer, using std::weak_ptr makes the ownership semantics clearer and integrates better
//   with the rest of the smart pointer system, especially if you later need to detect when the node has been destroyed.


        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;

        mutable std::mutex mutex;
        
        // Constructor
        Node(const T& val): value(std::make_unique<T>(val)), next(nullptr), prev(nullptr) {}

    };

    std::unique_ptr<Node> dummy_head;

    std::unique_ptr<Node> dummy_tail;

public: 

    ThreadSafeLinkedList() {
        dummy_head = std::make_shared<Node>();
        dummy_tail = std::make_shared<Node>();
        dummy_head->next = dummy_tail;
        // you can assign a shared ptr to a weak ptr in cpp
        dummy_tail->prev = dummy_head;
    }


    // push_front: Insert at the beginning.
    void push_front(const T& val) {
        // lock both at the same to prevent deadlocks
        std::scoped_lock lock(dummy_head->mutex, dummy_head->next->mutex);
        auto newNode = std::make_shared<Node>(val);
        // reference below to avoid copying, you are accessing the shared pointer
        // if next was a unique ptr it wld cause an issue, here its shared ptr so still okay to copy (now 2 pointers point at the same object), but no biggie
        // dont do const reference because you are modifying this
        auto& prevLast = dummy_head->next;
        dummy_head->next = newNode;
        newNode-> prev = dummy_head;
        newNode-> next = prevLast;
        prevLast-> prev = newNode;

    }

    // The other methods just use this method^'s principles

    // If you want to edit in the middle, need to use iterator





    


















};
