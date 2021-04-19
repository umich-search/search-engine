#pragma once

#include "Post.h"
#include "PostingListBlob.h"
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

    // Get Terms data of an ISR
    virtual ISR **GetTerms() = 0;
};

class ISRWord : public ISR {
public:
    ISRWord(FileManager fileManager, const char *word) : manager(fileManager), currChunk(0), currIndex(0), term(word),
                                                         currPost(0),Doc(0) {
            // Initalize with term seek to 0
            Seek(0);
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

    // Get Terms data of an ISR
    ISR **GetTerms();

private:
    FileManager manager;
    const char *term;
    size_t currChunk;
    Offset currIndex;
    Post currPost;
    Post Doc;
    TermPostingListRaw termPostingListRaw;
};

class ISREndDoc : public ISR {
public:
    ISREndDoc(FileManager filemanager) : manager(filemanager), currChunk(0),currIndex(0),
                                         endDocPostingListRaw(manager.GetEndDocList(0)),
                                         currPost(0){
            Seek(0);
    }
    Post *GetCurrentPost();

    Post *NextEndDoc();

    Post *Next();

    // Get the first post after the target
    Post *Seek(Location target);

    // Get position of first term
    Location GetStartLocation();

    // Get position of last term
    Location GetEndLocation();

    // Get doc index 
    Offset GetCurrIndex();

    // get length of document
    unsigned GetDocumentLength();

    // get length of document title
    unsigned GetTitleLength();

    // get length of url
    unsigned GetUrlLength();

    // Get Terms data of an ISR
    ISR **GetTerms();

private:
    FileManager manager;
    size_t currChunk;
    Offset currIndex;
    Post currPost;
    EndDocPostingListRaw endDocPostingListRaw;
};
