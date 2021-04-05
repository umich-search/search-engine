
#pragma once
#include "stdlib.h"
#define NUM_SYNC_POINTS 11
#define MAX_SYNC_DISTANCE 100
#define MAX_WORD_LENGTH 100
#define MAX_TITLE_LENGTH 512
#define MAX_URL_LENGTH 2048
#define CHUNK_SIZE_BYTES 10000
// TODO: Move somewhere else eventually
#define DOCUMENT_SIZE 2576
//#include "../utility/Common.h"
typedef size_t Location;
// Should be large enough to fit max word occurence
typedef uint64_t w_Occurence;
// Should be large enough to fit max document occurence
typedef uint32_t d_Occurence;
//
typedef size_t Offset;
