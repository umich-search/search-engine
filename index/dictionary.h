
//#include "stdlib.h"
#pragma once
#include "ISR.h"
#include "DictionarySerializer.h"
#include "../utility/string.h"
#include "params.h"
//#include "../utility/HashTable.h"
//TODO: Fix include multi errors



class Dictionary {
public:
    // return an ISRWord object given word
    ISRWord* OpenISRWord(  char*word );
    // return then ISREndDoc object 
    ISREndDoc* OpenISREndDoc( );
    // Get number of words in index
    Location GetNumberOfWords( );
    // Get number of unique words in index
    Location GetNumberOfUniqueWords( );
    // Get number of documents
    Location GetNumberOfDocuments( );
    // constructor
    Dictionary() {
        numberOfUniqueWords = 0;
        numberOfDocuments = 0;
        numberOfWords = 0;
    }

private:
    // number of unique words in the index
    size_t numberOfUniqueWords;
    // number of documents in the index
    size_t numberOfDocuments;
    // number of words in the index
    Location numberOfWords;
    
    size_t numChunks;

    HashBlob indexBlob;
};
