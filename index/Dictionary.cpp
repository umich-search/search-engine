#include "Dictionary.h"

ISRWord *Dictionary::OpenISRWord(char *word) {
    //construct commonHeader
    return new ISRWord(manager, word);
}

ISREndDoc *Dictionary::OpenISREndDoc() {
    return nullptr;
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
