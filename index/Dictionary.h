
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
    Dictionary(FileManager filemanager) {
        manager = filemanager;
        numberOfUniqueWords = filemanager.getIndexUniqueWords();
        numberOfDocuments = filemanager.getNumDocuments();
        numberOfWords = filemanager.getIndexWords();
        numChunks = filemanager.getNumChunks();
    }

private:
    FileManager manager;
    // number of unique words in the index
    w_Occurence numberOfUniqueWords;
    // number of documents in the index
    d_Occurence numberOfDocuments;
    // number of words in the index
    w_Occurence numberOfWords;
    size_t numChunks;
};
