
#pragma once
#include "stdlib.h"
#include "../utility/Vector.h"
#include "../utility/string.h"
#include "../utility/HashTable.h"
#include "../utility/SmartPointer.h"

#define NUM_SYNC_POINTS 8
#define MAX_SYNC_DISTANCE 100
#define MAX_WORD_LENGTH 100
#define MAX_TITLE_LENGTH 512
#define MAX_URL_LENGTH 2048
#define DOCUMENT_SIZE 2576
#define CHUNK_SIZE_BYTES 100
#define CHUNKS_META_FILENAME "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/chunks_metadata.txt"
#define WRITE_TO_DISK false

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
