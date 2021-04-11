#pragma once

#include "DictionarySerializer.h"
#include "DocumentInfo.h"
#include "DocumentsSerializer.h"
#include "EndDocSerializer.h"
#include "Global.h"
#include "FileManager.h"

class IndexConstructor {
public:
    IndexConstructor(w_Occurence uniqueWords, d_Occurence numDocs, w_Occurence numWords, Location firstDocEnd, Location endLocation, Location end, size_t currChunkNum) :
                        fileManager(0),
                        numberOfUniqueWords(uniqueWords),
                        numberOfDocuments(numDocs),
                        numberOfWords(numWords),
                        firstDocEnd(firstDocEnd),
                        endLocation(endLocation),
                        currentChunkNum(currChunkNum),
                        chunkMemoryAlloc(0)
                        {
                            endDocPostings = SharedPointer<EndDocPostingList>(new EndDocPostingList(NUM_SYNC_POINTS));
                            constructionData = SharedPointer<HashTable< String, ConstructionData*>>(new HashTable<String, ConstructionData*>());
                            termIndex = SharedPointer<HashTable< String, TermPostingList*>>(new HashTable<String, TermPostingList*>());
                        }
    
    IndexConstructor(size_t threadID = 0) :
                        fileManager(threadID),
                        numberOfUniqueWords(0),
                        numberOfDocuments(fileManager.getNumDocuments()),
                        numberOfWords(fileManager.getIndexWords()),
                        firstDocEnd(0),
                        endLocation(0),
                        currentChunkNum(fileManager.getNumChunks()),
                        threadID(threadID)
                        {
                            //std::cout << "Created Index constructor with numDocs: " << numberOfDocuments << " numChunks: " << currentChunkNum << std::endl;
                            endDocPostings = SharedPointer<EndDocPostingList>(new EndDocPostingList(NUM_SYNC_POINTS));
                            constructionData = SharedPointer<HashTable< String, ConstructionData*>>(new HashTable<String, ConstructionData*>());
                            termIndex = SharedPointer<HashTable< String, TermPostingList*>>(new HashTable<String, TermPostingList*>());
                        }
    
    ~IndexConstructor() {
    }
    
    int Insert ( String title, String URL );
    int Insert( String term, Type type );
    int FinishConstruction();

    
public:
    FileManager fileManager;
    SharedPointer<HashTable< String, TermPostingList*>> termIndex;
    SharedPointer<HashTable< String, ConstructionData*>> constructionData;
    SharedPointer<EndDocPostingList> endDocPostings;
    ::vector<SharedPointer<DocumentDetails>> docDetails;
    DocumentInfo currDocInfo;
    w_Occurence numberOfUniqueWords;
    d_Occurence numberOfDocuments;
    w_Occurence numberOfWords;
    Location firstDocEnd; 
    Location endLocation;
    size_t currentChunkNum;
    size_t chunkMemoryAlloc;
    size_t threadID;
    void optimizeIndex();
    void createSynchronization();
    void createNewChunk();
    int flushData();
    int resolveChunkMem();
};

