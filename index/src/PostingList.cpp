#include "PostingList.h"

size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index, size_t chunkSize) 
    {
    size_t numLowBits = getNumLowBits(chunkSize, NUM_SYNC_POINTS);
    try 
        {
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= postings->syncIndex.postLocation.size()) 
            {
            throw "seek out of range";
            }   
        Location loc = postings->syncIndex.postLocation[curr];
        if(loc == -1) 
            {
            while(loc == -1) 
                {
                if(++curr >= postings->syncIndex.postingsListOffset.size()) 
                    throw "seek out of range";
                loc = postings->syncIndex.postLocation[curr];
                }
            if(loc != -1 ) 
                {
                index = postings->syncIndex.postingsListOffset[curr];
                }
            return loc;
            } else 
            {
            // Until next sync point or posts runs out
            for(Offset i = postings->syncIndex.postingsListOffset[syncPoint];
                loc < (syncPoint + 1) << numLowBits && i < postings->posts.size();
                ++i ) 
                {
                // Don't add delta from previous on initial sync point
                if(i != postings->syncIndex.postingsListOffset[syncPoint])
                    {
                    loc += postings->posts[i].delta;
                    }
                if(loc >= target) 
                    {
                    index = i;
                    return loc;
                    }
                }
            }
        throw "seek out of range";
    }
    catch(char *excp) 
        {
        throw "unknown error";
        }
    };


size_t seekEndDocTarget(SharedPointer<EndDocPostingList> postings, size_t target, size_t &index, size_t chunkSize) 
    {
    size_t numLowBits = getNumLowBits(chunkSize, NUM_SYNC_POINTS);
    try 
        {
        Location syncPoint = target >> numLowBits;
        Location curr = syncPoint;
        if(curr >= postings->syncIndex.postLocation.size()) 
            {
            throw "seek out of range";
            }
        Location loc = postings->syncIndex.postLocation[curr];
        if(loc == -1) 
            {
            while(loc == -1) 
                {
                if(++curr >= postings->syncIndex.postingsListOffset.size()) 
                    {
                    throw "seek out of range";
                    }
                loc = postings->syncIndex.postLocation[curr];
                }
            if(loc != -1) 
                {
                index = postings->syncIndex.postingsListOffset[curr];
                }   
            return loc;
            } else 
            {
            // Until next sync point or posts runs out
            for(Offset i = postings->syncIndex.postingsListOffset[syncPoint];
                loc < (syncPoint + 1) << numLowBits && i < postings->posts.size();
                ++i ) {
                // Don't add delta from previous on initial sync point
                if(i != postings->syncIndex.postingsListOffset[syncPoint])
                    {
                    loc += postings->posts[i].delta;
                    }
                if(loc >= target) 
                    {
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
};

int createSeekIndex(TermPostingList *postings, size_t startLoc, size_t numLowBits) 
    {
    try 
        {
        SyncIndex* index = &postings->syncIndex;
        Location runningLocation = startLoc;
        Location currentSyncPoint = 0;
        for(unsigned int i = 0; i < postings->posts.size(); ++i) 
            {
            runningLocation += postings->posts[i].delta;
            currentSyncPoint = runningLocation >> numLowBits;
            if(index->postLocation[currentSyncPoint] == -1) 
                {
                index->postLocation[currentSyncPoint] = runningLocation;
                index->postingsListOffset[currentSyncPoint] = i;
                }
            }
        return 0;
        }
    catch(char *excp) 
        {
        return -1;
        }
    }

int createSeekIndex(SharedPointer<EndDocPostingList> postings, size_t startLoc, size_t numLowBits) 
    {
    try 
        {
        SyncIndex* index = &postings->syncIndex;
        Location runningLocation = startLoc;
        Location currentSyncPoint = 0;
        postings->header.numOfOccurence = postings->posts.size();
        for(unsigned int i = 0; i < postings->posts.size(); ++i) 
            {
            runningLocation += postings->posts[i].delta;
            currentSyncPoint = runningLocation >> numLowBits;
            if(index->postLocation[currentSyncPoint] == -1) 
                {
                index->postLocation[currentSyncPoint] = runningLocation;
                index->postingsListOffset[currentSyncPoint] = i;
                }
            }
        return 0;
        }
    catch(char *excp) 
        {
        return -1;
        }
    }
