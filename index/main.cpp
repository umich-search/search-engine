#include <iostream>
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
        
        0 100 200 300 400 500 ...1000
        delta = 100
        sync = 1000
        posts = 1000
        max posts 1000
        syn Offset 0 10 20...
        syn Location 0 1000 2000 ...
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

    cout << isrword.Next()->GetStartLocation()<<endl;
    cout <<isrword.Next()->GetStartLocation()<<endl;
    cout << isrword.Seek(850)->GetStartLocation()<<endl;
    cout <<isrword.Next()->GetStartLocation()<<endl;
    return 0; 
}

