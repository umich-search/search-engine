#include "PostingListBlob.h"

CommonHeaderBlob * TermPostingListRaw::getHeader() {
    return header;
}

Offset TermPostingListRaw::getPostingsListOffsetAt(Offset i) {
    return *(Offset *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
}
Offset TermPostingListRaw::getPostLocationAt(Offset i) {
    return *(Offset *)(buffer + postLocationOffset + sizeof(size_t) * i);
}

IPostTerm TermPostingListRaw::getPostAt(Offset i) {
    uint8_t * curr = (uint8_t *)(buffer + postsOffset);
    for(size_t n = 0; n <= i; ++n) {
        size_t numBytes = ((*curr) & 0xE0) >> 5;
        numBytes += 1;
        if(n == i) {
            return IPostTerm(UtfToInt(curr));
        }
        curr = curr + numBytes;
    }
    throw "Searching for post beyond total posts size";
}

// byteAfterPost will return the byte offset of 1 after the requested post
IPostTerm TermPostingListRaw::getPostAt(Offset i, Offset &byteAfterPost) {
    uint8_t * curr = (uint8_t *)(buffer + postsOffset);
    Offset byteCount = 0;
    for(size_t n = 0; n <= i; ++n) {
        size_t numBytes = ((*curr) & 0xE0) >> 5;
        numBytes += 1;
        byteCount = numBytes;
        if(n == i) {
            byteAfterPost = byteCount;
            return IPostTerm(UtfToInt(curr));
        }
        curr = curr + numBytes;
    }
    throw "Searching for post beyond total posts size";
}
IPostTerm TermPostingListRaw::getPostAtByte(Offset numBytes, Offset &byteAfterPost) {
    uint8_t * curr = (uint8_t *)(buffer + postsOffset) + numBytes;
    size_t bytes = ((*curr) & 0xE0) >> 5;
    bytes += 1;
    byteAfterPost = numBytes + bytes;
    return IPostTerm(UtfToInt(curr));
}

Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t chunkSize) {
    size_t numLowBits = getNumLowBits(chunkSize, NUM_SYNC_POINTS);

    try {
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= NUM_SYNC_POINTS) {
            throw "seek out of range";
        }
        Location loc = raw->getPostLocationAt(syncPoint);

        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= NUM_SYNC_POINTS) {
                    return -1;
                    //throw "seek out of range";
                }
                loc = raw->getPostLocationAt(curr);
            }
            index = raw->getPostingsListOffsetAt(curr);
            return loc;
        } else {
            // Until next sync point or posts runs out
            for(Offset i = raw->getPostingsListOffsetAt(syncPoint);
                loc < (syncPoint + 1) << numLowBits && i < raw->getHeader()->numOfOccurence;
                ++i ) {
                Offset byteAfter = 0;
                if(i != raw->getPostingsListOffsetAt(syncPoint)) {
                    if(byteAfter == 0) {
                        loc += raw->getPostAt(i, byteAfter).delta;
                    } else {
                        loc += raw->getPostAtByte(byteAfter, byteAfter).delta;
                    }                }
                if(loc >= target) {
                    index = i;
                    return loc;
                }
            }
        }
        return -1;
        //throw "seek out of range";
    }
    catch(char *excp) {
        throw "unknown error";
    }
}

Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t chunkSize){
    size_t numLowBits = getNumLowBits(chunkSize, NUM_SYNC_POINTS);
    try{
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= NUM_SYNC_POINTS) {
            throw "seek out of range";
        }
        Location loc = raw->getPostLocationAt(syncPoint);
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= NUM_SYNC_POINTS) {
                    throw "seek out of range";
                }
                loc = raw->getPostLocationAt(curr);
                index = raw->getPostingsListOffsetAt(curr);
            }
            return loc;
        } else {
            // Until next sync point or posts runs out
            for(Offset i = raw->getPostingsListOffsetAt(syncPoint);
                loc < (syncPoint + 1) << numLowBits && i < raw->getHeader()->numOfOccurence;
                ++i ) {
                // Don't add delta from previous on initial sync point
                Offset byteAfter = 0;
                if(i != raw->getPostingsListOffsetAt(syncPoint)) {
                    //loc += raw->getPostAt(i).delta;
                    
                    if(byteAfter == 0) {
                        loc += raw->getPostAt(i, byteAfter).delta;
                    } else {
                        loc += raw->getPostAtByte(byteAfter, byteAfter).delta;
                    }
                    
                }
                if(loc >= target) {
                    index = i;
                    return loc;
                }
            }
        }
        throw "seek out of range";
    }
    catch(char *excp) {
        throw "unkown error";
    }
}

CommonHeaderBlob* EndDocPostingListRaw::getHeader() {
    return header;
}

Offset EndDocPostingListRaw::getPostingsListOffsetAt(Offset i) {
    return *(Offset *)(buffer + postingsListOffsetOffset + sizeof(size_t) * i);
}
Offset EndDocPostingListRaw::getPostLocationAt(Offset i) {
    return *(Offset *)(buffer + postLocationOffset + sizeof(size_t) * i);
}

IPostEndDoc EndDocPostingListRaw::getPostAt(Offset i) {
    uint8_t * curr = (uint8_t *)(buffer + postsOffset);
    for(size_t n = 0; n <= i; ++n) {
        size_t numBytes = ((*curr) & 0xE0) >> 5;
        numBytes += 1;
        if(n == i) {
            return IPostEndDoc(UtfToInt(curr));
        }
        curr = curr + numBytes;
    }
    throw "Searching for post beyond total posts size";}

// byteAfterPost will return the byte offset of 1 after the requested post
IPostEndDoc EndDocPostingListRaw::getPostAt(Offset i, Offset &byteAfterPost) {
    
    uint8_t * curr = (uint8_t *)(buffer + postsOffset);
    Offset byteCount = 0;
    for(size_t n = 0; n <= i; ++n) {
        size_t numBytes = ((*curr) & 0xE0) >> 5;
        numBytes += 1;
        byteCount = numBytes;
        if(n == i) {
            byteAfterPost = byteCount;
            return IPostEndDoc(UtfToInt(curr));
        }
        curr = curr + numBytes;
    }
    throw "Searching for post beyond total posts size";
     
}
IPostTerm EndDocPostingListRaw::getPostAtByte(Offset numBytes, Offset &byteAfterPost) {
    
    uint8_t * curr = (uint8_t *)(buffer + postsOffset) + numBytes;
    size_t bytes = ((*curr) & 0xE0) >> 5;
    bytes += 1;
    byteAfterPost = numBytes + bytes;
    return IPostTerm(UtfToInt(curr));
     
}

