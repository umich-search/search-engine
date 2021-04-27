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
      Post( const Post& other )
         {
         start = other.start;
         end = other.end;
         }
      Post& operator= ( const Post& other )
         {
         start = other.start;
         end = other.end;
         return *this;
         }
      // Get position in index
      Location GetStartLocation( );
      Location GetEndLocation( );
      // set start and end to the same loc
      Location SetLocation( Location loc );
<<<<<<< HEAD
>>>>>>> 9102d46 (added some prints)
=======
      Location SetLocation( Location start_, Location end_ );
>>>>>>> bd0e6e5 (resolve some mem leak)

private:
   Location start, end;
};
