#include "Post.h"
#include "PostingList.h"
#pragma once

typedef size_t Location;

class ISR {
public:
    //Get post of next term
    virtual Post* Next( ) = 0;
    // Get post of next endDoc
    virtual Post* NextEndDoc( ) = 0;
    // Get the first post after the target
    virtual Post* Seek( Location target) = 0;
    // Get position of first term
    virtual Location GetStartLocation( ) = 0;
    // Get position of last term
    virtual Location GetEndLocation( ) = 0;
};

class ISRWord: public ISR {
public:
    ISRWord(TermPostingList termPostingList) : currentPost(0),currentPostingsIndex(0),postingList(termPostingList) {
        // TODO: May need to change to pointer
    }
    // Returns next post
    Post* Next( );
    Post* NextEndDoc( );
    // Get the first post after the target
    Post* Seek( Location target);
    // Get position of first term
    Location GetStartLocation( );
    // Get position of last term
    Location GetEndLocation( );
    // get number of documents word is in
    unsigned GetDocumentCount( );
    // get number of occurances in index
    unsigned GetNumberOfOccurrences( );
    // get current post
    Post* GetCurrentPost( );
private:
    // TODO: Should this be a reference
    // postingList[]
    TermPostingList postingList;
    //index of Post 
    size_t currentPostingsIndex;
    Post currentPost;
    
};

class ISREndDoc: public ISR {
public: 
    ISREndDoc(EndDocPostingList endDocPostingList) : currentPost(0),currentPostingsIndex(0),postingList(endDocPostingList){
        // TODO: May need to change to pointer
    }
    Post* Next();
    Post* NextEndDoc( );
    // Get the first post after the target
    Post* Seek( Location target);
    // Get position of first term
    Location GetStartLocation( );
    // Get position of last term
    Location GetEndLocation( );
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