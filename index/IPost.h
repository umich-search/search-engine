#include "stdlib.h"
typedef size_t Location;
#define MAX_TITLE_LENGTH 500
#define MAX_URL_LENGTH 500

typedef union Attributes {
    //WordAttributes Word;
    //DocAttributes Doc;
    
};

struct IPostTerm {
    size_t delta;
};

struct IPostEndDoc {
    size_t delta;
    size_t lengthOfDocument;
    size_t numOfAnchorText;
    size_t numUniqueWords;
    char URL[MAX_URL_LENGTH];
    char title[MAX_TITLE_LENGTH];
};