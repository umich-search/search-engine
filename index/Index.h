#include "stdlib.h"
#include "../utility/Vector.h"
#include "../utility/string.h"
#include "PostingList.h"
#include <unordered_map>


class Index {
    public:
        std::unordered_map<String, PostingList> index;
        int insert(Post post);
        Post* first (Post post);
        Post* last (Post post);
        Post* next (Post post, size_t current);
        


    // insert (term);
    // first (term);
    // last (term);
    // next (t, current);
    // process document

}