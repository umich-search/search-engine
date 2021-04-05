#pragma once

#include <iostream> // TODO: Remove

#include "params.h"
#include "IPost.h"
#include "../utility/string.h"
#include "../utility/Vector.h"


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
    Location firstTermLoc; // location in index of first term
    Location latestTermLoc;
};

struct CommonHeader{
    CommonHeader () : numOfOccurence(0), numOfDocument(0) {}
    // TODO: us init list
    w_Occurence numOfOccurence; //number of times the term occurs
    d_Occurence numOfDocument; // number of doucments term occurs in
    String term;
    Type type; // type of token document is
};

struct CommonHeaderBlob{
    w_Occurence numOfOccurence;
    d_Occurence numOfDocument;
    Type type;
    char term[];
};

struct PostingList {};

struct TermPostingList {
    TermPostingList(){};
    TermPostingList(size_t syncSize) : syncIndex(syncSize) {}
    CommonHeader header;
    ::vector< IPostTerm > posts;
    SyncIndex syncIndex;
};

struct TermPostingListRaw {
    const char* buffer;
    CommonHeaderBlob *header;
    Offset termOffset;
    Offset numOccurenceOffset;
    Offset numDocumentsOffset;
    Offset typeOffset;
    Offset postingsListOffsetOffset;
    Offset postLocationOffset;
    Offset postsOffset;

    TermPostingListRaw(const char * b) {
        buffer = b + strlen(b) + 1;
        header = (CommonHeaderBlob *)buffer;
        postingsListOffsetOffset = sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence) + strlen(header->term) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * 8; //TODO: Change based off num sync points//header->numOfOccurence;
        postsOffset = postLocationOffset + sizeof(size_t) * 8; //TODO: Same as abovevheader->numOfOccurence;
    }
    // TODO: Switch to smart pointers
    CommonHeaderBlob *getHeader() {
        return header;
    }
    
    Offset getPostingsListOffsetAt(Offset i) {
        //std::cout << "pploo: " << postingsListOffsetOffset << std::endl;
        return *(Offset *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
    }
    Offset getPostLocationAt(Offset i) {
        return *(Offset *)(buffer + postLocationOffset + sizeof(size_t) * i);
    }

    IPostTerm getPostAt(Offset i) {
        return IPostTerm(*(Offset *)(buffer + postsOffset + sizeof(size_t) * i));
    }
};


struct EndDocPostingListRaw {
    // TODO: Remove superflous members
    const char* buffer;
    CommonHeaderBlob *header;
    Offset termOffset;
    Offset numOccurenceOffset;
    Offset numDocumentsOffset;
    Offset typeOffset;
    Offset postingsListOffsetOffset;
    Offset postLocationOffset;
    Offset postsOffset;

    EndDocPostingListRaw(const char * b) {
        buffer = b;
        header = (CommonHeaderBlob *)buffer;
        postingsListOffsetOffset = sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence) + strlen(header->term) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * 8; //TODO: Change based off num sync points//header->numOfOccurence;
        postsOffset = postLocationOffset + sizeof(size_t) * 8; //TODO: Same as abovevheader->numOfOccurence;
    }
    // TODO: Switch to smart pointers
    CommonHeaderBlob *getHeader() {
        return header;
    }
    
    Offset getPostingsListOffsetAt(Offset i) {
        //std::cout << "pploo: " << postingsListOffsetOffset << std::endl;
        return *(Offset *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
    }
    Offset getPostLocationAt(Offset i) {
        return *(Offset *)(buffer + postLocationOffset + sizeof(size_t) * i);
    }

    IPostTerm getPostAt(Offset i) {
        return IPostTerm(*(Offset *)(buffer + postsOffset + sizeof(size_t) * i));
    }
};

struct EndDocPostingList {
    EndDocPostingList(size_t syncSize) : syncIndex(syncSize) {}
   CommonHeader header;
   size_t DocID;
    ::vector< IPostEndDoc > posts;
    SyncIndex syncIndex;
};

Location seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits);
Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints);
Location seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits);
Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints);

void createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits);

void createSeekIndex(EndDocPostingList *postings, size_t startLoc, size_t numLowBits);

