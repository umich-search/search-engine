#include <iostream>
#include "ISR.h"
//#include "DictionarySerializer.h"
#include "IPost.h"
#include "../utility/HashTable.h"
#include "../utility/Common.h"
#include "Dictionary.h"
using namespace std;
int main (int argc, char *argv[]) 
{ 
    // CREATE POSTING LIST  
    CommonHeader header;
    SyncIndex syncIndex;
    TermPostingList p;
    header.term = "dog";
    header.numOfDocument = 111;
    header.numOfOccurence = 16;
    header.type = Title;

    for(unsigned int i = 0; i < 16; ++i) {
        syncIndex.postingsListOffset.pushBack(i);
        syncIndex.postLocation.pushBack(i);
        p.posts.pushBack(IPostTerm(i));
    }
    p.header = header;
    p.syncIndex = syncIndex;

    // CREATE SINGLE BUCKET
    Bucket<const char *, TermPostingList *> testBucket(header.term, fnvHash(p.header.term, strlen(p.header.term) + 1), &p);

    // ALLOCATE BUFFER
    SerialTuple s;
    size_t bucketBytes = s.BytesRequired(&testBucket);
    char *buffer = (char *)malloc(bucketBytes);

    // TEST WRITE BUCKET TO BUFFER
    s.Write(buffer, buffer + bucketBytes, &testBucket);

    const SerialTuple *tuple = (SerialTuple *) buffer;
    cout << "Tuple length: " << tuple->Length << endl;
    cout << "Tuple hash value: " << tuple->HashValue << endl;
    const char * dynamicSpace = tuple->DynamicSpace;
    cout << "Tuple key: " << dynamicSpace << endl;
    dynamicSpace += strlen(dynamicSpace) + 1;
    cout << "Header type: " << (int)dynamicSpace[0] << endl;
    dynamicSpace += sizeof(int);
    cout << "header numOccurences: " << *(size_t*)dynamicSpace << endl;
    dynamicSpace += sizeof(size_t);
    cout << "header numDocs: " << *(size_t*)dynamicSpace << endl;
    dynamicSpace += sizeof(size_t);
    cout << "header term: " << dynamicSpace << endl;
    dynamicSpace += strlen(dynamicSpace) + 1;
    for(size_t i = 0; i < 16; ++i) {
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
        cout << endl;
    }    
    for(size_t i = 0; i < 16; ++i) {
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
    }

    // TEST WRTIE TO HASHTABLE (1 Entry)
    HashTable< const char *, TermPostingList *> table;
    table.Find("dog", &p);
    HashBlob blob;
    HashBlob *b = blob.Create(&table);
    tuple = b->Find("dog");
    
    cout << "Tuple length: " << tuple->Length << endl;
    cout << "Tuple hash value: " << tuple->HashValue << endl;
    dynamicSpace = tuple->DynamicSpace;
    cout << "Tuple key: " << dynamicSpace << endl;
    dynamicSpace += strlen(dynamicSpace) + 1;
    cout << "Header type: " << (int)dynamicSpace[0] << endl;
    dynamicSpace += sizeof(int);
    cout << "header numOccurences: " << *(size_t*)dynamicSpace << endl;
    dynamicSpace += sizeof(size_t);
    cout << "header numDocs: " << *(size_t*)dynamicSpace << endl;
    dynamicSpace += sizeof(size_t);
    cout << "header term: " << dynamicSpace << endl;
    dynamicSpace += strlen(dynamicSpace) + 1;
    for(size_t i = 0; i < 16; ++i) {
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
    } 
    cout << endl;
    for(size_t i = 0; i < 16; ++i) {
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
    }   
    cout << endl;
    
    for(size_t i = 0; i < 16; ++i) {
        cout << *(size_t *)dynamicSpace << " ";
        dynamicSpace += sizeof(size_t);
    }
    cout << endl;
    tuple = b->Find("dog");

    TermPostingListRaw raw(tuple->DynamicSpace);
    cout << "raw type: " << raw.getType() << endl;
    cout << "raw numOccurence: " << raw.getNumOccurence() << endl;
    cout << "raw numDocs: " << raw.getNumDocuments() << endl;
    cout << "raw term: " << raw.getTerm() << endl;
    cout << "raw postOffset at 14: " << raw.getPostingsListOffsetAt(14) << endl;
    cout << "raw postLocation at 9: " << raw.getPostLocationAt(9) << endl;
    cout << "raw post at 6: " << raw.getPostAt(6).delta << endl;

// TEST INDEX CONSTRUCTOR
    IndexConstructor indexConstructor(10);
    indexConstructor.Insert("catman", Body);
    TermPostingList * pl = indexConstructor.termIndex.Find("hello")->value;
    cout << pl->posts.size() << endl;


/*

    cout << "PostingList bytes required is: " << postingsBytes << endl;
    cout << "Bucket bytes requried is: " << bucketBytes << endl;
    cout << blob.BytesRequired(&table) << endl;
    




    header.term[0] = 'd';
    header.term[1] = 'o';
    header.term[2] = 'g';
    header.term[3] = '\0';
    header.numOfOccurence=100;
    header.numOfDocument = 10;
    header.type = Title;
    header.size = 1024;
    */
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
    
    /*
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
    */
    return 0; 

}

