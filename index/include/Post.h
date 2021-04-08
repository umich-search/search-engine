#pragma once

#include "Global.h"

class Post {
public:
    Post(Location indexLoc) {
        loc = indexLoc;
    }
    // Get position in index
    Location GetLocation();
    Location SetLocation(Location loc);

private:
    Location loc;
};
