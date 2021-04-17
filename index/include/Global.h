
#pragma once

#include "Vector.h"
#include "mString.h"
#include "SmartPointer.h"
#include "CommonUtil.h"
#include "stdlib.h"
#include "HashTable.h"
#include "Common.h"

// Max number of sync points
#define NUM_SYNC_POINTS 64 // 8
// Size of bytes to write in memory before writing to disk
#define CHUNK_SIZE_BYTES 104857600 // 1000
// titles > MAX_TITLE_LENGTH will be cutoff
#define MAX_TITLE_LENGTH 512
// Documents with URLs > MAX_URL_LENGTH will be skipped
#define MAX_URL_LENGTH 2048
// Document Size = MAX_TITLE_LENGTH + MAX_URL_LENGTH + 2 * sizeof(Location)
#define DOCUMENT_SIZE 2576
// Maximum length of local machine pathname
#define MAX_PATHNAME_LENGTH 4096
// Storage locations
#define CHUNKS_METADATA_PATH "index_files/chunks_metadata.txt"
#define CHUNKS_METADATA_DIRECTORY "index_files/"
#define CHUNK_DIRECTORY "index_files/"
#define DOCS_DIRECTORY "index_files/"
// Whether or not to write to disk (set to false for in-memory tests)
//#define WRITE_TO_DISK true
// Whether or not to write every chunk bytes ( used for testing)
//#define USE_CHUNK_LIMIT false

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
