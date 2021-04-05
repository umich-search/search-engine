#include "PostingList.h"
#include "../utility/BinarySearch.h"

Location seekTermTarget(TermPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    try {
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        Location loc = raw->getPostLocationAt(syncPoint);
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= numSyncPoints) {
                    return -1;
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
        return -1;
    }
    catch(char *excp) {
        return -1;
    }
}


// TODO: size_t can't return -1 dummy
size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
    try {
        Location syncPoint = target >> numLowBits;
        if(syncPoint >= postings->syncIndex.postingsListOffset.size()) {
            return -1;
        }
        Location curr = syncPoint;
        Location loc = postings->syncIndex.postLocation[curr];
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= postings->syncIndex.postingsListOffset.size()) {
                    return -1;
                }
                loc = postings->syncIndex.postLocation[curr];
            }
            index = postings->syncIndex.postingsListOffset[curr];
            return loc;
        } else {
            // Until next sync point or posts runs out
            for(Offset i = postings->syncIndex.postingsListOffset[syncPoint];
                loc < (syncPoint + 1) << numLowBits && i < postings->posts.size();
                ++i ) {
                // Don't add delta from previous on initial sync point
                if(i != postings->syncIndex.postingsListOffset[syncPoint]){
                    loc += postings->posts[i].delta;
                }
                if(loc >= target) {
                    index = i;
                    return loc;
                }
            }
        }
        return -1;
    }
    catch(char *excp) {
        return -1;
    }
};


size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index, size_t numLowBits) {
    try {
        Location syncPoint = target >> numLowBits;
        if(syncPoint >= postings->syncIndex.postingsListOffset.size()) {
            return -1;
        }
        Location curr = syncPoint;
        Location loc = postings->syncIndex.postLocation[curr];
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= postings->syncIndex.postingsListOffset.size()) {
                    return -1;
                }
                loc = postings->syncIndex.postLocation[curr];
                index = postings->syncIndex.postingsListOffset[curr];
            }
            return loc;
        } else {
            // Until next sync point or posts runs out
            for(Offset i = postings->syncIndex.postingsListOffset[syncPoint];
                loc < (syncPoint + 1) << numLowBits && i < postings->posts.size();
                ++i ) {
                // Don't add delta from previous on initial sync point
                if(i != postings->syncIndex.postingsListOffset[syncPoint]){
                    loc += postings->posts[i].delta;
                }
                if(loc >= target) {
                    index = i;
                    return loc;
                }
            }
        }
        return -1;
    }
    catch(char *excp) {
        return -1;
    }
};

Location seekEndDocTarget(EndDocPostingListRaw *raw, size_t target, size_t &index, size_t numLowBits, size_t numSyncPoints) {
    try{
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        Location loc = raw->getPostLocationAt(syncPoint);
        if(loc == -1) {
            while(loc == -1) {
                if(++curr >= numSyncPoints) {
                    return -1;
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
        return -1;
    }
    catch(char *excp) {
        return -1;
    }
}

int createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits) {
    try {
        SyncIndex* index = &postings->syncIndex;
        Location runningLocation = startLoc;
        Location currentSyncPoint = 0;
        for(unsigned int i = 0; i < postings->posts.size(); ++i) {
            runningLocation += postings->posts[i].delta;
            currentSyncPoint = runningLocation >> numLowBits;
            if(index->postLocation[currentSyncPoint] == -1) {
                index->postLocation[currentSyncPoint] = runningLocation;
                index->postingsListOffset[currentSyncPoint] = i;
            }
        }
        return 0;
    }
    catch(char *excp) {
        return -1;
    }
}

int createSeekIndex(EndDocPostingList *postings, size_t startLoc, size_t numLowBits) {
    try {
        SyncIndex* index = &postings->syncIndex;
        Location runningLocation = startLoc;
        Location currentSyncPoint = 0;
        postings->header.numOfOccurence = postings->posts.size();
        for(unsigned int i = 0; i < postings->posts.size(); ++i) {
            runningLocation += postings->posts[i].delta;
            currentSyncPoint = runningLocation >> numLowBits;
            if(index->postLocation[currentSyncPoint] == -1) {
                index->postLocation[currentSyncPoint] = runningLocation;
                index->postingsListOffset[currentSyncPoint] = i;
            }
        }
        return 0;
    }
    catch(char *excp) {
        return -1;
    }
}
