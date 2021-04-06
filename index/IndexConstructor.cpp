#include "IndexConstructor.h"
#include "DocumentsSerializer.h"

using TermHash = HashTable< String, TermPostingList* >;
using ConstructionHash = HashTable< String, ConstructionData*>;


int IndexConstructor::Insert( String title, String URL) {
    IPostEndDoc lastDoc;
    if(endDocPostings->posts.size() == 0){
        lastDoc.delta = 0;
        firstDocEnd = endLocation;
    }
    else {
        lastDoc.delta = endLocation - currDocInfo.getPrevEndLocation();
    }
    
    endDocPostings->posts.pushBack(lastDoc);
    SharedPointer<DocumentDetails> sharedDoc(new DocumentDetails(URL.cstr(),
                                                                 title.cstr(),
                                                                 currDocInfo.getNumberOfWords(),
                                                                 currDocInfo.getNumberOfUniqueWords())
                                             );
    docDetails.pushBack(sharedDoc);
    numberOfDocuments++;
    endDocPostings->header.numOfDocument++;
    currDocInfo.reset(numberOfDocuments, endLocation);
    endLocation+=2;
    if(WRITE_TO_DISK) {
        resolveChunkMem();
    }
    return 0;
}

int IndexConstructor::resolveChunkMem() {
    chunkMemoryAlloc += DOCUMENT_SIZE;
    //if(chunkMemoryAlloc > CHUNK_SIZE_BYTES) {
        createSynchronization();
    //    optimizeIndex();
        flushData();
    
    endDocPostings = SharedPointer<EndDocPostingList>(new EndDocPostingList(NUM_SYNC_POINTS));
    termIndex = SharedPointer<TermHash>(new TermHash);
    constructionData = SharedPointer<ConstructionHash>(new ConstructionHash);
    docDetails = ::vector<SharedPointer<DocumentDetails>>();
    
    return 0;
}


int IndexConstructor::Insert( String term, Type type ) {
    CommonHeader header;
    TermPostingList *postings = nullptr;
    ConstructionData *cd = nullptr;
    size_t memoryAlloc = 0;
    size_t delta;

    Tuple<String, TermPostingList*> * termTuple = termIndex->Find(term);
    Tuple<String, ConstructionData*> * cdTuple = constructionData->Find( term);

    if(termTuple) {
        postings = termTuple->value;
        cd = cdTuple->value;
        delta = endLocation - cd->latestTermLoc;

    } else {
        postings = new TermPostingList(NUM_SYNC_POINTS);
        postings->header = header;
        postings->header.term = String(term);
        postings->header.type = type;
        postings->header.numOfDocument = 0;
        postings->header.numOfOccurence = 0;
        cd = new ConstructionData;
        cd->currDoc = -2;
        cd->firstTermLoc = endLocation;
        cd->latestTermLoc = endLocation;

        termIndex->Find(term, postings);
        constructionData->Find(term, cd);
        delta = 0;
        numberOfUniqueWords++;
        memoryAlloc += sizeof(w_Occurence) + sizeof(d_Occurence) + sizeof(type) + strlen(term.cstr()) + 1;
        memoryAlloc += NUM_SYNC_POINTS * 2 * sizeof(size_t);

    }
   /// memoryAlloc += sizeof(size_t) * 3;

    if(cd->currDoc != currDocInfo.DocID) {
        postings->header.numOfDocument++;
        currDocInfo.incrementUniqueNumberOfWords();
        cd->currDoc = currDocInfo.DocID;
    }
    
    postings->posts.pushBack(IPostTerm(delta));
    postings->header.numOfOccurence++;
    cd->latestTermLoc = endLocation;

    currDocInfo.incrementNumberOfWords();
    numberOfWords++;
    endLocation++;
    chunkMemoryAlloc += memoryAlloc;
    return 0;
};

void IndexConstructor::optimizeIndex() {
    termIndex->Optimize();
}

int IndexConstructor::flushData() {
    char chunkFilename[100];
    char docsFilename[100];
    sprintf(chunkFilename, "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/%zu.chunk", currentChunkNum);
    sprintf(docsFilename, "/Users/andrewjiang/Desktop/s-engine/search-engine/index/gen_files/%zu.chunkd", currentChunkNum);
    fileManager.WriteChunk(termIndex,
                           endDocPostings,
                           numberOfWords,
                           numberOfUniqueWords,
                           numberOfDocuments,
                           endLocation,
                           docDetails,
                           currentChunkNum,
                           chunkFilename,
                           docsFilename);
    return 0;
}

void IndexConstructor::createNewChunk() {
    currentChunkNum++;
}

// returns number of low bits given total count and number of syncs
size_t getNumLowBits(size_t count, size_t spacing) {
    size_t leftShift = ( count + spacing - 1 )/ spacing;
    int numLowBits=0;
    leftShift--;
    while(leftShift)
    {
        numLowBits++;
        leftShift=leftShift>>1;
    }
    return numLowBits;
}
void IndexConstructor::createSynchronization() {
    size_t numLowBits = getNumLowBits(endLocation, NUM_SYNC_POINTS);
    
    for (  TermHash::Iterator iterator = termIndex->begin();
            iterator != termIndex->end( );
            ++iterator ) {
                        createSeekIndex(iterator->value,
                            constructionData->Find(iterator->key)->value->firstTermLoc,
                            numLowBits
                        );
    }
    createSeekIndex(endDocPostings, firstDocEnd, numLowBits);
}
