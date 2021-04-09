#include "Post.h"

// Get position in index
Location Post::GetStartLocation( ) {
    return start;
};

Location Post::GetEndLocation( ) {
    return end;
};

Location Post::SetLocation(Location location) {
    start = end = location;
    return start;
}