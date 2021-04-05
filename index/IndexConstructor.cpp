#include "IndexConstructor.h"
#include "DocumentsSerializer.h"


int IndexConstructor::Insert( String title, String URL) {
    // TODO: Is this correct?
    if(firstDocEnd == 0) {
        firstDocEnd = endLocation;
        // TODO: Porb not a good idea to hardcode liket this
    }
    IPostEndDoc lastDoc;
    if(endDocPostings.posts.size() == 0){
        lastDoc.delta = 0;
    }
    else {
        lastDoc.delta = endLocation - currDocInfo.getPrevEndLocation();
    }
    endDocPostings.posts.pushBack(lastDoc);
    
    // TODO: Proper location pushing?
    // TODO: Do we need doc id
    // TODO: DocID could be safegaurd if something goes wrong
    docDetails.pushBack(new DocumentDetails(URL.cstr(), title.cstr(), currDocInfo.getNumberOfWords(), currDocInfo.getNumberOfUniqueWords() ));
    numberOfDocuments++;
    // TODO: Prob don't need to actually do this
    endDocPostings.header.numOfDocument++;
    currDocInfo.reset(numberOfDocuments, endLocation);
    // Skip location between endDoc positions;
    endLocation+=2;
    //resolveChunkMem();
    return 0;
}

int IndexConstructor::resolveChunkMem() {
    chunkMemoryAlloc += DOCUMENT_SIZE;
    if(chunkMemoryAlloc > CHUNK_SIZE_BYTES) {
        createSynchronization();
        optimizeIndex();
        createNewChunk();
    }
    return 0;
}


int IndexConstructor::Insert( String term, Type type ) {
    // Get either currently existing postings or create a new one
    CommonHeader header; //= new CommonHeader;
    TermPostingList *postings = nullptr;
    ConstructionData *cd = nullptr;
    size_t delta;
    // TODO: Prob ened to use new operator

    Tuple<String, TermPostingList*> * termTuple = termIndex.Find(term);//, postings);//->value;
    Tuple<String, ConstructionData*> * cdTuple = constructionData.Find( term) ;//, cd);//->value;
    // TODO: Can prob provide beter schemantics

    if(termTuple) {
        postings = termTuple->value;
        cd = cdTuple->value;
        delta = endLocation - cd->latestTermLoc;

    } else {
        // TODO: Change num sync points
        postings = new TermPostingList(NUM_SYNC_POINTS);
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
        // TODO: Move somewhere else probably
        chunkMemoryAlloc += sizeof(w_Occurence) + sizeof(d_Occurence) + sizeof(type) + strlen(term.cstr()) + 1;
    }
    // TODO: Move somewhere else probably
    chunkMemoryAlloc += sizeof(size_t) * 3;
    cd->latestTermLoc = endLocation;

    if(cd->currDoc != currDocInfo.DocID) {
        postings->header.numOfDocument++;
        currDocInfo.incrementUniqueNumberOfWords();
        cd->currDoc = currDocInfo.DocID;
        //numberOfUniqueWords++;
    }
    postings->posts.pushBack(IPostTerm(delta));
    currDocInfo.incrementNumberOfWords();
    numberOfWords++;
    endLocation++;
    postings->header.numOfOccurence++;

    return 0;
};

void IndexConstructor::optimizeIndex() {
    termIndex.Optimize();
}

int IndexConstructor::flushData() {
    char *filename;
    sprintf(filename, "chunk_%zu", currentChunkNum);
    HashFile h(filename, &termIndex);
    createNewChunk();
    return 0;
}

void IndexConstructor::createNewChunk() {
    currentChunkNum++;
}

// returns number of low bits given total count and number of syncs
size_t getNumLowBits(size_t count, size_t spacing) {
    size_t leftShift = ( count + spacing - 1 )/ spacing;
    int numLowBits=0;
    // While loop will run until we get n = 0
    while(leftShift)
    {
        numLowBits++;
        leftShift=leftShift>>1;
    }
    return numLowBits;
}
// TODO: I think ptr passing is off
void IndexConstructor::createSynchronization() {

    size_t numLowBits = getNumLowBits(endLocation, NUM_SYNC_POINTS);
    for ( HashTable< String, TermPostingList *>::Iterator iterator = termIndex.begin(); iterator != termIndex.end( ); ++iterator ) {
        createSeekIndex(iterator->value, constructionData.Find(iterator->key)->value->firstTermLoc, numLowBits);
    }
    // TODO: Make doc temp data
    createSeekIndex(&endDocPostings, firstDocEnd, numLowBits);
}
