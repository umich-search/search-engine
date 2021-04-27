// bitmap

#pragma once 

#include "Vector.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cassert>
#include <string.h>
#include <cstdint>
//#include <malloc.h>
#include <sys/mman.h>
#include "mString.h"

class bitmap {
public:
    bitmap() : bitSize(0) {
        v = new vector<int>;
    }

    ~bitmap() {
        delete v;
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

    size_t size() const {
        return bitSize;
    }
private:
    size_t bitSize;
    vector<int> *v;
};

class bitmap_f  // bitmap in file
    {
    private:
        String f_name;
        size_t bitSize;
        uint32_t *v;
        size_t mapSize;

    public:
        // ctor needs absolute path to the file
        bitmap_f( const char *filename, size_t numBits );
        ~bitmap_f( );
        void set( size_t i );
        bool isTrue( size_t i ) const;
        size_t size( ) const;
    };