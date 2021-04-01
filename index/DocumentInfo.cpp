#include "IndexConstructor.h"

size_t IndexConstructor::DocumentInfo::getNumberOfUniqueWords() {
    return numUniqueDocWords;
};

void IndexConstructor::DocumentInfo::incrementUniqueNumberOfWords() {
    numUniqueDocWords++;
}

size_t IndexConstructor::DocumentInfo::getNumberOfWords() {
    return numDocWords;
};

void IndexConstructor::DocumentInfo::incrementNumberOfWords(){
    numDocWords++;
};

size_t IndexConstructor::DocumentInfo::getPrevEndLocation() {
    return prevEndLocation;
};

char* IndexConstructor::DocumentInfo::getTitle() {
    return title;
};

char* IndexConstructor::DocumentInfo::getURL() {
    return URL;
};

void IndexConstructor::DocumentInfo::reset( size_t dID, Location recenEndDocLocation ){
    numDocWords = 0;
    numUniqueDocWords = 0;
    prevEndLocation = recenEndDocLocation;
    DocID = dID;
};