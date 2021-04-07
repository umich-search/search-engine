#pragma once
// Common code used by the various hashing sample applications.

// Nicole Hamilton  nham@umich.edu

#include "Vector.h"
#include "string.h"
uint32_t fnvHash( const char *data, size_t length )
    {
    static const size_t FnvOffsetBasis = 146959810393466560;
    static const size_t FnvPrime = 1099511628211ul;
    size_t hash = FnvOffsetBasis;
    for (size_t i = 0; i < length; ++i) 
        {
        hash *= FnvPrime;
        hash ^= data[i];
        }
    return hash;
    }

bool CompareEqual( const char *L, const char *R ) 
    {
    // Your code here.
    while (*L != '\0' || *R != '\0') 
        {
        if (*L != *R)return false;
        L += 1;
        R += 1;
        }
    return true;
    }

bool CompareEqual(const String &L,const String &R) {
    //cout << "In static ocmpare qequal" << endl;
    //std::cout << "comparing: " << L.cstr() << " and " << R.cstr() << std::endl;
    bool ret = strcmp(L.cstr(),R.cstr()) == 0;
    //cout << "Returning with : " << ret << endl;
    return ret;
}

size_t IntToUtf(size_t a, uint8_t *buffer, int count) {
    //return number of Bytes write into buffer
    size_t numBytes = 0;
    size_t z = a;
    for (numBytes = 0; numBytes < 8; numBytes++) {
        if (a < (1 << (8 * numBytes + 5)))break;
    }
    numBytes += 1;
    if (numBytes > count) return -1;
    for (size_t i = numBytes - 1; i > 0; i--) {
        *(buffer + i) = size_t(z & 0xFF);
        z = z >> 8;
    }
    *buffer = z + ((numBytes - 1) << 5);
    return numBytes;
}

size_t UtfToInt(size_t *a, const uint8_t *buffer, int count) {
    size_t numBytes = ((*buffer) & 0xE0) >> 5;
    numBytes += 1;
    if (numBytes > count) return -1;
    *a = 0;
    *a += ((*buffer) & 0x1F) << ((numBytes - 1) * 8);
    for (int i = 1; i < numBytes; i++) {
        *a += (*(buffer + i)) << ((numBytes - i - 1) * 8);
    }
    return numBytes;
}



extern bool optVerbose;
