
#pragma once

#include "stdlib.h"
#include "../utility/Vector.h"
#include "../utility/string.h"
#include "HashTable.h"
#include "../utility/SmartPointer.h"
#include "../utility/UTF8Conv.h"

#define NUM_SYNC_POINTS 8
//TODO: determine NUM_LOW_BITS
#define NUM_LOW_BITS 4
#define MAX_SYNC_DISTANCE 100
#define MAX_WORD_LENGTH 100
#define MAX_TITLE_LENGTH 512
#define MAX_URL_LENGTH 2048
#define DOCUMENT_SIZE 2576
#define CHUNK_SIZE_BYTES 100
#define CHUNKS_META_FILENAME "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/chunks_metadata.txt"
#define CHUNK_DIRECTORY "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/"
#define DOCS_DIRECTORY "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/"
#define TERM_COUNT_PATH "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/term_count.map"
#define WRITE_TO_DISK false
#define INITAL_HASHTABLE_SIZE 5
#define MAX_PATHNAME_LENGTH 4096

typedef size_t Location;
// Should be large enough to fit max word occurence
typedef uint64_t w_Occurence;
// Should be large enough to fit max document occurence
typedef uint32_t d_Occurence;
//
typedef size_t Offset;


enum Type{
    Document,
    URL,
    Anchor,
    Title,
    Body
};

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
