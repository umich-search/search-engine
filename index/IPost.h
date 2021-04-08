#pragma once

#include "Global.h"

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
    IPostEndDoc(Location d) {
        delta = d;
    }
    
    IPostEndDoc() {
        delta = -1;
    }
    
    Location delta;
};
