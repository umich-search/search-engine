#pragma once

#include "Global.h"

class Post {
public:
   Post(Location indexLoc)
      {
      start = end = indexLoc;
      }
   Post(Location startLoc, Location endLoc) 
      {
      start = startLoc;
      end = endLoc;
      }
   // Get position in index
   Location GetStartLocation();
   Location GetEndLocation();
   Location SetLocation(Location loc);

private:
   Location start, end;
};
