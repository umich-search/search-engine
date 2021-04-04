
#pragma once

#include <stddef.h>  
typedef size_t Location;
#include "params.h"


//typedef union Attributes {
//    WordAttributes Word;
//    DocAttributes Doc;
//
//};

struct IPostTerm {
    IPostTerm(Location d) {
        delta = d;
    }

    IPostTerm() {
        delta = -1;
    }
    Location delta;
};

struct IPostEndDoc {
    Location delta;
    Location lengthOfDocument;
    Location numOfAnchorText;
    Location numUniqueWords;
    //char URL[MAX_URL_LENGTH];
    char *URL;
    //char title[MAX_TITLE_LENGTH];
    char *title;
};
