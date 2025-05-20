#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>

template <typename T>

class Vector {
private:
    T* ptr;
    size_t m_capacity;
    size_t idx; // this is the "current size"

public:

    // Default constructor

    // we use raw pointers here bc we want more flexibility and control, when memory is allocated and deallocated

    // ptr is the starting address of the allocated memory block for this vector

    Vector(): ptr(nullptr), m_capacity(0), idx(0) {}

    // Normal constructor

    Vector(size_t size, const T& element): ptr(nullptr), m_capacity(0), idx(0) {
        resize(size);
        for (size_t i = 0; i < size; i++) {
            // allocating the memory at this location with a new element 
            // T(element) uses the copy constructor
            new (&ptr[idx++]) T(element);
        }
    }

    // initialiser list constructor

    Vector(std::initializer_list<T> lst): ptr(nullptr), m_capacity(0), idx(0) {
        resize(lst.size());
        for (const auto& i: lst) {
            // & operator gets the address of the element at x basically
            new (&ptr[idx++]) T(i);
        }
    }

    // copy constructor

    Vector(const Vector& vec): ptr(nullptr), m_capacity(0), idx(0) {
        resize(vec.m_capacity);
        for (size_t i = 0; i < vec.idx; i++) {
            new (&ptr[idx++]) T(vec.ptr[i]);
        }
    }

    // move constructor

        Vector(Vector&& vec): ptr(vec.ptr), m_capacity(vec.m_capacity), idx(vec.idx) {
        vec.ptr = nullptr;
        vec.m_capacity = 0;
        vec.idx = 0;        
    }

    // copy assignment

    // copy the vector then swap

    // if anything goes wrong during copying, vec remains unchanged cause ure copying

    Vector& operator=(const Vector& vec) {
        Vector v{vec};
        swap(v);
        return *this;
    }

    // move assignment

    Vector& operator=(Vector&& vec) {
        // cast into an r value
        Vector v{std::move(vec)};
        swap(v);
        return *this;
    }

    void push_back(const T& val) {
        if (idx == m_capacity) {
            resize(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        new (&ptr[idx++]) T(val);
    }

    void pop_back() {
        if (idx > 0) {
            // call destructor of last element, so array remains contiguous
            ptr[idx - 1].~T();
            idx--;
        }
    }

    void clear() {
        for (int i = 0; i < idx; i++) {
            ptr[i].~T();
        }
        idx = 0;
    }

    // Takes in arguments that is of a variable type U, and creates a T using those arguments

    // std::forward uses perfect forwarding: if what was given was an rvalue, and rvalue is gonna be passed to T'S Constructor
    // and hence do a move constructor, but it what was given was an lvalue, it wld call T's copy constructor instead

    // Without perfect forwarding, if rvalue was passed, it would be transformed to an lvalue args and then copied over, unnecessary copies!

    template <typename... U>
    requires std::constructible_from<T, U...>
    T& emplace_back(U&&... args) {
        if (idx == m_capacity) {
            resize(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        new (&ptr[idx++]) T(std::forward<U>(args)...);
        return ptr[idx - 1];
    }

    template<typename... U>
    requires std::constructible_from<T, U...>
    T& emplace_insert(size_t pos, U&&... args) {
        if (pos > idx) 
            throw std::out_of_range("Vector::emplace_insert: position out of range");

    // 1) grow if needed
        if (idx == m_capacity) 
            resize(m_capacity == 0 ? 1 : m_capacity * 2);

    // 2) shift existing elements [pos..idx-1] → [pos+1..idx]
        for (size_t i = idx; i > pos; --i) {
            // backwards!!!
            new (&ptr[i]) T(std::move(ptr[i - 1]));
            ptr[i - 1].~T();
        }

    // 3) in‐place construct the new T at position pos
        new (&ptr[pos]) T(std::forward<U>(args)...);

    // 4) update size & return
        ++idx;
        return ptr[pos];
    }   

    // insert an initializer_list of T’s at position pos
    void insert(size_t pos, std::initializer_list<T> il) {
        if (pos > idx)
        throw std::out_of_range("Vector::insert: position out of range");

        size_t count = il.size();
        if (count == 0) return;

        // 1) grow once for all `count` new elements
        if (idx + count > m_capacity) {
        // double-or-enough strategy
            size_t newCap = std::max(m_capacity ? m_capacity * 2 : 1, idx + count);
            resize(newCap);
        }

        // 2) shift old tail [pos..idx-1] → [pos+count..idx+count-1]
        for (size_t i = idx; i > pos; --i) {
            new (&ptr[i + count - 1]) T(std::move(ptr[i - 1]));
            ptr[i - 1].~T();
        }

        // 3) copy-construct the new elements from the list
        size_t i = 0;
        for (auto& x : il) {
            new (&ptr[pos + i]) T(x);
            ++i;
        }

        // 4) update size
        idx += count;
    }

    void resize(size_t count) {
        if (count == m_capacity) {
            return;
        }

        if (m_capacity > count) {
        // Destroy elements beyond the new size if they exist
        for (size_t i = count; i < idx; i++) {
            ptr[i].~T();
        }
        idx = std::min(count, idx);
        return;
        }


        // ::operator new basically allocates a block of raw memory without invoking any
        // constructors

        // the return type of ::operator new is void,
        // reinterpret cast casts it into T, and tells compiler to treat it 
        // as if it were of type T

        T* newPtr = reinterpret_cast<T*>(::operator new(sizeof(T) * count));

        // moving is more efficient than copying
        // standard practice to use std::move when the source object does not matter anywhere
        // signals u dont need a deep copy, compiler can optimise
        for (size_t i = 0; i < idx; i++) {
            new (&newPtr[i]) T(std::move(ptr[i]));
        }

        //Call destructors for old memory
        for (size_t i = 0; i < idx; i++) {
            ptr[i].~T();
        }
        
        // free this vector from memory
        // destroying the T objects above just freed up space for "this" array
        // now we are destroying "this" array
        ::operator delete(ptr);
        ptr = newPtr;
        m_capacity = count;
    }

    size_t size() const {
        return idx;
    }

    size_t capacity() const {
        return m_capacity;
    }
    T* data() {
        return ptr;
    }
    T& at(size_t pos) {
        return ptr[pos];
    }

    T& operator[](size_t pos) {
        // can throw out of bounds exception instead actually
        if (pos >= idx) {
            throw std::out_of_range("Index out of range");
        }

        return ptr[pos];
    }
    
    void swap(Vector& vec) {
        std::swap(ptr, vec.ptr);
        std::swap(m_capacity, vec.m_capacity);
        std::swap(idx, vec.idx);
    }

};
