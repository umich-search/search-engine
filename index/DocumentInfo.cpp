#include "params.h"
#include "DocumentInfo.h"


size_t DocumentInfo::getNumberOfUniqueWords() {
    return numUniqueDocWords;
};

void DocumentInfo::incrementUniqueNumberOfWords() {
    numUniqueDocWords++;
}

size_t DocumentInfo::getNumberOfWords() {
    return numDocWords;
};

void DocumentInfo::incrementNumberOfWords(){
    numDocWords++;
};

size_t DocumentInfo::getPrevEndLocation() {
    return prevEndLocation;
};

char* DocumentInfo::getTitle() {
    return title;
};

char* DocumentInfo::getURL() {
    return URL;
};
// TODO: typo
void DocumentInfo::reset( size_t dID, Location recenEndDocLocation ){
    numDocWords = 0;
    numUniqueDocWords = 0;
    prevEndLocation = recenEndDocLocation;
    DocID = dID;
};
