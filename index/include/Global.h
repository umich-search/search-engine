
#pragma once

#include "../utility/Vector.h"
#include "../utility/string.h"
#include "../utility/SmartPointer.h"
#include "../utility/UTF8Conv.h"
#include "CommonUtil.h"
#include "stdlib.h"
#include "HashTable.h"

// Max number of sync points
#define NUM_SYNC_POINTS 8
// Size of bytes to write in memory before writing to disk
#define CHUNK_SIZE_BYTES 10000
// titles > MAX_TITLE_LENGTH will be cutoff
#define MAX_TITLE_LENGTH 512
// Documents with URLs > MAX_URL_LENGTH will be skipped
#define MAX_URL_LENGTH 2048
// Document Size = MAX_TITLE_LENGTH + MAX_URL_LENGTH + 2 * sizeof(Location)
#define DOCUMENT_SIZE 2576
// Maximum length of local machine pathname
#define MAX_PATHNAME_LENGTH 4096
// Storage locations
#define CHUNKS_METADATA_PATH "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/chunks_metadata.txt"
#define CHUNK_DIRECTORY "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/"
#define DOCS_DIRECTORY "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/"
#define TERM_COUNT_PATH "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/term_count.map"
// Whether or not to write to disk (set to false for in-memory tests)
#define WRITE_TO_DISK true
// Whether or not to write every chunk bytes ( used for testing)
#define USE_CHUNK_LIMIT true

// Should be large enough to define unique positions for every single term and endDoc
typedef size_t Location;
// Should be large enough to fit max word occurence
typedef uint64_t w_Occurence;
// Should be large enough to fit max document occurence
typedef uint32_t d_Occurence;
// Should be large enough for offsets, possibly across chunks
typedef size_t Offset;
// Size of hash value
typedef uint64_t fnvHash_t;
// Type of document used
enum Type{
    Document,
    URL,
    Anchor,
    Title,
    Body
};
