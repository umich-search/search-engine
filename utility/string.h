// string

#pragma once 

#include <cstddef> // for size_t
#include <iostream> //for ostream

class string {
public:

    // Default Constructor
    // REQUIRES: Nothing
    // MODIFIES: *this
    // EFFECTS: Creates an empty string
    string() : length(0), capacity(16) {
        buffer = new char[capacity];
        buffer[0] = '\0';
    }

    ~string() {
        delete[] buffer;
    }

    // string Literal / C string Constructor
    // REQUIRES: cstr is a null terminated C style string
    // MODIFIES: *this
    // EFFECTS: Creates a string with equivalent contents to cstr
    string(const char *cstr) {
        length=0;
        while(cstr[length]!='\0')length++;
        capacity = length * 2;
        buffer = new char[capacity];
        for (size_t i = 0; i < length; i++) {
            buffer[i] = cstr[i];
        }
        buffer[length] = '\0';
    }

    // Size
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns the number of characters in the string
    size_t size() const {
        return this->length;
    }

    // C string Conversion
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a pointer to a null terminated C string of *this
    const char *cstr() const {
        return buffer;
    }

    // Iterator Begin
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a random access iterator to the start of the string
    const char *begin() const {
        return buffer;
    }

    // Iterator End
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns a random access iterator to the end of the string
    const char *end() const {
        return buffer + length;
    }

    // Element Access
    // REQUIRES: 0 <= i < size()
    // MODIFIES: Allows modification of the i'th element
    // EFFECTS: Returns the i'th character of the string
    char &operator[](size_t i) {
        if (i >= length) exit(1);
        return this->buffer[i];
    }

    // string Append
    // REQUIRES: Nothing
    // MODIFIES: *this
    // EFFECTS: Appends the contents of other to *this, resizing any
    //      memory at most once
    void operator+=(const string &other) {
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
    // EFFECTS: Appends c to the string
    void pushBack(char c) {
        if (length + 1 >= capacity) expand();
        buffer[length] = c;
        length++;
        buffer[length] = '\0';
    }

    // Pop Back
    // REQUIRES: string is not empty
    // MODIFIES: *this
    // EFFECTS: Removes the last charater of the string
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
    bool operator==(const string &other) const {
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
    bool operator!=(const string &other) const {
        return !(*this == other);
    }

    // Less Than Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically less than other
    bool operator<(const string &other) const {
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
    bool operator>(const string &other) const {
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
    bool operator<=(const string &other) const {
        return (*this) == other or (*this) < other;
    }

    // Greater Than Or Equal Operator
    // REQUIRES: Nothing
    // MODIFIES: Nothing
    // EFFECTS: Returns whether *this is lexigraphically less or equal to other
    bool operator>=(const string &other) const {
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

std::ostream &operator<<(std::ostream &os, const string &s) {
    for (size_t i = 0; i < s.size(); i++) {
        os << *(s.begin() + i);
    }
    return os;
}
