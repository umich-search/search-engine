
#include "stdlib.h"
#include "ISR.h"
#include <unordered_map>
typedef size_t Location; 


class Dictionary {
public:
    // return an ISRWord object given word
    ISRWord* OpenISRWord(  char*word );
    // return then ISREndDoc object 
    ISREndDoc* OpenISREndDoc( );
    // Get number of words in index
    Location GetNumberOfWords( );
    // Get number of unique words in index
    Location GetNumberOfUniqueWords( );
    // Get number of documents
    Location GetNumberOfDocuments( );
private:
    std::unordered_map<std::string, size_t> dictionary;
    size_t numberOfWords;
    size_t numberOfUniqueWords;
    size_t numberOfDocuments;
};

