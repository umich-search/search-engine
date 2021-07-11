#include "Post.h"

// Get position in index
Location Post::GetStartLocation( ) 
    {
    return start;
    };

Location Post::GetEndLocation( ) 
    {
    return end;
    };

Location Post::SetLocation(Location location) 
    {
    start = end = location;
    return start;
    }

Location Post::SetLocation( Location start_, Location end_ )
    {
    start = start_;
    end = end_;
    return start;
    }