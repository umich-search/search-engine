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
    size_t numChunks = manager.getChunkEndLocations().size();
    w_Occurence total = 0;
    for (int i = 0; i < numChunks; i++)total += manager.GetTermList(term, i).getHeader()->numOfDocument;
    return total;
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
Post *ISREndDoc::GetCurrentPost(){
    return &currPost;
}

Post *ISREndDoc::Next() {
    size_t numDoc = endDocPostingListRaw.getHeader()->numOfDocument;
    if (currIndex < numDoc - 1) {
        currIndex += 1;
        Location delta = endDocPostingListRaw.getPostAt(currIndex).delta;
        currPost.SetLocation(delta + currPost.GetStartLocation());
    } else {
        Post *post = Seek(currPost.GetStartLocation() + 1);
        if (post == nullptr) return nullptr;
        else currPost = *post;
    }
    return &currPost;
}

Post *ISREndDoc::NextEndDoc(){
    return Next();
}

Post *ISREndDoc::Seek(Location target) {
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
            EndDocPostingListRaw docraw = manager.GetEndDocList(chunk);
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
            result = seekEndDocTarget(&docraw, searchTarget, temp, chunkSize);
            if (result == -1) continue;
            if (chunk != 0)result += endLocs[chunk - 1];
            currChunk = chunk;
            endDocPostingListRaw = docraw;
            currIndex = temp;
            break;
        }
        catch (const char *excep) {
            continue;
        }
    }
    if (result == -1) return nullptr;
    currPost.SetLocation(result);
    return &currPost;
}

Location ISREndDoc::GetStartLocation() {
    //fetch termpostinglistraw for the first chunk
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t numChunks = endLocs.size();
    Location result = -1;
    for (size_t chunk = 0; chunk < numChunks; chunk++) {
        try {
            EndDocPostingListRaw docRaw = manager.GetEndDocList(chunk);
            size_t temp;
            Offset chunkSize;
            if (chunk > 0) {
                chunkSize = endLocs[chunk] - endLocs[chunk - 1];
            } else {
                chunkSize = endLocs[chunk];
            }
            result = seekEndDocTarget(&docRaw, 0, temp, chunkSize);
            if (chunk != 0)result += endLocs[chunk - 1];
            return result;
        }
        catch (std::string &) {
            continue;
        }
    }
    return result;
}

Location ISREndDoc::GetEndLocation() {
    size_t numChunks = manager.getChunkEndLocations().size();
    vector<Location> endLocs = manager.getChunkEndLocations();
    size_t result = -1;
    for (size_t chunk = numChunks - 1; chunk >= 0; chunk--) {
        try {
            EndDocPostingListRaw docraw = manager.GetEndDocList(chunk);
            size_t temp;
            size_t chunkSize;
            size_t numOccurence = docraw.header->numOfDocument;
            if (chunk > 0) {
                chunkSize = endLocs[chunk] - endLocs[chunk - 1];
            } else {
                chunkSize = endLocs[chunk];
            }
            result = seekEndDocTarget(&docraw, 0, temp, chunkSize);
            for (int i = 1; i < numOccurence; i++) {
                result += docraw.getPostAt(i).delta;
            }
            if (chunk != 0)result += endLocs[chunk - 1];
            return result;
        }
        catch (std::string &) {
            continue;
        }
    }
    return result;
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
