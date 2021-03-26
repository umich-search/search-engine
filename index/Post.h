#include "stdlib.h"
#include <unordered_map>
typedef size_t Location;

typedef union Attributes {
    //WordAttributes Word;
    //DocAttributes Doc;
    
};


class Post {
    public: 
        virtual Location GetStartLocation();
        virtual Location GetEndLocation();
        virtual Attributes GetAttributes();
        virtual Location getDelta();

    private:
        Location delta;

};

class Term : public Post {
};


class EndDoc : public Post {
};

