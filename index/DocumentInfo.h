#include "../utility/HashTable.h"
#include "../utility/Vector.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
//#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>


// DocumentInfo class to collect per document information during construction
class DocumentInfo {
    public:
    DocumentInfo() : numDocWords(0), numUniqueDocWords(0), prevEndLocation(0) {}
    
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
    private:
        size_t numDocWords;
        size_t numUniqueDocWords;
        size_t prevEndLocation;
        char* title;
        char* URL;
};

//SyncIndex(size_t initialElements) : postingsListOffset(initialElements, -1), postLocation(initialElements, -1){

