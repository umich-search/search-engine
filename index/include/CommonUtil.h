#pragma once
#include "Global.h"

static size_t RoundUp( size_t length, size_t boundary )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   static const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }

static size_t UtfBytes(size_t a){
    size_t numBytes = 0;
    for (numBytes = 0; numBytes < 8; numBytes++) {
        if (a < (1 << (8 * numBytes + 5)))break;
    }
    numBytes += 1;
    return numBytes;
}

static size_t IntBytes(uint8_t *buffer) {
    size_t numBytes = ((*buffer) & 0xE0) >> 5;
    numBytes += 1;
    return numBytes;
}
// Caller guarantees buffer is large
static size_t IntToUtf(size_t a, uint8_t *buffer) {
    //return number of Bytes write into buffer
    size_t numBytes = 0;
    size_t z = a;
    for (numBytes = 0; numBytes < 8; numBytes++) {
        if (a < (1 << (8 * numBytes + 5)))break;
    }
    numBytes += 1;
    for (size_t i = numBytes - 1; i > 0; i--) {
        *(buffer + i) = size_t(z & 0xFF);
        z = z >> 8;
    }
    *buffer = z + ((numBytes - 1) << 5);
    return numBytes;
}

static size_t UtfToInt(const uint8_t *buffer) {
    size_t numBytes = ((*buffer) & 0xE0) >> 5;
    numBytes += 1;
    size_t a = 0;
    a += ((*buffer) & 0x1F) << ((numBytes - 1) * 8);
    for (int i = 1; i < numBytes; i++) {
        a += (*(buffer + i)) << ((numBytes - i - 1) * 8);
    }
    return a;
}

// returns number of low bits given total count and number of syncs
static size_t getNumLowBits( size_t count, size_t spacing ) 
    {
    size_t leftShift = ( count + spacing - 1 ) / spacing;
    int numLowBits=0;
    leftShift--;
    while(leftShift)
    {
        numLowBits++;
        leftShift=leftShift>>1;
    }
    return numLowBits;
}
