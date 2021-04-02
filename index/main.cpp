//#include <iostream>
//#include "ISR.h"
//#include "DictionarySerializer.h"
//#include "IPost.h"
//#include "../utility/HashTable.h"
#include "../utility/Common.h"
#include "Dictionary.h"
#include "IndexConstructor.h"
using namespace std;
#include <assert.h>     /* assert */
#define ASSERT(left,operator,right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; } }

int main (int argc, char *argv[]) 
{ 
    /*
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
*/
// TEST INDEX CONSTRUCTOR
    cout << "TEST: Basic Insert" << endl;
    TermPostingList *pl;
    IndexConstructor indexConstructor(10);
    indexConstructor.Insert("cat", Body);
    pl = indexConstructor.termIndex.Find(String("cat"))->value;
    ASSERT(std::strcmp("cat",pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    ASSERT(pl->posts.size(), ==, 1);
    ASSERT(pl->posts[0].delta, ==, 0);
    //cout << "Inserting same term again" << endl;
    cout << "TEST: Insert Multiple inserts same term" << endl;
    for(unsigned int i = 0; i < 10; ++i) {
        indexConstructor.Insert("dog", Body);
        pl = indexConstructor.termIndex.Find(String("dog"))->value;
        ASSERT(strcmp("dog",pl->header.term.cstr()), ==, 0);
        ASSERT(pl->header.numOfDocument, ==, 1);
        ASSERT(pl->header.numOfOccurence, ==, i + 1);
        ASSERT(pl->posts.size(), ==, i + 1);
        if(i == 0) {
            ASSERT(pl->posts[i].delta, ==, 0);
        }
        else {
            ASSERT(pl->posts[i].delta, ==, 1);
        }
    }
    cout << "TEST: Insert Two Seperate Terms" << endl;
    IndexConstructor indexConstructor2(10);
    indexConstructor2.Insert("cat", Body);
    pl = indexConstructor2.termIndex.Find("cat")->value;
    ASSERT(strcmp("cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    indexConstructor2.Insert("dog", Body);
    pl = indexConstructor2.termIndex.Find("dog")->value;
    ASSERT(strcmp("dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    // inserting dog 10 times
    for(unsigned int i = 0; i < 10; ++i) {
        indexConstructor2.Insert("dog", Body);
        //pl = indexConstructor2.termIndex.Find(String("dog"))->value;
    }
    indexConstructor2.Insert("cat", Body);
    pl = indexConstructor2.termIndex.Find("cat")->value;
    cout << pl->header.term << endl;
    ASSERT(strcmp("cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 2);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 2);
    if(pl->posts.size() == 2){
        // 1 cat + 11 dogs  + 1 cat
        ASSERT(pl->posts[1].delta, ==, 12);
    }
    cout << "TEST INSERT MULTIPLE TERMS" << endl;
    IndexConstructor ic3(10);
    for(unsigned int i = 0; i < 40; ++i) {
        if(i % 4 == 0) {
            ic3.Insert("Dog", Body);
        } else if (i % 4 == 1) {
            ic3.Insert("Cat", Body);
        } else if (i % 4 == 2) {
            ic3.Insert("Hamster", Body);
        } else if ( i % 4 == 3 ) {
            ic3.Insert("Deer", Body);
        }
    }
    pl = ic3.termIndex.Find(String("Dog"))->value;
    cout << pl->header.term << endl;
    ASSERT(strcmp("Dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    Tuple<String, TermPostingList*> *t = ic3.termIndex.Find(String("Cat"));
    if(!t) {
        cout << "T is nullptr";
    }
    else {
        cout << t->key.cstr() << " THIS IS THE KEY TERM " << endl;
    }
    pl = t->value;//ic3.termIndex.Find(String("Cat"))->value;
    cout << "TEST: Checking between " << "Cat" << " and " << pl->header.term << endl;
    cout  << pl->header.numOfOccurence << endl;



    cout << pl->header.term << endl;
    ASSERT(strcmp("Cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex.Find(String("Hamster"))->value;
    cout << pl->header.term << endl;

    ASSERT(strcmp("Hamster", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
        cout << pl->header.term << endl;

    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex.Find(String("Deer"))->value;
    cout << pl->header.term << endl;

    ASSERT(strcmp("Deer", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    /*
    indexConstructor2.Insert("hamster", Body);
    pl = indexConstructor2.termIndex.Find("hamster")->value;
    ASSERT(strcmp("hamster", pl->header.term), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 2);
    if(pl->posts.size() == 2){
        ASSERT(pl->posts[1].delta, ==, 0);
    }
    */
    return 0; 
}

