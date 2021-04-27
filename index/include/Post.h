#pragma once

#include "Global.h"

<<<<<<< HEAD
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
=======
class Post 
   {
   public:
      Post( Location indexLoc )
         {
         start = end = indexLoc;
         }
      Post( Location startLoc, Location endLoc ) 
         {
         start = startLoc;
         end = endLoc;
         }
      // Get position in index
      Location GetStartLocation( );
      Location GetEndLocation( );
      // set start and end to the same loc
      Location SetLocation( Location loc );
>>>>>>> 9102d46 (added some prints)

private:
   Location start, end;
};
