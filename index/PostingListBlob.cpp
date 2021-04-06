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
    return IPostTerm(*(Offset *)(buffer + postsOffset + sizeof(size_t) * i));
}

Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    try {
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= numSyncPoints) {
            throw "seek out of range";
        }
        Location loc = raw->getPostLocationAt(syncPoint);

        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= numSyncPoints) {
                    throw "seek out of range";
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
                if(i != raw->getPostingsListOffsetAt(syncPoint)) {
                    loc += raw->getPostAt(i).delta;
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
        throw "unknown error";
    }
}

Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    try{
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= numSyncPoints) {
            throw "seek out of range";
        }
        Location loc = raw->getPostLocationAt(syncPoint);
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= numSyncPoints) {
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
                if(i != raw->getPostingsListOffsetAt(syncPoint)) {
                    loc += raw->getPostAt(i).delta;
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

IPostTerm EndDocPostingListRaw::getPostAt(Offset i) {
    return IPostTerm(*(Offset *)(buffer + postsOffset + sizeof(size_t) * i));
}
