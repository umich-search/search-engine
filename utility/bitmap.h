// bitmap

#pragma once 

#include "Vector.h"

class bitmap {
public:
    bitmap() : bitSize(0) {
        v = new vector<int>;
    }

    void resize(size_t numBits) {
        bitSize = numBits;
        v->resize(numBits / 32 + 1); // an int is 32 bits
    }

    // set the ith bit of bitmap to 1
    void set(size_t i) {
        (*v)[i>>5] |= (1<<(i & 31)); // i & 31 equals i % 32, set the (i%32) bit of a[i/32] to 1.
    }

    // determine if the ith bit of bitmap is 1
    bool isTrue(size_t i) {
        size_t r = 1 << (i & 31); //r represents remainder
        if(r == ((*v)[i>>5] & r))
            return true;
        else return false;
    }

    size_t size() {
        return bitSize;
    }
private:
    size_t bitSize;
    vector<int> *v;
};