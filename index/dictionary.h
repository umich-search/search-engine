
//#include "stdlib.h"
#include "ISR.h"
#include "DictionarySerializer.h"
#include "../utility/string.h"
#include "../utility/HashTable.h"
typedef size_t Location; 


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

    HashBlob indexBlob;
    size_t numChunks;
    /*
    // Dict of Terms to posting lists
    HashTable< const char*, TermPostingList> termIndex;
    // Dict of Terms to relevant term data during construction
    HashTable< const char*, ConstructionData> constructionData;
    */
    // EndDoc postings list
    //EndDocPostingList endDocPostings;
    //size_t numTerms;
    // Current document info
    //DocumentInfo currDocInfo;
    /*
    // optimize the index
    void optimizeIndex();
    // creates synchronization points for every postings list
    void createSynchronization();
    // creates hashblob for the termIndex
    void createHashBlob();
    // TODO: Create blbo for endDocPostingsList
    */
};

class IndexConstructor {
public:
    IndexConstructor(size_t c_size) {
        numberOfUniqueWords = 0;
        numberOfDocuments = 0;
        endLocation = 0;
        numberOfWords = 0;
        chunkNum = 0;
        chunkSize = c_size;
        currentChunkSize = 0;
    }
    // Insert term
//int IndexConstructor::Insert( String term, Type type ) {

    int Insert( String term, Type type );
    // Insert document
    //int Insert ( String title, String URL );
    
public:

    // DocumentInfo class to collect per document information during construction
    class DocumentInfo {
        public:
            size_t DocID; // DocumentID
            // increment
            void incrementNumberOfWords(); 
            void incrementUniqueNumberOfWords();
            void reset( size_t DocID, Location recentEndDocLocation );
            char* getTitle();
            char* getURL();
            size_t getNumberOfWords();
            size_t getNumberOfUniqueWords();
            Location getPrevEndLocation(); 
            HashTable< const char*, int > wordSet; // TODO: Not used
        private:
            size_t numDocWords;
            size_t numUniqueDocWords;
            size_t prevEndLocation;
            char* title;
            char* URL;
    };

    // number of unique words in the index
    size_t numberOfUniqueWords;
    // number of documents in the index
    size_t numberOfDocuments;
    // number of words in the index
    size_t chunkNum;
    size_t chunkSize;
    size_t currentChunkSize;
    Location numberOfWords;
        // Dict of Terms to posting lists
    HashTable< const char*, TermPostingList*> termIndex;
    // Dict of Terms to relevant term data during construction
    HashTable< const char*, ConstructionData> constructionData;
    EndDocPostingList endDocPostings;
    DocumentInfo currDocInfo;  
 

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
};

