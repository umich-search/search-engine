
#pragma once
#include <stddef.h>  
#include "global.h"


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
};
