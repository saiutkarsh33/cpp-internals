#pragma once

#include <cstddef>   
#include <iterator>
#include <string>
#include <iostream>
#include <cstring> 
#include <string_view>   


// i chose arbitrary number - should be a multiple of 64 to fit into cache line

#define SSO_SIZE 127

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

    // Below, there is const in the arguments because
    // const in general accepts const and non const
    // but non const cannot accept const
    // You cannot modify the non const string passed into this const argument ctor
    // but doesent matter cause u donmt even wanna modify it

    // Construct from C-style string.
    String(const char* s);

    // Construct from C-style string with a given length.
    String(const char* s, size_t len);

    // Copy constructor.
    String(const String& other);

    // Move constructor.
    String(String&& other) noexcept;

    String(const std::string&);
    
    // Create a new string with the same contents as the sv string
    // You don't need "explicit" for this use case becase theres a char* ctor already
    // so u wont have a situation where char* is implictly converted to string view
    String(std::string_view sv);

    std::string_view first_token(char delim) const;
    String first_token_copy(char delim) const;

    bool starts_with(std::string_view prefix) const;
    bool starts_with(const String& prefix) const;
    bool starts_with(const std::string& prefix) const;

    std::string_view drop_prefix(size_t n) const;
    String drop_prefix_copy(size_t n) const;


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
    
    // Return true if needle is a substring of this String
    // (memcmp(haystack + i, needle, needle_len) == 0) sliding window under the hood.

    bool contains(std::string_view needle) const;
    bool contains(std::string needle) const;

    bool contains(const String& needle) const;
    // Return the size (length) of the string.
    size_t size() const;

    // Return a C-style null-terminated string.
    const char* c_str() const;

    bool starts_with_digit() const;

    // Clear the contents of the string.
    void clear();

    bool operator==(const String& other) const;
    bool operator==(const char* s) const;

    Iterator begin();
    Iterator end();
    const Iterator begin() const;
    const Iterator end() const;





};























