#pragma once

#include "global.h"

class Post {
public:
    Post(Location indexLoc) {
        loc = indexLoc;
    }
    // Get position in index
    Location GetStartLocation( );
    // Get position in index
    Location GetEndLocation( );
private:
    Location loc;
    //virtual Attributes GetAttributes( );};
};
