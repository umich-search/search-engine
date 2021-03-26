#include "Post.h"
#include "SyncIndex.h"
#include "../utility/Vector.h"

struct SyncIndex {
    vector<size_t> syncPoints;
    vector<size_t> postingsListOffset;
    vector<size_t> postLocation;
};

class PostingList {

    public: 
        size_t getSize();
        virtual Post *Next();
        virtual Post *Seek( Location target );
        virtual int Insert( Post post );
    protected:
        size_t listSize;
        size_t currentPostIndex;
};

class EndDocList : PostingList {
    EndDocList(){}
    public:
        Post *Next();
        Post *Seek( Location target );
        int Insert( EndDoc post );
    private:
        SyncIndex syncIndex;
        vector<EndDoc*> posts;

};

class TermList : PostingList {
    TermList(){}
    public:
        size_t getNumberOfDocuments();
        Post *Next();
        Post *Seek( Location target );
        int Insert ( Term post );
        const char* term;

    private:
        SyncIndex syncIndex;
        size_t numberOfDocuments;
        vector<Term*> posts;

};