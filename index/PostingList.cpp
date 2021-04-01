#include "PostingList.h"
#include "../utility/BinarySearch.h"



size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
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

void createSeekIndex(TermPostingList *postings, size_t numLowBits) {
    SyncIndex* index = &postings->syncIndex;
    size_t runningLocation = 0;
    unsigned int currentSyncPoint = 0;

    for(unsigned int i = 0; i < postings->posts.size(); ++i) {
        runningLocation += postings->posts[i].delta;
        currentSyncPoint = runningLocation >> numLowBits;
        if(index->postLocation[currentSyncPoint] == -1) {
            index->postLocation[currentSyncPoint] = runningLocation;
            index->postingsListOffset[currentSyncPoint] = i;
        }
    }
}

void createSeekIndex(EndDocPostingList *postings, size_t numLowBits) {
    SyncIndex* index = &postings->syncIndex;
    size_t runningLocation = 0;
    unsigned int currentSyncPoint = 0;

    for(unsigned int i = 0; i < postings->posts.size(); ++i) {
        runningLocation += postings->posts[i].delta;
        currentSyncPoint = runningLocation >> numLowBits;
        if(index->postLocation[currentSyncPoint] == -1) {
            index->postLocation[currentSyncPoint] = runningLocation;
            index->postingsListOffset[currentSyncPoint] = i;
        }
    }
}
