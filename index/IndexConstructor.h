#include "DictionarySerializer.h"
#include "DocumentInfo.h"
#include "DocumentsSerializer.h"
#include "params.h"
#include "../utility/string.h"


class IndexConstructor {
public:
    IndexConstructor(w_Occurence uniqueWords, d_Occurence numDocs, w_Occurence numWords, Location endLocation, size_t currChunkNum) :
                        numberOfUniqueWords(uniqueWords),
                        numberOfDocuments(numDocs),
                        numberOfWords(numWords),
                        endLocation(endLocation),
                        currentChunkNum(currChunkNum),
                        chunkMemoryAlloc(0),
                        firstDocEnd(0),
                        endDocPostings(8){}
    
    IndexConstructor() : numberOfUniqueWords(0), numberOfDocuments(0), numberOfWords(0), endLocation(0), currentChunkNum(0), chunkMemoryAlloc(0), firstDocEnd(0), endDocPostings(8) {}
    
    int Insert ( String title, String URL );
    int Insert( String term, Type type );

    
public:
    // TODO: Change public/ private and fix tests accordingly when ready
    // TODO: See if initalizer list ordering matters
    // number of unique words in the index
    w_Occurence numberOfUniqueWords;
    // number of documents in the index
    d_Occurence numberOfDocuments;
    // number of words in the index
    size_t currentChunkNum;
    size_t chunkMemoryAlloc;
    Location numberOfWords;
    Location firstDocEnd; // TODO: Prob need to ptut this somewhere
    // Dict of Terms to posting lists
    HashTable< String, TermPostingList*> termIndex;
    // Dict of Terms to relevant term data during construction
    HashTable< String, ConstructionData*> constructionData;
    EndDocPostingList endDocPostings;
    DocumentInfo currDocInfo;
    ::vector<DocumentDetails *> docDetails;

    // One after the current index position
    Location endLocation;
    // optimize the index
    void optimizeIndex();
    // creates synchronization points for every postings list
    void createSynchronization();
    // creates hashblob for the termIndex
    void createHashBlob();
    // TODO: Create blbo for endDocPostingsList
    void createNewChunk();
    // Flushes to chunk to file if chunk size is reached
    int flushData();
    
    int resolveChunkMem();
};

