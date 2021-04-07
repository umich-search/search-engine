#include "ISR.h"
#include "PostingList.h"

// ISRWord Functions

Post *ISRWord::Next() {
    //Todo: come to next chunk if endpoint reach
    Location delta = termPostingListRaw.getPostAtByte(currBytes, currBytes).delta;
    currPost.SetLocation(delta + currPost.GetLocation());
    return &currPost;
}

Post *ISRWord::NextEndDoc() {
    return nullptr;
}

Post *ISRWord::Seek(size_t target) {
    size_t index = 0;
    ::vector<Location> chunkOffsets = manager.getChunkStartLocations();
    for (int i = 0; i < chunkOffsets.size(); i++) {
        termPostingListRaw = manager.GetTermList(term, chunkOffsets[i]);
        Location loc = seekTermTarget(&termPostingListRaw, target, index, NUM_LOW_BITS, NUM_SYNC_POINTS);
        if (loc >= target) {
            currPost.SetLocation(loc);
            currChunk = i;
            break;
        }
    }
    return &currPost;
}

Location ISRWord::GetStartLocation() {
    Location chunkOffset = manager.getChunkEndLocations()[0];
    termPostingListRaw = manager.GetTermList(term, chunkOffset);
    Location delta = termPostingListRaw.getPostAtByte(0, currBytes).delta;
    currPost.SetLocation(delta);
    currChunk = 0 ;
    return delta;
}

Location ISRWord::GetEndLocation() {
    return -1;
}

d_Occurence ISRWord::GetDocumentCount() {
    return manager.getNumDocuments();
}

w_Occurence ISRWord::GetNumberOfOccurrences() {
    return manager.getIndexWords();
}

Post *ISRWord::GetCurrentPost() {
    return &currPost;
}


// ISREndDoc Functions
Post *ISREndDoc::Next() {
    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex + 1].delta;
    currentPostingsIndex++;
    currentPost = Post(indexLocation);
    return &currentPost;
}

Post *ISREndDoc::NextEndDoc() {
    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex + 1].delta;
    currentPostingsIndex++;
    currentPost = Post(indexLocation);
    return &currentPost;
}

Post *ISREndDoc::Seek(Location target) {
    //size_t indexLocation = seekEndDocTarget(&postingList, target, currentPostingsIndex);
    // term after target not found;
    /*
    if(indexLocation == -1) {
        return nullptr;
    }
    */
    currentPost = Post(2);//Post(indexLocation);
    return &currentPost;
}

Location ISREndDoc::GetStartLocation() {
    //return seekEndDocTarget(&postingList, 0, throwaway);
    return -1;
}

Location ISREndDoc::GetEndLocation() {
    return -1;
}

unsigned ISREndDoc::GetDocumentLength() {
    /*
   return postingList.posts[currentPostingsIndex].lengthOfDocument;
     */
    return -1;
}

unsigned ISREndDoc::GetTitleLength() {
    /*
    char c = postingList.posts[currentPostingsIndex].title[0];
    unsigned int count = 0;
    while(c != '\0') {
        count++;
    }
   
    return count;
     */
    return -1;

}

unsigned ISREndDoc::GetUrlLength() {
    /*
    char c = postingList.posts[currentPostingsIndex].URL[0];
    unsigned int count = 0;
    while(c != '\0') {
        count++;
    }
    return count;
     */
    return -1;
}

