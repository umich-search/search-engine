#pragma once

#include "IPost.h"
#include <iostream>
#include "../utility/string.h"
#include "../utility/Vector.h"
#include "params.h"

//#define MAX_POSTS 1000
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
    SyncIndex() {}
    SyncIndex(size_t initialElements) : postingsListOffset(initialElements, -1), postLocation(initialElements, -1){
    }
    ::vector < int64_t > postingsListOffset; // offset in postings list
    ::vector < int64_t > postLocation; // location in index
};

struct ConstructionData {
    size_t currDoc;
    size_t firstTermLoc; // location in index of first term
    size_t latestTermLoc;
};

struct CommonHeader{
    // TODO: us init list
    size_t numOfOccurence; //number of times the term occurs
    size_t numOfDocument; // number of doucments term occurs in
    String term;
    Type type; // type of token document is
};

struct CommonHeaderBlob{
    size_t numOfOccurence;
    size_t numOfDocument;
    Type type;
    char term[];
};

struct PostingList {};

struct TermPostingList {
    TermPostingList(){};
    TermPostingList(size_t syncSize) : syncIndex(syncSize) {
        //syncIndex(syncSize);
        //syncIndex = SyncIndex(syncSize);
    }
    CommonHeader header;
    ::vector< IPostTerm > posts;
    SyncIndex syncIndex;
};

struct TermPostingListRaw {
    const char* buffer;
    CommonHeaderBlob *header;
    size_t termOffset;
    size_t numOccurenceOffset; 
    size_t numDocumentsOffset;
    size_t typeOffset;
    size_t postingsListOffsetOffset;
    size_t postLocationOffset;
    size_t postsOffset;

    TermPostingListRaw(const char * b) {
        buffer = b + strlen(b) + 1;
        header = (CommonHeaderBlob *)buffer;
        /*
        termOffset = sizeof(int) + 2 * sizeof(size_t);
        numOccurenceOffset = sizeof(int);
        numDocumentsOffset = sizeof(int) + sizeof(size_t);
        typeOffset = 0;
         */
        postingsListOffsetOffset = sizeof(int) + 2 * sizeof(size_t) + strlen(header->term) + 1;// strlen(buffer + termOffset) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * 8; //TODO: Change based off num sync points//header->numOfOccurence;
        postsOffset = postLocationOffset + sizeof(size_t) * 8; //TODO: Same as abovevheader->numOfOccurence;
    }
    // TODO: Switch to smart pointers
    CommonHeaderBlob *getHeader() {
        return header;
    }
    
    size_t getPostingsListOffsetAt(size_t i) {
        //std::cout << "pploo: " << postingsListOffsetOffset << std::endl;
        return *(size_t *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
    }
    size_t getPostLocationAt(size_t i) {
        return *(size_t *)(buffer + postLocationOffset + sizeof(size_t) * i);
    }

    IPostTerm getPostAt(size_t i) {
        size_t a = sizeof(size_t);
        size_t b = sizeof(int64_t);
        const char * offset = buffer + postsOffset;
        size_t conv = *(size_t *)(buffer + postsOffset + sizeof(size_t) * i);
        return IPostTerm(*(size_t *)(buffer + postsOffset + sizeof(size_t) * i));
    }
};
/*
struct EndDocPostingListRaw {
    const char* buffer;
    CommonHeaderBlob *header;
    size_t termOffset;
    size_t numOccurenceOffset;
    size_t numDocumentsOffset;
    size_t typeOffset;
    size_t postingsListOffsetOffset;
    size_t postLocationOffset;
    size_t postsOffset;

    TermPostingListRaw(const char * b) {
        buffer = b + strlen(b) + 1;
        header = (CommonHeaderBlob *)buffer;
        
        termOffset = sizeof(int) + 2 * sizeof(size_t);
        numOccurenceOffset = sizeof(int);
        numDocumentsOffset = sizeof(int) + sizeof(size_t);
        typeOffset = 0;
         
        postingsListOffsetOffset = sizeof(int) + 2 * sizeof(size_t) + strlen(header->term) + 1;// strlen(buffer + termOffset) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * header->numOfOccurence;
        postsOffset = postLocationOffset + sizeof(size_t) * header->numOfOccurence;
    }
    // TODO: Switch to smart pointers
    CommonHeaderBlob *getHeader() {
        return header;
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
*/


struct EndDocPostingList {
   CommonHeader header;
   size_t DocID;
    ::vector< IPostEndDoc > posts;
    SyncIndex syncIndex;
};

size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits);
size_t seekTermTarget(TermPostingListRaw *postings, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints);


size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits);

void createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits);

void createSeekIndex(EndDocPostingList *postings, size_t startLoc, size_t numLowBits);

