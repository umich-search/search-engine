typedef size_t Location;

#include "Post.h"
#include "PostingList.h"

class ISR {
public:
    //Get post of next term
    virtual Post* Next( );
    // Get post of next endDoc
    virtual Post* NextEndDoc( );
    // Get the first post after the target
    virtual Post* Seek( Location target);
    // Get position of first term
    virtual Location GetStartLocation( );
    // Get position of last term
    virtual Location GetEndLocation( );
    
};

class ISRWord: public ISR {
public:
    ISRWord(TermPostingList termPostingList) {
        postingList = termPostingList;
        currentPostingsIndex = 0;
        // TODO: May need to change to pointer
        currentPost = Post(0);
    }
    // Returns next post
    Post* Next ();
    Post* NextEndDoc();
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

class ISREndDoc: public ISRWord {
public: 
    ISREndDoc(EndDocPostingList endDocPostingList) {
        postingList = endDocPostingList;
        currentPostingsIndex = 0;
        // TODO: May need to change to pointer
        currentPost = Post(0);
    }
    Post* Next ();
    Post* NextEndDoc();
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