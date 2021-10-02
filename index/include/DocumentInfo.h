#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>


// DocumentInfo class to collect per document information during construction
class DocumentInfo {
    public:
    DocumentInfo() : numDocWords(0), numUniqueDocWords(0), prevEndLocation(0) {}
        size_t DocID; // DocumentID
        // increment number of words for document
        void incrementNumberOfWords();
        // increment number of unique words for document
        void incrementUniqueNumberOfWords();
        // reset the document ID and the enddoc location
        void reset( size_t DocID, Location recentEndDocLocation );
        // get the title
        char* getTitle();
        // get the url
        char* getURL();
        // get the number of words
        size_t getNumberOfWords();
        // get number of unique words
        size_t getNumberOfUniqueWords();
        // get end location of previous document
        Location getPrevEndLocation();
    private:
        size_t numDocWords;
        size_t numUniqueDocWords;
        size_t prevEndLocation;
        char* title;
        char* URL;
};


