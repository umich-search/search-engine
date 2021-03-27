#include "Dictionary.h"
#include "ISR.h"

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
  size_t numberOfWords;
    size_t numberOfUniqueWords;
    size_t numberOfDocuments;
