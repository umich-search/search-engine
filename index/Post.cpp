#include "Post.h"

// Get position in index
Location Post::GetLocation( ) {
    return loc;
};

Location Post::SetLocation(Location location) {
    loc = location;
    return loc;
}