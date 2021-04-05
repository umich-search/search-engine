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
    Location delta;
};
