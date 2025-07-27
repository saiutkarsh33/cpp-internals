# C++ Internals — Custom Implementations by Sai Utkarsh

Welcome to my personal collection of C++ internals, where I’ve re-implemented fundamental data structures and utilities from scratch. Almost everything here is tested using `gtest`, with a strong focus on thread safety, memory management, and custom iterators.

I’ve also documented key learnings and insights on my [tech blog](https://saiutkarsh.netlify.app).

---

## Data Structures

### Vector
- Custom implementation with:
  - `resize()`, `emplace_back()`
  - Copy-and-swap idiom
- Fully unit-tested using `gtest`

### HashMap
- Uses an array of linked lists (separate chaining)
- Custom iterator class
- `[]` operator overload, CRUD, and `rehash()` support
- Extensively tested with `gtest`

### Thread-Safe Queue
- Uses `std::mutex` and `std::condition_variable`
- Designed for multithreaded producer-consumer scenarios
- Unit-tested with `gtest`

### Thread-Safe Linked List
- Dummy head & tail nodes
- Smart pointers inside nodes
- Unit-tested with `gtest`

### Intrusive Linked List
- Based on CRTP
- Uses raw pointers with dummy head and tail
- Custom iterator implemented

### FlatMap
- Sorted vector under the hood
- Uses `lower_bound` for fast insertions

### BiMap (WIP)
- Partial implementation
- Open for community contributions

---

## Smart Pointers

### UniquePointer
- Custom ownership semantics
- Fully unit-tested

### SharedPointer
- Custom `ControlBlock` implementation
- Supports `make_shared` with `InPlaceControlBlock`
- No `enable_shared_from_this` (PRs welcome)
- Unit-tested

### WeakPointer
- Supports locking and non-owning semantics

---

## Custom String

- Backed by a dynamic `char*` array with Short String Optimization (SSO)
- Constructible from:
  - `std::string`
  - C-style strings
  - `std::string_view`
- Key features: `append()` and `swap()`
- Fully unit-tested

---

## Multithreaded Program

- A simple multithreaded program to demonstrate concurrency utilities in action.

---


## Blog

Read more at [saiutkarsh.netlify.app](https://saiutkarsh.netlify.app)
