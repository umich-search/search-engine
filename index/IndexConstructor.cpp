#include "IndexConstructor.h"
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
    //cout << "Inserting: " << term.cstr() << endl;
    // Get either currently existing postings or create a new one
    CommonHeader header; //= new CommonHeader;
    TermPostingList *postings = nullptr;
    ConstructionData *cd = nullptr;
    size_t delta;
    // TODO: This is size_t isn't it
    //newConstructionData->currDoc = -2;//currDocInfo.DocID;
    /*
    header.type = type;
    cout << "Assigning header to be  " << term.cstr() << endl;
    header.term = term.cstr();

    // TODO: Prob ened to use new operator
    cout << "Searching for: " << term.cstr() << endl;
    const char* str = term.cstr();
    */
    /*
        for(unsigned int i = 0; i < 10; ++i){
            cout << "sstr[" << i << "]: " << str[i] << " ";
        }
        cout << endl;
        */
    Tuple<String, TermPostingList*> * termTuple = termIndex.Find(term);//, postings);//->value;
    Tuple<String, ConstructionData*> * cdTuple = constructionData.Find( term) ;//, cd);//->value;
    // TODO: Can prob provide beter schemantics

    if(termTuple) {
        //cout << "Old Term: " << term.cstr() << endl;
        postings = termTuple->value;
        cd = cdTuple->value;
    } else {
        //cout << "New Term: " << term.cstr() << endl;
        // TODO: Change num sync points
        postings = new TermPostingList(8);
        cd = new ConstructionData;

        postings->header = header;
        postings->header.term = String(term);
        postings->header.type = type;
        postings->header.numOfDocument = 0;
        postings->header.numOfOccurence = 0;
        cd->currDoc = -2;//currDocInfo.DocID;
        cd->firstTermLoc = endLocation;
        cd->latestTermLoc = endLocation;

        termIndex.Find(term, postings);
        constructionData.Find(term, cd);

        delta = 0;
        numberOfUniqueWords++;
    }
/*
    if(postings->posts.size() == 0){
        delta = 0;
        numberOfUniqueWords++;
        cd->firstTermLoc = endLocation;
    } 
    */
   // TODO: Can prob concat w above if
        //cout << "end location: " << endLocation << " latestTermLoc: " << cd->latestTermLoc << endl;

    if(postings->posts.size() > 0) {
        delta = endLocation - cd->latestTermLoc;
        //cd->latestTermLoc = endLocation;
    }

    cd->latestTermLoc = endLocation;

    if(cd->currDoc != currDocInfo.DocID) {
        postings->header.numOfDocument++;
        currDocInfo.incrementUniqueNumberOfWords();
        cd->currDoc = currDocInfo.DocID;
        numberOfUniqueWords++;
    }
    postings->posts.pushBack(IPostTerm(delta));
    //cout << "Pushed back another post with delta " << delta << endl;
    //cout << "Posts size: " << postings->posts.size() << endl;
    currDocInfo.incrementNumberOfWords();
    numberOfWords++;
    endLocation++;
    //cout << "num of occurence before adding: " << postings->header.numOfOccurence << endl;
    postings->header.numOfOccurence++;
    //cout << "num of occurence after adding: " << postings->header.numOfOccurence << endl;

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
    size_t leftShift = numberOfWords / 8;///NUM_SYNC_POINTS;
    // TODO: Change to dynamic caluclation of num low bits
    size_t numHighBits = sizeof(size_t) * 8 - leftShift;
    int count=0;
    // While loop will run until we get n = 0
    while(leftShift)
    {
        count++;
        // We are shifting n to right by 1
        // place as explained above
        leftShift=leftShift>>1;
    }
    size_t numLowBits = count;
    for ( HashTable< String, TermPostingList *>::Iterator iterator = termIndex.begin(); iterator != termIndex.end( ); ++iterator ) {
        createSeekIndex(iterator->value, constructionData.Find(iterator->key)->value->firstTermLoc, numLowBits);
    }
    // TODO: Make doc temp data
    //createSeekIndex(&endDocPostings, numLowBits);
}
