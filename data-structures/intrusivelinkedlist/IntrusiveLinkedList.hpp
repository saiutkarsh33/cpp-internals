// For a normal list, lets say of T objects, there is a Node wrapper struct around this "T" object, Node = {T obj, Node* Next}
// The nodes are specifically created for the list, and the list "owns" these nodes
// but for an instrusive linked list, the T object exist by themselves, and have T* next as a field

#pragma once

#include <cstddef>

// IntrusiveList expects T to have these members:
//   T* next{nullptr};
//   T* prev{nullptr};

// Alternatively, Can create a struct to inherit from:
// IntrusiveLink is templated on T (the derived class)
template<typename T>
struct IntrusiveLink {
    T* next = nullptr;
    T* prev = nullptr;
};
// CRTP: Curiously Recurring Template Pattern
// Its just a parent class with generic and type compatibility
// example usage: have a Person object
// struct Person : IntrusiveLink {
//     std::string name;
//     int age;
// };
template<typename T>

class IntrusiveList {
    T* head_{nullptr};
    T* tail_{nullptr};
public:
    IntrusiveList() = default;
    ~IntrusiveList() = default;

    void push_front(T* node) {
        node->prev = nullptr;
        node->next = head_;
        if (head_) head_->prev = node;
        else        tail_ = node;
        head_ = node;
    }

    void push_back(T* node) {
        node->next = nullptr;
        node->prev = tail_;
        if (tail_) tail_->next = node;
        else       head_ = node;
        tail_ = node;
    }

    void remove(T* node) {
        if (node->prev) node->prev->next = node->next;
        else  head_ = node->next;

        if (node->next) node->next->prev = node->prev;
        else  tail_ = node->prev;

        node->next = node->prev = nullptr;
    }

    bool empty() const { return head_ == nullptr; }
    T* front() const   { return head_; }
    T* back()  const   { return tail_; }

    // Iterator is used in range based for loops like for (auto x: list) 
    // becomes for (auto it = myList.begin(), end = myList.end(); it != end; ++it)
    // some STL algos like find() and for each() also use iterators

    struct iterator {
        // data: current node
        T* p;
        // explicit: if a T* p_ was provided in a function argument where an iterator object was expected,
        // compiler will automatically create an iterator object with that T* p_
        // In this case we dont want that cause generally we only want to use begin() and end() ie start and end of iterator
        // or a deliberate cast.
        explicit iterator(T* p_) : p(p_) {}
        // dereference
        T& operator*()  const { return *p; }
        // member access
        T* operator->() const { return p; }
        // pre increment
        iterator& operator++() {     
        p = p->next;
        return *this;
        }
        // post increment
        iterator operator++(int) {    // post-increment
        iterator tmp = *this;     // copy current position
        ++*this;                  // advance this iterator
        return tmp;               // return old position
        }

        bool operator!=(const iterator& o) const {
        return p != o.p;
        }

    }

    iterator begin() const { return iterator(head_); }
    iterator end()   const { return iterator(nullptr); }

}
// Instrusive linked list is generally afste

// Intrusive list
// • You already own or pre-allocate your objects (arenas, stack, static arrays).
// • You want the simplest possible pointer-only API: the object is the node.
// • You need absolute minimal indirection and no wrapper overhead.

// Pool-backed non-intrusive list (so cache thrashing is mitigated)
// • You like the std::list API (splice, merge, safe node ownership).
// • You’re willing to copy/move your objects into the nodes it allocates.
// • You just want lower allocation jitter without rewriting container logic.

// pros of intrusive list
// 1. Intrusive reduces thrashing: for non intrusive, each Node is new'd so nodes live scattered across the heap. So you traverse from one random cache line to another. there are a lot of cache evictions and misses - resulting in cache thrashes (flipping back n forth between different cache lines)
// However for intrusive, if its contiguous , dont have . technically u can have a pool backed non intrusive list as well

// 2. zero extra memory allocations (no new or delete for Node wrapper structs)
// In low latency scenarios: you typically pre allocate a pool of objects or place them on the stack, then all container operations are purely pointer manipulations
// Heap calls can be unpredictable so its best to minimise them: malloc or free cld take v long depending on lock contention, page faults, fragmentation (need to swap some pages to disk to make space)

// 3. For normal list, unless u have the node*, u need an O(n) traversal to find the node. for the instrusive, the object itself has next n prev ptrs

// 4. Multi container membership, by having eg LIST1 PREV LIST2 PREV etc in the hooks, object can reside in seevral lists

// Cons of instrusive list
// 1. T must have the pointer
// 2. if an element is destroyed when still linked, u get undefined behaviour when referencing it (u can gracefully hv a destructor that points data to null?)
// u must make sure the objects outlive the list memebership then, to prevent undefined behaviour

// a.next = b
// delete b
// when u call a.next, it wont auto return nullptr ! it still contains the old address where b used to live. its a dangling pointer
// if there is still smth there (another obj), itll return that when u derefence that memory address. if there is nth there, it has a segmentation fault.

// 3. u must write custom copy/move logic unles u want ur copied T to have the same next n prev ptrs as the original
// 4. u dont get all of std::list's rich API

