#pragma once

#include "IPost.h"
#include <iostream>
#include "../utility/string.h"
#include "../utility/Vector.h"

//#define MAX_POSTS 1000
#define NUM_SYNC_POINTS 128 
#define MAX_SYNC_DISTANCE 100
#define MAX_WORD_LENGTH 100
#pragma once

typedef size_t Location;

enum Type{
    Document,
    URL,
    Anchor,
    Title,
    Body
};

// TODO: Need to figure out optimal size
struct SyncIndex {
    ::vector < Location > postingsListOffset; // offset in postings list
    ::vector < Location > postLocation; // location in index
};

struct ConstructionData {
    size_t currDoc;
    size_t firstTermLoc; // location in index of first term
    size_t latestTermLoc;
};

struct CommonHeader{
    // TODO: us init list
    size_t numOfDocument; // number of doucments term occurs in
    size_t numOfOccurence; //number of times the term occurs
    //char* term; // term
    String term;
    Type type; // type of token document is
    //size_t size; // size of entire postings list
};

struct PostingList {};

struct TermPostingList {
    TermPostingList(){};
    CommonHeader header;
    ::vector< IPostTerm > posts;
    SyncIndex syncIndex;
};

struct TermPostingListRaw {
    const char* buffer;
    size_t termOffset;
    size_t numOccurenceOffset; 
    size_t numDocumentsOffset;
    size_t typeOffset;
    size_t postingsListOffsetOffset;
    size_t postLocationOffset;
    size_t postsOffset;

    TermPostingListRaw(const char * b) {
        buffer = b + 4;
        termOffset = sizeof(int) + 2 * sizeof(size_t);
        numOccurenceOffset = sizeof(int);
        numDocumentsOffset = sizeof(int) + sizeof(size_t);
        typeOffset = 0;
        postingsListOffsetOffset = termOffset + strlen(buffer + termOffset) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * *(size_t *)(buffer + numOccurenceOffset);
        postsOffset = postLocationOffset + sizeof(size_t) * *(size_t *)(buffer + numOccurenceOffset);
    }
    const char *getTerm() {
        return buffer + termOffset;
    }
    size_t getNumDocuments() {
        return *(size_t *)(buffer + numDocumentsOffset);
    }
    size_t getNumOccurence() {
        return *(size_t *)(buffer + numOccurenceOffset);

    }
    Type getType() {
        return *(Type *)(buffer + typeOffset);
    }

    size_t getPostingsListOffsetAt(size_t i) {
        //std::cout << "pploo: " << postingsListOffsetOffset << std::endl;
        return *(size_t *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
    }
    size_t getPostLocationAt(size_t i) {
        return *(size_t *)(buffer + postLocationOffset + sizeof(size_t) * i);
    }

    IPostTerm getPostAt(size_t i) {
        return IPostTerm(*(size_t *)(buffer + postsOffset + sizeof(size_t) * i));
    }


};

struct EndDocPostingList {
   CommonHeader header;
   size_t DocID;
    ::vector< IPostEndDoc > posts;
    SyncIndex syncIndex;
};

size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits);

size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits);

void createSeekIndex(TermPostingList *postings, size_t numLowBits);

void createSeekIndex(EndDocPostingList *postings, size_t numLowBits);

