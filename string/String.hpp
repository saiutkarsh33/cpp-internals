#pragma once

#include <cstddef>   
#include <iterator>
#include <string>
#include <iostream>
#include <cstring>   


// i chose arbitrary number - should be a multiple of 64 to fit into cache line

#define SSO_SIZE 128

class String {
private:
    char* ptr; // start of the string
    size_t len; // size of the string
    // +1 for null terminator
    char smallBuffer[SSO_SIZE + 1];
    bool sso; // using sso if the string is <= 128 bytes
    void swap(String& other); // fr exception safe assignment


public:
    // bi directional iterator for the string
    struct Iterator {
    // public by default in a struct

        // custom types
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = char;
        using pointer           = char*;
        using reference         = char&;

        Iterator(pointer ptr) : m_ptr(ptr) {}
        reference operator*() const {return *m_ptr;}
        pointer operator->() { return m_ptr; }

        // Pre-increment
        Iterator& operator++() { ++m_ptr; return *this; }

        // Post-increment
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        // Pre-decrement
        Iterator& operator--() { --m_ptr; return *this; }
        // Post-decrement
        Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

        // == and != will be used by the String class, not Iterator, so need to be friend 
        friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; }        

    private:
        pointer m_ptr;
    };

    // Default constructor (creates an empty string).
    String();

    // Construct from C-style string.
    String(const char* s);

    // Construct from C-style string with a given length.
    String(const char* s, size_t len);

    // Copy constructor.
    String(const String& other);

    // Move constructor.
    String(String&& other) noexcept;


    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;

    ~String();

    // Append a C-style string.
    String& append(const char* s);
    // Append another String.
    String& append(const String& s);
    // Overload operator+= for C-style string.
    String& operator+=(const char* s);
    // Overload operator+= for String.
    String& operator+=(const String& s);

    // Non-const element access.
    char& operator[](size_t pos);
    // when the string is not const

    // Const element access.
    const char& operator[](size_t pos) const;
    // when the string is const

    // Return the size (length) of the string.
    size_t size() const;

    // Return a C-style null-terminated string.
    const char* c_str() const;


    // Clear the contents of the string.
    void clear();

    bool operator==(const String& other) const;
    bool operator==(const char* s) const;

    Iterator begin();
    Iterator end();
    const Iterator begin() const;
    const Iterator end() const;




};























