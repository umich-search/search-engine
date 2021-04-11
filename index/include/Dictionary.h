
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
    Dictionary(FileManager filemanager, size_t threadID) : manager(threadID) {
        manager = filemanager;
        numberOfDocuments = filemanager.getNumDocuments();
        numberOfWords = filemanager.getIndexWords();
        numChunks = filemanager.getNumChunks();
    }
    // Default constructor
    Dictionary(size_t threadID) : manager(threadID){
        numberOfDocuments = manager.getNumDocuments();
        numberOfWords = manager.getIndexWords();
        numChunks = manager.getNumChunks();
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
