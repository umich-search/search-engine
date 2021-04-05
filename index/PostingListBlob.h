#pragma once

#include "Global.h"
#include "IPost.h"

typedef size_t Location;

struct CommonHeaderBlob{
    w_Occurence numOfOccurence;
    d_Occurence numOfDocument;
    Type type;
    char term[];
};

struct TermPostingListRaw {
    CommonHeaderBlob *header;
    Offset postingsListOffsetOffset;
    Offset postLocationOffset;
    Offset postsOffset;
    const char* buffer;

    TermPostingListRaw(const char * b) {
        buffer = b + strlen(b) + 1;
        header = (CommonHeaderBlob *)buffer;
        postingsListOffsetOffset = sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence) + strlen(header->term) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * NUM_SYNC_POINTS;
        postsOffset = postLocationOffset + sizeof(size_t) * NUM_SYNC_POINTS;
    }
    
    CommonHeaderBlob *getHeader();
    Offset getPostingsListOffsetAt(Offset i);
    Offset getPostLocationAt(Offset i);
    IPostTerm getPostAt(Offset i);
};


struct EndDocPostingListRaw {
    const char* buffer;
    CommonHeaderBlob *header;
    Offset postingsListOffsetOffset;
    Offset postLocationOffset;
    Offset postsOffset;

    EndDocPostingListRaw(const char * b) {
        buffer = b;
        header = (CommonHeaderBlob *)buffer;
        postingsListOffsetOffset = sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence) + strlen(header->term) + 1;
        postLocationOffset = postingsListOffsetOffset + sizeof(size_t) * NUM_SYNC_POINTS;
        postsOffset = postLocationOffset + sizeof(size_t) * NUM_SYNC_POINTS;
    }
    
    CommonHeaderBlob *getHeader();
    Offset getPostingsListOffsetAt(Offset i);
    Offset getPostLocationAt(Offset i);
    IPostTerm getPostAt(Offset i);
};

Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints);
Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints);



