#pragma once
// Common code used by the various hashing sample applications.

// Nicole Hamilton  nham@umich.edu

#include "Vector.h"
#include "string.h"
fnvHash_t fnvHash( const char *data, size_t length )
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


extern bool optVerbose;
