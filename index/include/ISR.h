#pragma once

#include "Post.h"
#include "PostingListBlob.h"
#include "FileManager.h"
#include "../constraint_solver/constraint_solver.h"

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

    // Get number of Terms of and ISR
    virtual int GetTermNum() = 0;

    // Calculate according to heuristics
    virtual int GetHeuristicScore( Match *document ) = 0;

protected:
    int num_short_spans = 0;
    int num_inorder_spans = 0;
    int num_exact_phrase = 0;
    int num_spans_near_top = 0;
    bool all_words_freq = false;
    bool most_words_freq = false;
    bool some_words_freq = false;
    const int freq_threshold = 3;

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

    int GetTermNum(){ return 0; }

    int GetHeuristicScore( Match *document );

private:
    FileManager manager;
    const char *term;
    size_t currChunk;
    Offset currIndex;
    Post currPost;
    Post Doc;
    TermPostingListRaw termPostingListRaw;
    const int short_span_weight = 1;
    const int span_near_top_weight = 0.5;
    const int some_freq_weight = 5;
    const int near_top_threshold = 50;
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

    int GetTermNum(){ return 0; }

    int GetHeuristicScore(){return 0;}

private:
    FileManager manager;
    size_t currChunk;
    Offset currIndex;
    Post currPost;
    EndDocPostingListRaw endDocPostingListRaw;
};
