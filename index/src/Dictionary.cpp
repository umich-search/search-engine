#include "Dictionary.h"

ISRWord *Dictionary::OpenISRWord(const char *word) {
    //construct commonHeader
    return new ISRWord(manager, word);
}

ISREndDoc *Dictionary::OpenISREndDoc() {
    return new ISREndDoc(manager);
}

Location Dictionary::GetNumberOfWords() {
    return numberOfWords;
}

Location Dictionary::GetNumberOfUniqueWords() {
    return numberOfUniqueWords;
}

Location Dictionary::GetNumberOfDocuments() {
    return numberOfDocuments;
}

DocumentDetails * Dictionary::GetDocumentDetials( Offset index ) {
    ::vector<d_Occurence> docOccurenceAfterChunk = manager.getDocCountsAfterChunk();
    
    size_t currChunk = 0;
    for(; currChunk < docOccurenceAfterChunk.size(); ++currChunk ) {
        if(index < docOccurenceAfterChunk[currChunk]) {
            break;
        }
    }
    if(currChunk == docOccurenceAfterChunk.size()) {
        return nullptr;
    }
    else {
        DocumentDetails *dd = new DocumentDetails(manager.GetDocumentDetails(index, currChunk));
        return dd;
    }
}

