// String

#pragma once 

#include <cstddef> // for size_t
#include <iostream> //for ostream

class String {
public:

    // Marks the end of a string
    static const size_t npos = -1;

    // Default Constructor
    // REQUIRES: Nothing
    // MODIFIES: *this
    // EFFECTS: Creates an empty String
    String() : length(0), capacity(16) {
        buffer = new char[capacity];
        buffer[0] = '\0';
    }

    ~String() {
        delete[] buffer;
    }

    // String Literal / C String Constructor
    // REQUIRES: cstr is a null terminated C style String
    // MODIFIES: *this
    // EFFECTS: Creates a String with equivalent contents to cstr
    String(const char *cstr) {
        length=0;
        while(cstr[length]!='\0')length++;
        capacity = length * 2;
        buffer = new char[capacity];
        for (size_t i = 0; i < length; i++) {
            buffer[i] = cstr[i];
        }
        buffer[length] = '\0';
    }

    // Buffer Constructor
    // REQUIRES: cstr is a buffer with size >= len
    // MODIFIES: *this
    // EFFECTS: Creates a String with equivalent contents to cstr
    String(const char *cstr, const size_t len) {
        length=len;
        capacity = length * 2;
        buffer = new char[capacity];
        for (size_t i = 0; i < length; i++) {
            buffer[i] = cstr[i];
        }
        buffer[length] = '\0';
    }

    // Copy Constructor
    // REQUIRES: other is another String
    // MODIFIES: *this
    // EFFECTS: Creates a copy of String based on the input
    String( const String& other )
        : length( other.length ), capacity( other.capacity )
        {
        buffer = new char[ capacity ];
        for ( int i = 0; i < length; i++ )
            buffer[ i ] = other.buffer[ i ];
        }

    // Assignment Operator ( Deep Copy )
    // REQUIRES: other is another string
    // MODIFIES: *this
    // EFFECTS: Delete the content of *this and make a deep copy of other
    String& operator= ( const String& other )
        {
        if ( this == &other )
            return *this;
        delete [] buffer;
        length = other.length;
        capacity = other.capacity;
        buffer = new char[ capacity ];
        for ( int i = 0; i < length; i++ )
            buffer[ i ] = other.buffer[ i ];
        return *this;
        }

    // Char Literal / char constructor
    // REQUIRES: c is a char
    // MODIFIES: *this
    // EFFECTS: Creates a string with only one character c
    String( const char c )
        {
        length = 1;
        capacity = length * 2;
        buffer = new char[ capacity ];
        buffer[ 0 ] = c;
        }

    // Size
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns the number of characters in the String
    size_t size() const {
        return this->length;
    }

    // C String Conversion
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a pointer to a null terminated C String of *this
    const char *cstr() const {
        return buffer;
    }

    // Iterator Begin
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a random access iterator to the start of the String
    const char *begin() const {
        return buffer;
    }

    // Iterator End
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a random access iterator to the end of the String
    const char *end() const {
        return buffer + length;
    }

    // Element Access
    // REQUIRES: 0 <= i < size()
    // MODIFIES: Allows modification of the i'th element
    // EFFECTS: Returns the i'th character of the String
    char &operator[](size_t i) {
        if (i >= length) exit(1);
        return this->buffer[i];
    }

    const char &operator[](size_t i) const {
        if (i >= length) exit(1);
        return this->buffer[i];
    }

    // String Append
    // REQUIRES: Nothing
    // MODIFIES: *this
    // EFFECTS: Appends the contents of other to *this, resizing any
    //      memory at most once
    void operator+=(const String &other) {
        size_t total = length + other.length;
        if (capacity > total) {
            for (size_t i = 0; i < other.length; i++) buffer[i + length] = other.buffer[i];
        } else {
            capacity = total * 2;
            char *new_buffer = new char[capacity];
            for (size_t i = 0; i < length; i++) new_buffer[i] = buffer[i];
            for (size_t i = 0; i < other.length; i++) new_buffer[i + length] = other.buffer[i];
            delete[] buffer;
            buffer = new_buffer;
        }
        length = total;
        buffer[total] = '\0';
    }


    // Push Back
    // REQUIRES: Nothing
    // MODIFIES: *this
    // EFFECTS: Appends c to the String
    void pushBack(char c) {
        if (length + 1 >= capacity) expand();
        buffer[length] = c;
        length++;
        buffer[length] = '\0';
    }

    // Pop Back
    // REQUIRES: String is not empty
    // MODIFIES: *this
    // EFFECTS: Removes the last charater of the String
    void popBack() {
        if (length == 0) exit(1);
        if (length + 1 <= capacity / 2) shrink();
        length--;
        buffer[length] = '\0';
    }

    // Equality Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether all the contents of *this
    //    and other are equal
    bool operator==(const String &other) const {
        if (length != other.size()) return false;
        else {
            for (size_t i = 0; i < length; i++) {
                if (buffer[i] != other.buffer[i])
                    return false;
            }
        }
        return true;
    }

    // Not-Equality Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether at least one character differs between
    //    *this and other
    bool operator!=(const String &other) const {
        return !(*this == other);
    }

    // Less Than Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically less than other
    bool operator<(const String &other) const {
        if (*this == other) return false;
        size_t min_len = length < other.size() ? length : other.size();
        for (size_t i = 0; i < min_len; i++) {
            if (buffer[i] < other.buffer[i]) return true;
            else if (buffer[i] > other.buffer[i]) return false;
        }
        return length < other.size();
    }

    // Greater Than Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically greater than other
    bool operator>(const String &other) const {
        if (*this == other) return false;
        size_t min_len = length < other.size() ? length : other.size();
        for (size_t i = 0; i < min_len; i++) {
            if (buffer[i] > other.buffer[i]) return true;
            else if (buffer[i] < other.buffer[i]) return false;
        }
        return length > other.size();
    }

    // Less Than Or Equal Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically less or equal to other
    bool operator<=(const String &other) const {
        return (*this) == other or (*this) < other;
    }

    // Greater Than Or Equal Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically less or equal to other
    bool operator>=(const String &other) const {
        return (*this) == other or (*this) > other;
    }

private:
    size_t length;
    size_t capacity;
    char *buffer;

    void expand() {
        char *new_buffer = new char[capacity * 2];
        for (size_t i = 0; i < length; i++) {
            new_buffer[i] = buffer[i];
        }
        new_buffer[length] = '\0';
        delete[] buffer;
        buffer = new_buffer;
        capacity *= 2;
    }

    void shrink() {
        char *new_buffer = new char[capacity / 2];
        for (size_t i = 0; i < length; i++) {
            new_buffer[i] = buffer[i];
        }
        new_buffer[length] = '\0';
        delete[] buffer;
        buffer = new_buffer;
        capacity /= 2;
    }
};

std::ostream &operator<<(std::ostream &os, const String &s) {
    for (size_t i = 0; i < s.size(); i++) {
        os << *(s.begin() + i);
    }
    return os;
}
