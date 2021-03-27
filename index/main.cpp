#include "ISR.h"
using namespace std;
int main (int argc, char *argv[]) 
{ 
    
    CommonHeader header;
    header.term[0] = 'd';
    header.term[1] = 'o';
    header.term[2] = 'g';
    header.term[3] = '\0';
    header.numOfOccurence=100;
    header.numOfDocument = 10;
    header.type = Title;
    header.size = 1024;
    /*
        10000 posts total
        64-8=56
        i=pow(2,56)
        0 i 2i 3i 
        term = dog, (100 posts)
        
        0 100 200 300 400 500 
        delta = 100
        sync = 100
        posts = 1000
        max posts 1000*100/1000=100
    */
    
    SyncIndex syncIndex;
    
    for (int i = 0;i<NUM_SYNC_POINTS;i++){
        syncIndex.postingsListOffset[i] = i*10;
        syncIndex.postLocation[i] = i*1000;
    }

    TermPostingList termPostingList;
    termPostingList.header = header;
    for (int i=0;i<MAX_POSTS;i++){
        termPostingList.posts[i].delta = 100;
    }
    termPostingList.syncIndex = syncIndex;
    ISRWord isrword = ISRWord(termPostingList);
    cout << isrword.Next()->GetStartLocation();
    
    return 0; 
}

struct SyncIndex {
    size_t syncPoints[NUM_SYNC_POINTS]; // high eight bits
    size_t postingsListOffset[NUM_SYNC_POINTS]; // offset in postings list
    size_t postLocation[NUM_SYNC_POINTS]; // location in index
};

struct TermPostingList {
    CommonHeader header;
    IPostTerm posts[MAX_POSTS];
    SyncIndex syncIndex;
};