
#pragma once
#include "ISR.h"
#include "DictionarySerializer.h"
#include "Global.h"

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
    HashBlob indexBlob;
    // number of unique words in the index
    w_Occurence numberOfUniqueWords;
    // number of documents in the index
    d_Occurence numberOfDocuments;
    // number of words in the index
    w_Occurence numberOfWords;
    size_t numChunks;
};
