
#include "stdlib.h"
#include <unordered_map>
typedef size_t Location; 

class Dictionary {
public:
    ISRWord* OpenISRWord(  char*word );
    ISREndDoc* OpenISREndDoc( );
    Location GetNumberOfWords( );
    Location GetNumberOfUniqueWords( );
    Location GetNumberOfDocuments( );
private:
    std::unordered_map<std::string, size_t> dictionary;
};

class Post {
public:
    // Get position in index
    virtual Location GetStartLocation( );
    // Get position in index
    virtual Location GetEndLocation( );
    //virtual Attributes GetAttributes( );};
};



class ISR {
public:
    //Get post of next term
    Post* Next( );
    // Get post of next endDoc
    Post* NextEndDoc( );
    // Get the first post after the target
    virtual Post* Seek( Location target);
    // Get position of first term
    virtual Location GetStartLocation( );
    // Get position of last term
    virtual Location GetEndLocation( );
};

class ISRWord: public ISR {
public:
    // get number of documents word is in
    unsigned GetDocumentCount( );
    // get number of occurances in index
    unsigned GetNumberOfOccurrences( );
    // get current post
    virtual Post* GetCurrentPost( );
};

class ISREndDoc: public ISRWord {
public: 
    // get length of document
    unsigned GetDocumentLength();
    // get length of document title
    unsigned GetTitleLength();
    // get length of url
    unsigned GetUrlLength();

}