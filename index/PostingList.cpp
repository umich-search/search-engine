#include "PostingList.h"
#include "../utility/BinarySearch.h"


size_t seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    // TODO: Catch exceptions?
    size_t syncPoint = target >> numLowBits;
    if(syncPoint >= numSyncPoints) {
        return -1;
    }
    size_t loc = raw->getPostLocationAt(syncPoint);// postings->syncIndex.postLocation[syncPoint];

    // Until next sync point or posts runs out
    for(unsigned int i = raw->getPostingsListOffsetAt(syncPoint); loc < (syncPoint + 1) << numLowBits && i < raw->header->numOfOccurence; ++i ) {
        // Don't add delta from previous on initial sync point
        if(i != raw->getPostingsListOffsetAt(syncPoint)) {
            loc += raw->getPostAt(i).delta;
        }
        
        if(loc >= target) {
            index = i;
            return loc;
        }
    }
    return -1;
}
/*
size_t seekTermTarget(EndDocPostingList *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    // TODO: Catch exceptions?
    size_t syncPoint = target >> numLowBits;
    if(syncPoint >= numSyncPoints) {
        return -1;
    }
    size_t loc = raw->getPostLocationAt(syncPoint);// postings->syncIndex.postLocation[syncPoint];

    // Until next sync point or posts runs out
    for(unsigned int i = raw->getPostingsListOffsetAt(syncPoint); loc < (syncPoint + 1) << numLowBits && i < raw->header->numOfOccurence; ++i ) {
        // Don't add delta from previous on initial sync point
        if(i != raw->getPostingsListOffsetAt(syncPoint)) {
            loc += raw->getPostAt(i).delta;
        }
        
        if(loc >= target) {
            index = i;
            return loc;
        }
    }
    return -1;
}

*/
size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
    // TODO: Catch exceptions?
    size_t syncPoint = target >> numLowBits;
    if(syncPoint >= postings->syncIndex.postingsListOffset.size()) {
        return -1;
    }
    size_t loc = postings->syncIndex.postLocation[syncPoint];

    // Until next sync point or posts runs out
    for(unsigned int i = postings->syncIndex.postingsListOffset[syncPoint]; loc < (syncPoint + 1) << numLowBits && i < postings->posts.size(); ++i ) {
        // Don't add delta from previous on initial sync point
        if(i != postings->syncIndex.postingsListOffset[syncPoint]){
            loc += postings->posts[i].delta;
        }
        
        if(loc >= target) {
            index = i;
            return loc;
        }
    }
    return -1;
};

// TODO: As of now not changed form seekTermTarget
size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
    // TODO: Catch exceptions?
    size_t syncPoint = target >> numLowBits;
    if(syncPoint >= postings->syncIndex.postingsListOffset.size()) {
        return -1;
    }
    size_t loc = postings->syncIndex.postLocation[syncPoint];

    // Until next sync point or posts runs out
    for(unsigned int i = postings->syncIndex.postingsListOffset[syncPoint]; loc < (syncPoint + 1) << numLowBits && i < postings->posts.size(); ++i ) {
        // Don't add delta from previous on initial sync point
        if(i != postings->syncIndex.postingsListOffset[syncPoint]){
            loc += postings->posts[i].delta;
        }
        
        if(loc >= target) {
            index = i;
            return loc;
        }
    }
    return -1;
};


/*
size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
    size_t syncPoint = target >> numLowBits;
    size_t loc = postings->syncIndex.postLocation[syncPoint];
    for(unsigned int i = postings->syncIndex.postingsListOffset[syncPoint]; loc < (syncPoint + 1) << numLowBits; ++i) {
        if(loc >= target) {
            index = i;
            return loc;
        }
        else {
            loc += postings->posts[i].delta;
        }
    }
    return -1;
};
 */
/*

size_t seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits) {
    size_t syncPoint = target >> numLowBits;
    size_t loc = raw->getPostLocationAt(syncPoint);////postings->syncIndex.postLocation[syncPoint];
    for(unsigned int i = raw->getPostingsListOffsetAt(syncPoint); loc < (syncPoint + 1) << numLowBits; ++i) {
        if(loc >= target) {
            index = i;
            return loc;
        }
        else {
            loc += raw->getPostAt(i).delta;////postings->posts[i].delta;
        }
    }
    return -1;
}
*/

void createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits) {
    //numLowBits -= 1;
    SyncIndex* index = &postings->syncIndex;
    size_t runningLocation = startLoc;
    unsigned int currentSyncPoint = 0b0;
    unsigned int curr = 0b0;
    for(unsigned int i = 0; i < postings->posts.size(); ++i) {
        runningLocation += postings->posts[i].delta;
        currentSyncPoint = runningLocation >> numLowBits;
        if(curr != currentSyncPoint ) {
            curr = currentSyncPoint;
        }
        int t = index->postLocation[currentSyncPoint];
        
        if(index->postLocation[currentSyncPoint] == -1) {
            index->postLocation[currentSyncPoint] = runningLocation;
            index->postingsListOffset[currentSyncPoint] = i;
        }
    }
}

void createSeekIndex(EndDocPostingList *postings, size_t startLoc, size_t numLowBits) {
    //numLowBits -= 1;
    SyncIndex* index = &postings->syncIndex;
    size_t runningLocation = startLoc;
    unsigned int currentSyncPoint = 0b0;
    unsigned int curr = 0b0;
    for(unsigned int i = 0; i < postings->posts.size(); ++i) {
        runningLocation += postings->posts[i].delta;
        currentSyncPoint = runningLocation >> numLowBits;
        if(curr != currentSyncPoint ) {
            curr = currentSyncPoint;
        }
        int t = index->postLocation[currentSyncPoint];
        
        if(index->postLocation[currentSyncPoint] == -1) {
            index->postLocation[currentSyncPoint] = runningLocation;
            index->postingsListOffset[currentSyncPoint] = i;
        }
    }
}
