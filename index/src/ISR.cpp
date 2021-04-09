#include "ISR.h"

// ISRWord Functions

Post *ISRWord::Next() {
    size_t numOccurence = termPostingListRaw.getHeader()->numOfOccurence;
    if (currIndex < numOccurence - 1) {
        currIndex += 1;
        Location delta = termPostingListRaw.getPostAt(currIndex).delta;
        currPost.SetLocation(delta + currPost.GetStartLocation());
    } else {
        Post *post = Seek(currPost.GetStartLocation() + 1);
        if (post == nullptr) return nullptr;
        else currPost = *post;
    }
    return &currPost;
}

Post *ISRWord::NextEndDoc() {
    EndDocPostingListRaw endDoc = manager.GetEndDocList(currChunk);
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t target = currPost.GetStartLocation();
    size_t chunkSize;
    size_t temp;
    size_t offset;
    if (currChunk > 0) {
        chunkSize = endLocs[currChunk] - endLocs[currChunk - 1];
        offset = endLocs[currChunk - 1];
    } else {
        chunkSize = endLocs[currChunk];
        offset = 0;
    }
    Location endLoc = seekEndDocTarget(&endDoc, target - offset, temp, chunkSize);
    if (currChunk > 0) endLoc += endLocs[currChunk - 1];
    Doc.SetLocation(endLoc);
    return &Doc;
}

Post *ISRWord::Seek(size_t target) {
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t numChunks = endLocs.size();
    size_t chunkIndex;
    Location result = -1;
    for (chunkIndex = 0; chunkIndex < numChunks; chunkIndex++) {
        if (endLocs[chunkIndex] >= target) break;
    }
    if (chunkIndex >= numChunks) return nullptr;
    for (size_t chunk = chunkIndex; chunk < numChunks; chunk++) {
        try {
            TermPostingListRaw termraw = manager.GetTermList(term, chunk);
            if (termraw.getHeader()->numOfOccurence == 0) continue;
            else {
                size_t temp;
                Offset chunkSize;
                size_t offset;
                size_t searchTarget;
                if (chunk > 0) {
                    chunkSize = endLocs[chunk] - endLocs[chunk - 1];
                    offset = endLocs[chunk - 1];
                } else {
                    chunkSize = endLocs[chunk];
                    offset = 0;
                }
                if (target < offset) searchTarget = 0;
                else searchTarget = target - offset;
                result = seekTermTarget(&termraw, searchTarget, temp, chunkSize);
                if (result == -1) continue;
                if (chunk != 0)result += endLocs[chunk - 1];
                currChunk = chunk;
                termPostingListRaw = termraw;
                currIndex = temp;
                break;
            }
        }
        catch (const char *excep) {
            continue;
        }
    }
    if (result == -1) return nullptr;
    currPost.SetLocation(result);
    return &currPost;

}

Location ISRWord::GetStartLocation() {
    //fetch termpostinglistraw for the first chunk
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t numChunks = endLocs.size();
    Location result = -1;
    for (size_t chunk = 0; chunk < numChunks; chunk++) {
        try {
            TermPostingListRaw termraw = manager.GetTermList(term, chunk);
            if (termraw.getHeader()->numOfOccurence == 0) continue;
            else {
                size_t temp;
                Offset chunkSize;
                if (chunk > 0) {
                    chunkSize = endLocs[chunk] - endLocs[chunk - 1];
                } else {
                    chunkSize = endLocs[chunk];
                }
                result = seekTermTarget(&termraw, 0, temp, chunkSize);
                if (chunk != 0)result += endLocs[chunk - 1];
                return result;
            }
        }
        catch (std::string &) {
            continue;
        }
    }
    return result;
}

Location ISRWord::GetEndLocation() {
    size_t numChunks = manager.getChunkEndLocations().size();
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t result = -1;
    for (size_t chunk = numChunks - 1; chunk >= 0; chunk--) {
        try {
            TermPostingListRaw termraw = manager.GetTermList(term, chunk);
            if (termraw.getHeader()->numOfOccurence == 0) continue;
            else {
                size_t numOccurence = termraw.getHeader()->numOfOccurence;
                size_t temp;
                size_t chunkSize;
                if (chunk > 0) {
                    chunkSize = endLocs[chunk] - endLocs[chunk - 1];
                } else {
                    chunkSize = endLocs[chunk];
                }
                result = seekTermTarget(&termraw, 0, temp, chunkSize);
                for (int i = 1; i < numOccurence; i++) {
                    result += termraw.getPostAt(i).delta;
                }
                if (chunk != 0)result += endLocs[chunk - 1];
                return result;
            }
        }
        catch (std::string &) {
            continue;
        }
    }
    return result;
}

d_Occurence ISRWord::GetDocumentCount() {
    return manager.getNumDocuments();
}

w_Occurence ISRWord::GetNumberOfOccurrences() {
    size_t numChunks = manager.getChunkEndLocations().size();
    w_Occurence total = 0;
    for (int i = 0; i < numChunks; i++)total += manager.GetTermList(term, i).getHeader()->numOfOccurence;
    return total;
}

Post *ISRWord::GetCurrentPost() {
    return &currPost;
}


// ISREndDoc Functions
Post *ISREndDoc::Next() {
    size_t numChunks = manager.getChunkEndLocations().size();
    //todo: should deal with the case when curr term is the last in the chunk
    if (currPost.GetStartLocation() < manager.getChunkEndLocations()[currChunk]) {
        Location delta = endDocPostingListRaw.getPostAtByte(currBytes, currBytes).delta;
        currPost.SetLocation(delta + currPost.GetStartLocation());
    } else {
        currChunk += 1;
        if (currChunk > numChunks - 1) return nullptr;
        try {
            endDocPostingListRaw = manager.GetEndDocList(currChunk);
        }
        catch (std::string &) {
            return nullptr;
        }
        Location delta = endDocPostingListRaw.getPostAtByte(0, currBytes).delta;
        currPost.SetLocation(delta + manager.getChunkEndLocations()[currChunk - 1]);
    }
    return &currPost;
}

Post *ISREndDoc::NextEndDoc() {
    return nullptr;
}

Post *ISREndDoc::Seek(Location target) {
    vector<Location> chunkEndLocations = manager.getChunkEndLocations();
    size_t numChunks = chunkEndLocations.size();
    size_t chunkIndex;
    for (chunkIndex = 0; chunkIndex < numChunks; chunkIndex++) {
        if (chunkEndLocations[chunkIndex] >= target) break;
    }
    if (chunkIndex >= numChunks) return nullptr;
    currChunk = chunkIndex;
    try {
        endDocPostingListRaw = manager.GetEndDocList(currChunk);
    }
    catch (std::string &) {
        return nullptr;
    }
    size_t index;
    Location searchResult = seekEndDocTarget(&endDocPostingListRaw, target - chunkEndLocations[currChunk - 1], index,
                                             CHUNK_SIZE_BYTES);

    //todo: set currBytes to be bytes after Post
    if (searchResult != -1) {
        currPost.SetLocation(searchResult);
        return &currPost;
    } else return nullptr;
}

Location ISREndDoc::GetStartLocation() {
    //return seekEndDocTarget(&postingList, 0, throwaway);
    //fetch termpostinglistraw for the first chunk
    currChunk = 0;
    try {
        endDocPostingListRaw = manager.GetEndDocList(currChunk);
    }
    catch (std::string &) {
        return -1;
    }
    Location delta = endDocPostingListRaw.getPostAtByte(0, currBytes).delta;
    currPost.SetLocation(delta);
    currChunk = 0;
    return delta;
}

Location ISREndDoc::GetEndLocation() {
    size_t numChunks = manager.getChunkEndLocations().size();
    return manager.getChunkEndLocations()[numChunks - 1];
}

unsigned ISREndDoc::GetDocumentLength() {
    return DOCUMENT_SIZE;
}

unsigned ISREndDoc::GetTitleLength() {
    return MAX_TITLE_LENGTH;
}

unsigned ISREndDoc::GetUrlLength() {
    return MAX_URL_LENGTH;
}
