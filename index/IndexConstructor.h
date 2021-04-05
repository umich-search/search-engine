#pragma once

#include "DictionarySerializer.h"
#include "DocumentInfo.h"
#include "DocumentsSerializer.h"
#include "EndDocSerializer.h"
#include "Global.h"

class IndexConstructor {
public:
    IndexConstructor(w_Occurence uniqueWords, d_Occurence numDocs, w_Occurence numWords, Location firstDocEnd, Location endLocation, Location end, size_t currChunkNum) :
                        endDocPostings(NUM_SYNC_POINTS),
                        numberOfUniqueWords(uniqueWords),
                        numberOfDocuments(numDocs),
                        numberOfWords(numWords),
                        firstDocEnd(firstDocEnd),
                        endLocation(endLocation),
                        currentChunkNum(currChunkNum),
                        chunkMemoryAlloc(0)
                        {}
    
    IndexConstructor() :
                        endDocPostings(NUM_SYNC_POINTS),
                        numberOfUniqueWords(0),
                        numberOfDocuments(0),
                        numberOfWords(0),
                        firstDocEnd(0),
                        endLocation(0),
                        currentChunkNum(0),
                        chunkMemoryAlloc(0)
                        {}
    
    int Insert ( String title, String URL );
    int Insert( String term, Type type );

    
public:
    HashTable< String, TermPostingList*> termIndex;
    HashTable< String, ConstructionData*> constructionData;
    EndDocPostingList endDocPostings;
    ::vector<DocumentDetails *> docDetails;
    DocumentInfo currDocInfo;
    w_Occurence numberOfUniqueWords;
    d_Occurence numberOfDocuments;
    Location numberOfWords;
    Location firstDocEnd; 
    Location endLocation;
    size_t currentChunkNum;
    size_t chunkMemoryAlloc;

    void optimizeIndex();
    void createSynchronization();
    void createHashBlob();
    void createNewChunk();

    int flushData();
    int resolveChunkMem();
    // TODO: Change public/ private and fix tests accordingly when ready
};

