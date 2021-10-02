#pragma once

#include "Global.h"
#include "IPost.h"
#include <cstring>

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

    TermPostingListRaw() {}
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
    // byteAfterPost will return the byte offset of 1 after the requested post
    IPostTerm getPostAt(Offset i, Offset &byteAfterPost);
    IPostTerm getPostAtByte(Offset numBytes, Offset &byteAfterPost);

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
    IPostEndDoc getPostAt(Offset i);
    // byteAfterPost will return the byte offset of 1 after the requested post
    IPostEndDoc getPostAt(Offset i, Offset &byteAfterPost);
    IPostTerm getPostAtByte(Offset numBytes, Offset &byteAfterPost);

};

Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t chunkSize);
Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t chunkSize);



