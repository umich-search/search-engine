#pragma once

#include "Global.h"
#include "Post.h"
#include "PostingList.h"
#include "FileManager.h"

typedef size_t Location;

class ISR {
public:
    //Get post of next term
    virtual Post *Next() = 0;

    // Get post of next endDoc
    virtual Post *NextEndDoc() = 0;

    // Get the first post after the target
    virtual Post *Seek(Location target) = 0;

    // Get position of first term
    virtual Location GetStartLocation() = 0;

    // Get position of last term
    virtual Location GetEndLocation() = 0;
};

//CommonHeader commonHeader;
//commonHeader.term = word;
//commonHeader.numOfDocument = manager.getNumDocuments();
//commonHeader.numOfOccurence = manager.getIndexWords();
//
//size_t syncSize = NUM_SYNC_POINTS;
//TermPostingList termPostingList(syncSize);
//termPostingList.header = commonHeader;
//termPostingList.posts = posts;
class ISRWord : public ISR {
public:
    ISRWord(FileManager fileManager) : manager(fileManager),currLoc(0) {
    }

    // Returns next post
    Post *Next();

    Post *NextEndDoc();

    // Get the first post after the target
    Post *Seek(size_t target);

    // Get position of first term
    Location GetStartLocation();

    // Get position of last term
    Location GetEndLocation();

    // get number of documents word is in
    d_Occurence GetDocumentCount();

    // get number of occurances in index
    w_Occurence GetNumberOfOccurrences();

    // get current post
    Post *GetCurrentPost();

private:
    FileManager manager;
    const char* term;
    size_t currChunk;
    Offset currBytes;
    Post currPost;
    TermPostingListRaw termPostingListRaw;


};

class ISREndDoc : public ISR {
public:
    ISREndDoc(EndDocPostingList endDocPostingList) : currentPost(0), currentPostingsIndex(0),
                                                     postingList(endDocPostingList) {
    }

    Post *Next();

    Post *NextEndDoc();

    // Get the first post after the target
    Post *Seek(Location target);

    // Get position of first term
    Location GetStartLocation();

    // Get position of last term
    Location GetEndLocation();

    // get length of document
    unsigned GetDocumentLength();

    // get length of document title
    unsigned GetTitleLength();

    // get length of url
    unsigned GetUrlLength();

private:
    EndDocPostingList postingList;
    size_t currentPostingsIndex;
    Post currentPost;
};
