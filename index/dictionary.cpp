#include "Dictionary.h"
#include "PostingList.h"
//#include "ISR.h"
//#include "DictionarySerializer.h"


ISRWord* Dictionary::OpenISRWord( char* word ) {
    return nullptr;
}
ISREndDoc* Dictionary::OpenISREndDoc( ) {
    return nullptr;
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