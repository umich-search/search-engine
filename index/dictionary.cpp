#include "Dictionary.h"
#include "PostingList.h"
#include "ISR.h"
#include "DictionarySerializer.h"


ISRWord* Dictionary::OpenISRWord( char* word ) {

}
ISREndDoc* Dictionary::OpenISREndDoc( ) {

}

Location Dictionary::GetNumberOfWords() {
    return numberOfWords;
}

Location Dictionary::GetNumberOfUniqueWords( ) {
    return numberOfUniqueWords;
}

Location Dictionary::GetNumberOfDocuments() {
    return numberOfDocuments;
}
/*

int IndexConstructor::Insert( String title, String URL) {
    IPostEndDoc lastDoc;
    lastDoc.lengthOfDocument = currDocInfo.getNumberOfWords();
    lastDoc.numUniqueWords = currDocInfo.getNumberOfUniqueWords();
    lastDoc.delta = endLocation - currDocInfo.getPrevEndLocation();
    lastDoc.title = currDocInfo.getTitle();
    lastDoc.URL = currDocInfo.getURL();

    endDocPostings.posts.pushBack(lastDoc);
    numberOfDocuments++;
    currDocInfo.reset(numberOfDocuments, endLocation);
    // Skip location between endDoc positions;
    endLocation+=2;
    return 0;
}
*/
int IndexConstructor::Insert( String term, Type type ) {
    // Get either currently existing postings or create a new one
    CommonHeader header;
    TermPostingList newPostings;
    ConstructionData newConstructionData;
    size_t delta;

    newConstructionData.currDoc = currDocInfo.DocID;
    header.type = type;
    header.term = term.cstr();
    newPostings.header = header;
    // TODO: Prob ened to use new operator

    TermPostingList * postings = termIndex.Find(header.term, &newPostings)->value;
    ConstructionData * cd = &constructionData.Find( header.term, newConstructionData)->value;
    

    if(postings->posts.size() == 0){
        delta = 0;
        numberOfUniqueWords++;
        cd->firstTermLoc = endLocation;
    } 
    else {
        delta = endLocation - cd->latestTermLoc;
    }
    if(cd->currDoc != currDocInfo.DocID) {
        postings->header.numOfDocument++;
        currDocInfo.incrementUniqueNumberOfWords();
        numberOfUniqueWords++;
    }
    postings->posts.pushBack(IPostTerm(delta));
    cd->latestTermLoc = endLocation;
    currDocInfo.incrementNumberOfWords();
    numberOfWords++;
    endLocation++;
    return 0;
};

void IndexConstructor::optimizeIndex() {
    termIndex.Optimize();
}

void IndexConstructor::createNewChunk() {
    char* filename;// = ""
    // TODO: May not be able to use sprintf
    sprintf(filename, "chunk_%d", chunkNum);
    const char *fn = filename;
    HashFile h(filename, &termIndex);
    
    chunkNum++;

}

// TODO: I think ptr passing is off
void IndexConstructor::createSynchronization() {
    size_t leftShift = numberOfWords / NUM_SYNC_POINTS;
    size_t numHighBits = sizeof(size_t) * 8 - leftShift;
    size_t numLowBits = leftShift;
    for ( HashTable< const char*, TermPostingList *>::Iterator iterator = termIndex.begin(); iterator != termIndex.end( ); ++iterator ) {
        createSeekIndex(iterator->value, numLowBits);
    }
    createSeekIndex(&endDocPostings, numLowBits);
}

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