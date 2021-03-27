#include "IPost.h"
#include "SyncIndex.h"
#include "../utility/Vector.h"

#define MAX_POSTS 1000
#define NUM_SYNC_POINTS 100 
#define MAX_WORD_LENGTH 100

typedef size_t Location;
// TODO: Need to figure out optimal size
struct SyncIndex {
    size_t syncPoints[NUM_SYNC_POINTS]; // high eight bits
    size_t postingsListOffset[NUM_SYNC_POINTS]; // offset in postings list
    size_t postLocation[NUM_SYNC_POINTS]; // location in index
};

enum Type{
    Document,
    URL,
    Anchor,
    Title,
    Body
};
struct CommonHeader{
    char term[MAX_WORD_LENGTH];
    size_t numOfOccurence;
    size_t numOfDocument;
    Type type;
    size_t size;
};

struct TermPostingList {
    CommonHeader header;
    IPostTerm posts[MAX_POSTS];
    SyncIndex syncIndex;
};

struct EndDocPostingList {
    CommonHeader header;
    IPostEndDoc posts[MAX_POSTS];
    SyncIndex syncIndex;
};

size_t seekTermTarget(TermPostingList *postings, size_t target, size_t &index);

size_t seekEndDocTarget(EndDocPostingList *postings, size_t target, size_t &index);





/*
    public: 
        size_t getSize();
        virtual Post *Next();
        virtual Post *Seek( Location target );
        virtual int Insert( Post post );
    protected:
        // the number of post in the posting list
        size_t listSize;
        size_t currentPostIndex;
    
};
/*
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
*/