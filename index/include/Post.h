#pragma once

#include "Global.h"

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
      Location SetLocation( Location start_, Location end_ );

private:
   Location start, end;
};
