#include "ISR.h"

// ISRWord Functions

Post *ISRWord::Next() {
    size_t numChunks = manager.getChunkEndLocations().size();
    //todo: should deal with the case when curr term is the last in the chunk
    if (currPost.GetLocation() < manager.getChunkEndLocations()[currChunk]) {
        Location delta = termPostingListRaw.getPostAtByte(currBytes, currBytes).delta;
        currPost.SetLocation(delta + currPost.GetLocation());
    } else {
        currChunk += 1;
        if (currChunk > numChunks - 1) return nullptr;
        try {
            termPostingListRaw = manager.GetTermList(term, currChunk);
        }
        catch (std::string &) {
            return nullptr;
        }
        Location delta = termPostingListRaw.getPostAtByte(0, currBytes).delta;
        currPost.SetLocation(delta + manager.getChunkEndLocations()[currChunk - 1]);
    }
    return &currPost;
}

Post *ISRWord::NextEndDoc() {
    return nullptr;
}

Post *ISRWord::Seek(size_t target) {
    ::vector<Location> chunkEndLocations = manager.getChunkEndLocations();
    size_t numChunks = chunkEndLocations.size();
    size_t chunkIndex;
    for (chunkIndex = 0; chunkIndex < numChunks; chunkIndex++) {
        if (chunkEndLocations[chunkIndex] >= target) break;
    }
    if (chunkIndex >= numChunks) return nullptr;
    currChunk = chunkIndex;
    try {
        termPostingListRaw = manager.GetTermList(term, currChunk);
    }
    catch (std::string &) {
        return nullptr;
    }
    size_t index;
    Location searchResult = seekTermTarget(&termPostingListRaw, target - chunkEndLocations[currChunk - 1], index,
                                           NUM_LOW_BITS, NUM_SYNC_POINTS);
    //todo: set currBytes to be bytes after Post
    if (searchResult != -1) {
        currPost.SetLocation(searchResult);
        return &currPost;
    }
    else return nullptr;
}

Location ISRWord::GetStartLocation() {
    //fetch termpostinglistraw for the first chunk
    currChunk = 0;
    try {
        termPostingListRaw = manager.GetTermList(term, currChunk);
    }
    catch (std::string &) {
        return -1;
    }
    Location delta = termPostingListRaw.getPostAtByte(0, currBytes).delta;
    currPost.SetLocation(delta);
    currChunk = 0;
    return delta;
}

Location ISRWord::GetEndLocation() {
    //todo: modify it to return the end location of this word, not whole chunk, getPostAtByte may need to return nullptr when out of range
    size_t numChunks = manager.getChunkEndLocations().size();
    return manager.getChunkEndLocations()[numChunks - 1];
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


//// ISREndDoc Functions
//Post *ISREndDoc::Next() {
//    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex + 1].delta;
//    currentPostingsIndex++;
//    currentPost = Post(indexLocation);
//    return &currentPost;
//}
//
//Post *ISREndDoc::NextEndDoc() {
//    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex + 1].delta;
//    currentPostingsIndex++;
//    currentPost = Post(indexLocation);
//    return &currentPost;
//}
//
//Post *ISREndDoc::Seek(Location target) {
//    //size_t indexLocation = seekEndDocTarget(&postingList, target, currentPostingsIndex);
//    // term after target not found;
//    /*
//    if(indexLocation == -1) {
//        return nullptr;
//    }
//    */
//    currentPost = Post(2);//Post(indexLocation);
//    return &currentPost;
//}
//
//Location ISREndDoc::GetStartLocation() {
//    //return seekEndDocTarget(&postingList, 0, throwaway);
//    return -1;
//}
//
//Location ISREndDoc::GetEndLocation() {
//    return -1;
//}
//
//unsigned ISREndDoc::GetDocumentLength() {
//    /*
//   return postingList.posts[currentPostingsIndex].lengthOfDocument;
//     */
//    return -1;
//}
//
//unsigned ISREndDoc::GetTitleLength() {
//    /*
//    char c = postingList.posts[currentPostingsIndex].title[0];
//    unsigned int count = 0;
//    while(c != '\0') {
//        count++;
//    }
//
//    return count;
//     */
//    return -1;
//
//}
//
//unsigned ISREndDoc::GetUrlLength() {
//    /*
//    char c = postingList.posts[currentPostingsIndex].URL[0];
//    unsigned int count = 0;
//    while(c != '\0') {
//        count++;
//    }
//    return count;
//     */
//    return -1;
//}
//
