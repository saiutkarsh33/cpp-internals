#include "String.hpp"
#include <cstring>   
#include <utility>   


String::String() {
    len = 0;
    sso = true;
    ptr = smallBuffer;
    smallBuffer[0] = '\0';
}

String::String(const char* s) {
    // need to have the guard cause pointers can be null unlike references
    if (s == nullptr) {
        return;
    }    
    size_t inputLen = std::strlen(s);
    if (inputLen <= SSO_SIZE) {
        sso = true;
        ptr = smallBuffer;
    } else {
        sso = false;
        ptr = new char[inputLen + 1];
    }
    len = inputLen;
    // copies a block of memory from s array to ptr array 
    std::memcpy(ptr, s, inputLen + 1);
}

String::String(const char* s, size_t l) : len(l) {
    // need to have the guard cause pointers can be null unlike references
    if (s == nullptr) {
        return;
    }    
    if (l <= SSO_SIZE) {
        sso = true;
        ptr = smallBuffer;
    } else {
        sso = false;
        ptr = new char[l + 1];
    }
    // copying only l characters -> so u dont copy the null terminator
    std::memcpy(ptr, s, l);
    ptr[l] = '\0';
}

// Copy constructor.
String::String(const String& other) : len(other.len) {
    if (other.len <= SSO_SIZE) {
        sso = true;
        ptr = smallBuffer;
    } else {
        sso = false;
        ptr = new char[other.len + 1];
    }
    std::memcpy(ptr, other.ptr, other.len + 1);
}

// Move constructor.
String::String(String&& other) noexcept : len(other.len), sso(other.sso) {
    if (other.sso) {
        // If other is in SSO mode, copy its buffer.
        ptr = smallBuffer;
        std::memcpy(ptr, other.ptr, other.len + 1);
    } else {
        // Steal the allocated buffer.
        ptr = other.ptr;
        other.ptr = other.smallBuffer;  // set other into SSO mode
        other.sso = true;
    }
    other.len = 0;
    // mnaking first index null terminator will make it empty string,
    // no need to clear all the characters from the moved out buffer
    other.smallBuffer[0] = '\0';
}


String& String::operator=(const String& other) {
    if (this != &other) {
        String temp(other);
        // swap is exception safe bc it only performs low level non throwing operations like swapping pointers and no dynamic memory manipulation
        swap(temp);
    }
    return *this;
}

// Move assignment operator.
String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        if (!sso) {
            delete[] ptr;
        }
        // Move data from other.
        len = other.len;
        sso = other.sso;
        if (other.sso) {
            ptr = smallBuffer;
            std::memcpy(ptr, other.ptr, other.len + 1);
        // dont have to delete memory and allocate new memory, just shifting
        // the pointers suffice for this use case!
        } else {
            ptr = other.ptr;
            other.ptr = other.smallBuffer; // other goes to SSO mode
            other.sso = true;
        }
        other.len = 0;
        other.smallBuffer[0] = '\0';
    }
    return *this;
}

void String::swap(String& other) {

    // When both use SSO, swap the internal buffers.
    if (this->sso && other.sso) {
        char temp[SSO_SIZE + 1];
        // you cant just swap the 2 smallbuffersv bc they arent pointers but rather actual arrays in memory
        std::memcpy(temp, this->smallBuffer, SSO_SIZE + 1);
        std::memcpy(this->smallBuffer, other.smallBuffer, SSO_SIZE + 1);
        std::memcpy(other.smallBuffer, temp, SSO_SIZE + 1);
        std::swap(this->len, other.len);
    }
    // When one uses SSO and the other does not.
    else if (this->sso && !other.sso) {
        // Swap pointers and flags.
        char* tempPtr = other.ptr;
        char tempSmall[SSO_SIZE + 1];
        std::memcpy(tempSmall, this->smallBuffer, SSO_SIZE + 1);
        other.ptr = this->smallBuffer;
        this->ptr = tempPtr;
        std::swap(this->sso, other.sso);
        std::swap(this->len, other.len);
        std::memcpy(this->smallBuffer, other.ptr, (this->len < SSO_SIZE ? this->len + 1 : SSO_SIZE + 1));
        std::memcpy(other.smallBuffer, tempSmall, SSO_SIZE + 1);
    }
    else if (!this->sso && other.sso) {
        other.swap(*this);
    }
    // When both are not using SSO.
    else {
        std::swap(this->ptr, other.ptr);
        std::swap(this->len, other.len);
    }
}

// Append a C-style string.

// this function is a masterclass on memory movement!
String& String::append(const char* s) {
    size_t addLen = std::strlen(s);
    size_t newLen = len + addLen;

    // Allocate temporary buffer for new content.
    char* newBuffer = nullptr;
    bool useSSO = (newLen <= SSO_SIZE);

    if (useSSO) {
        newBuffer = smallBuffer;  // Use our internal buffer.
    } else {
        newBuffer = new char[newLen + 1];
    }

    // Copy existing content.
    std::memcpy(newBuffer, ptr, len);
    // Append the new string.
    std::memcpy(newBuffer + len, s, addLen);
    newBuffer[newLen] = '\0';

    // Free old heap memory if used.
    if (!sso) {
        delete[] ptr;
    }

    len = newLen;
    sso = useSSO;
    if (!sso) {
        ptr = newBuffer;
    } else {
        ptr = smallBuffer;  // Already pointing to smallBuffer.
        // In case we allocated a temporary buffer (unlikely here) we already copied into smallBuffer.
        // For safety, if newBuffer != smallBuffer, copy the contents.
        if (newBuffer != smallBuffer) {
            std::memcpy(smallBuffer, newBuffer, newLen + 1);
            delete[] newBuffer;  // Clean up temporary allocation.
        }
    }

    return *this;
}

// Append another String.
String& String::append(const String& s) {
    return append(s.c_str());
}

String& String::operator+=(const char* s) {
    return append(s);
}


char& String::operator[](size_t pos) {
    // actually i should check bounds?

    // [] on a pointer essentially does pointer arithmetic
    return ptr[pos];
}

size_t String::size() const {
    return len;
}

const char* String::c_str() const {
    return ptr;
}

void String::clear() {
    if (!sso) {
        delete[] ptr;
    }
    len = 0;
    sso = true;
    ptr = smallBuffer;
// In C-style strings, the string is defined as ending at the first null terminator ('\0'). When you set smallBuffer[0] = '\0', you're indicating that there are zero characters before the terminator, making it an empty string.
    smallBuffer[0] = '\0';
}

bool String::operator==(const String& other) const {
    if (len != other.len) {
        return false;
    }
    return std::memcmp(ptr, other.ptr, len) == 0;
}

bool String::operator==(const char* s) const {
    return std::strcmp(ptr, s) == 0;
}

// --- Iterator Support ---
String::Iterator String::begin() { 
    return Iterator(ptr); 
}

String::Iterator String::end() { 
    return Iterator(ptr + len); 
}

// when you do const, only the member functions that are const remain.

const String::Iterator String::begin() const { 
    // treated as a pointer to const
    // its used when the string is const
    return Iterator(const_cast<char*>(ptr)); 
}

const String::Iterator String::end() const { 
    // treated as a pointer to const
    // its used when the string is const
    return Iterator(const_cast<char*>(ptr + len)); 
}

// --- Destructor ---
String::~String() {
    if (!sso) {
        delete[] ptr;
    }
    // the rest of thr stuff is not dynamic memory on the heap, does not need to be deleted.
}







