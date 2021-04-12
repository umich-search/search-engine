#pragma once

#include "Global.h"
#include "IPost.h"

typedef size_t Location;

struct SyncIndex {
    SyncIndex() {}
    SyncIndex(size_t initialElements) :
        postingsListOffset(initialElements, -1),
        postLocation(initialElements, -1)
        {}
    ::vector < int64_t > postingsListOffset;
    ::vector < int64_t > postLocation;
};

struct ConstructionData {
    size_t currDoc;
    Location firstTermLoc;
    Location latestTermLoc;
};

struct CommonHeader{
    CommonHeader () :
                numOfOccurence(0),
                numOfDocument(0)
                {}
    
    String term;
    w_Occurence numOfOccurence;
    d_Occurence numOfDocument;
    Type type;
};

struct TermPostingList {
    TermPostingList(){};
    TermPostingList(size_t syncSize) : syncIndex(syncSize) {}
    
    CommonHeader header;
    ::vector< IPostTerm > posts;
    SyncIndex syncIndex;
};


struct EndDocPostingList {
    EndDocPostingList(size_t syncSize) : syncIndex(syncSize) {}
   CommonHeader header;
   size_t DocID;
    ::vector< IPostEndDoc > posts;
    SyncIndex syncIndex;
};

Location seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits);
Location seekEndDocTarget(SharedPointer<EndDocPostingList> postings, size_t target, size_t &index, size_t numLowBits);

int createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits);

int createSeekIndex(SharedPointer<EndDocPostingList> postings, size_t startLoc, size_t numLowBits);


