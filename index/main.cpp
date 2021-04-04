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
    
    // CREATE POSTING LIST
    CommonHeader header1;
    SyncIndex syncIndex1;
    TermPostingList list1;
    header1.term = "dog";
    header1.numOfDocument = 111;
    header1.numOfOccurence = 16;
    header1.type = Title;

    for(unsigned int i = 0; i < 16; ++i) {
        syncIndex1.postingsListOffset.pushBack(i);
        syncIndex1.postLocation.pushBack(i);
        list1.posts.pushBack(IPostTerm(i));
    }
    list1.header = header1;
    list1.syncIndex = syncIndex1;
    // CREATE POSTING LIST
    CommonHeader header2;
    SyncIndex syncIndex2;
    TermPostingList list2;
    header2.term = "camel";
    header2.numOfDocument = 222;
    header2.numOfOccurence = 26;
    header2.type = Body;
    
    for(unsigned int i = 0; i < 26; ++i) {
        syncIndex2.postingsListOffset.pushBack(i);
        syncIndex2.postLocation.pushBack(i);
        list2.posts.pushBack(IPostTerm(i));
    }
    list2.header = header2;
    list2.syncIndex = syncIndex2;

    cout << "TEST: Create TermPostingList Synchronization Index" << endl;
    // CREATE POSTING LIST
    //CommonHeader header3;

    TermPostingList list3(8);
    list3.header.term = "cow";
    list3.header.numOfDocument = 333;
    list3.header.numOfOccurence = 1024;
    list3.header.type = Body;
    //list3.header = lis;
    list3.posts.pushBack(IPostTerm(0));

    for(unsigned int i = 0; i < list3.header.numOfOccurence - 1; ++i) {
        list3.posts.pushBack(IPostTerm(1));
    }
    // Should be 7 lowbits, so we'll have 1024/2^7 == 8 sync points
    createSeekIndex(&list3, 0, 7);
    
    ASSERT(list3.syncIndex.postingsListOffset.size(), ==, 8);
    ASSERT(list3.syncIndex.postLocation.size(), ==, 8);
    ASSERT(list3.syncIndex.postingsListOffset[0], ==, 0);
    ASSERT(list3.syncIndex.postingsListOffset[1], ==, 128);
    ASSERT(list3.syncIndex.postingsListOffset[2], ==, 256);
    ASSERT(list3.syncIndex.postingsListOffset[3], ==, 384);
    ASSERT(list3.syncIndex.postingsListOffset[5], ==, 640);
    ASSERT(list3.syncIndex.postingsListOffset[7], ==, 896);
    ASSERT(list3.syncIndex.postLocation[0], ==, 0);
    ASSERT(list3.syncIndex.postLocation[1], ==, 128);
    ASSERT(list3.syncIndex.postLocation[2], ==, 256);
    ASSERT(list3.syncIndex.postLocation[3], ==, 384);
    ASSERT(list3.syncIndex.postLocation[5], ==, 640);
    ASSERT(list3.syncIndex.postLocation[7], ==, 896);
    
    cout << "TEST: Create Synchronization Index with non-continous deltas" << endl;
    TermPostingList list4(8);
    list4.header.numOfOccurence = 16;
    list4.posts.pushBack(IPostTerm(0));

    for(unsigned int i = 1; i < list4.header.numOfOccurence; ++i) {
        list4.posts.pushBack(IPostTerm(i));
    }
    createSeekIndex(&list4, 0, 4);

    ASSERT(list4.syncIndex.postingsListOffset.size(), ==, 8);
    ASSERT(list4.syncIndex.postLocation.size(), ==, 8);
    ASSERT(list4.syncIndex.postingsListOffset[0], ==, 0);
    ASSERT(list4.syncIndex.postingsListOffset[1], ==, 6);
    ASSERT(list4.syncIndex.postingsListOffset[2], ==, 8);
    ASSERT(list4.syncIndex.postingsListOffset[3], ==, 10);
    ASSERT(list4.syncIndex.postingsListOffset[4], ==, 11);
    ASSERT(list4.syncIndex.postingsListOffset[5], ==, 13);
    ASSERT(list4.syncIndex.postingsListOffset[6], ==, 14);
    ASSERT(list4.syncIndex.postingsListOffset[7], ==, 15);
    ASSERT(list4.syncIndex.postLocation[0], ==, 0);
    ASSERT(list4.syncIndex.postLocation[1], ==, 21);
    ASSERT(list4.syncIndex.postLocation[2], ==, 36);
    ASSERT(list4.syncIndex.postLocation[3], ==, 55);
    ASSERT(list4.syncIndex.postLocation[4], ==, 66);
    ASSERT(list4.syncIndex.postLocation[5], ==, 91);
    ASSERT(list4.syncIndex.postLocation[6], ==, 105);
    ASSERT(list4.syncIndex.postLocation[7], ==, 120);
    
    cout << "TEST: Seek using Synchronization Index" << endl;
    size_t indexPos = 0;
    size_t loc;
    // test with basic lsit
    loc = seekTermTarget(&list3, 0, indexPos, 7);
    ASSERT(loc,==,0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&list3, 1, indexPos, 7);
    ASSERT(loc,==,1);
    ASSERT(indexPos,==,1);
    
    loc = seekTermTarget(&list3, 88, indexPos, 7);
    ASSERT(loc,==,88);
    ASSERT(indexPos,==,88);
    
    loc = seekTermTarget(&list3, 512, indexPos, 7);
    ASSERT(loc,==,512);
    ASSERT(indexPos,==,512);
    
    loc = seekTermTarget(&list3, 831, indexPos, 7);
    ASSERT(loc,==,831);
    ASSERT(indexPos,==,831);
    
    loc = seekTermTarget(&list3, 1023, indexPos, 7);
    ASSERT(loc,==,1023);
    ASSERT(indexPos,==,1023);
    
    loc = seekTermTarget(&list3, 1024, indexPos, 7);
    ASSERT(loc,==, -1);
    ASSERT(indexPos,==,1023);
    // Test with complex list
    loc = seekTermTarget(&list4, 0,indexPos, 4);
    ASSERT(loc,==, 0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&list4, 2,indexPos, 4);
    ASSERT(loc,==,3);
    ASSERT(indexPos,==,2);
    
    loc = seekTermTarget(&list4, 10, indexPos, 4);
    ASSERT(loc,==, 10);
    ASSERT(indexPos,==,4);
    
    loc = seekTermTarget(&list4, 11, indexPos, 4);
    ASSERT(loc,==, 15);
    ASSERT(indexPos,==,5);
    
    loc = seekTermTarget(&list4, 55, indexPos, 4);
    ASSERT(loc,==, 55);
    ASSERT(indexPos,==,10);
    
    loc = seekTermTarget(&list4, 91, indexPos, 4);
    ASSERT(loc,==, 91);
    ASSERT(indexPos,==,13);
    
    loc = seekTermTarget(&list4, 104, indexPos, 4);
    ASSERT(loc,==, 105);
    ASSERT(indexPos,==,14);
    
    loc = seekTermTarget(&list4, 120, indexPos, 4);
    ASSERT(loc,==, 120);
    ASSERT(indexPos,==,15);
    
    loc = seekTermTarget(&list4, 125, indexPos, 4);
    ASSERT(loc,==, -1);
    ASSERT(indexPos,==,15);

    cout << "TEST: Synchronziation Index for EndDocList" << endl;


    // CREATE SINGLE BUCKET
    uint32_t hashValue1 = fnvHash(list1.header.term.cstr(), list1.header.term.size() + 1);
    Bucket<String, TermPostingList *> testBucket(list1.header.term, hashValue1, &list1);
    
    
    // ALLOCATE BUFFER
    SerialTuple s;
    size_t bucketBytes = s.BytesRequired(&testBucket);
    //cout << bucketBytes << endl;
    char *buffer = (char *)malloc(bucketBytes);
    
    // TEST WRITE BUCKET TO BUFFER
    cout << "TEST: Write single bucket to buffer" << endl;
    s.Write(buffer, buffer + bucketBytes, &testBucket);
    
    const SerialTuple *tuple = (SerialTuple *) buffer;
    //cout << "Tuple length: " << tuple->Length << endl;
    
    //size_t listSize = tuple->Length;
    ASSERT(tuple->HashValue, ==, hashValue1);
    const char * dynamicSpace = tuple->DynamicSpace;
    ASSERT(strcmp("dog", dynamicSpace), ==, 0);
    dynamicSpace += strlen(dynamicSpace) + 1;
    //CommonHeader * ch = (CommonHeader *)dynamicSpace;
    
    CommonHeaderBlob * blobHeader = (CommonHeaderBlob *)dynamicSpace;
    ASSERT(blobHeader->type,==,3);
    ASSERT(blobHeader->numOfOccurence,==,16);
    ASSERT(blobHeader->numOfDocument,==,111);
    ASSERT(strcmp(blobHeader->term, "dog"), ==, 0);
    ASSERT(sizeof(*blobHeader), ==, sizeof(int) + 2 * sizeof(size_t) + sizeof("dog"));
    dynamicSpace += sizeof(*blobHeader);
    
    // Assert postingsListsOffsets
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dynamicSpace, ==, i);
        dynamicSpace += sizeof(size_t);
    }
    // Assert postsOffset
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dynamicSpace, ==, i);
        dynamicSpace += sizeof(size_t);
    }
    // Assert post deltas
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dynamicSpace, ==, i);
        dynamicSpace += sizeof(size_t);
    }
    
    // Caller blob
    HashBlob blob;

    // TEST WRTIE TO HASHTABLE (1 Entry)
    cout << "TEST: Convert to hashblob (1 Entry)" << endl;
    HashTable< String, TermPostingList *> tbl1;
    tbl1.Find("dog", &list1);
    HashBlob *b1 = blob.Create(&tbl1);
    tuple = b1->Find("dog");
    
    ASSERT(tuple, !=, nullptr);
    ASSERT(tuple->HashValue, ==, fnvHash("dog", strlen("dog")));
    const char* dspace2 = tuple->DynamicSpace;
    ASSERT(strcmp("dog", dspace2), ==, 0);
    dspace2 += sizeof("dog");
    CommonHeaderBlob *chb2 = (CommonHeaderBlob *)dspace2;
    ASSERT(chb2->numOfDocument, ==, 111);
    ASSERT(chb2->numOfOccurence, ==, 16);
    ASSERT(strcmp(chb2->term,"dog"), ==, 0);
    ASSERT(chb2->type,==,3);
    ASSERT(sizeof(*chb2), ==, sizeof(int) + 2 * sizeof(size_t) + sizeof("dog"));
    dspace2 += sizeof(*chb2);
    // Assert postingsListsOffsets
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dspace2, ==, i);
        dspace2 += sizeof(size_t);
    }
    // Assert postsOffset
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dspace2, ==, i);
        dspace2 += sizeof(size_t);
    }
    // Assert post deltas
    for(size_t i = 0; i < 16; ++i) {
        ASSERT(*(size_t *)dspace2, ==, i);
        dspace2 += sizeof(size_t);
    }
    
    // TEST WRTIE TO HASHTABLE (1 Entry)
    // TODO: Can probably consolidate test variables
    cout << "TEST: Convert to hashblob (2 Entries)" << endl;
    tbl1.Find("camel", &list2);
    HashBlob *b2 = blob.Create(&tbl1);
    tuple = b2->Find("camel");
    ASSERT(tuple, !=, nullptr);
    ASSERT(tuple->HashValue, ==, fnvHash("camel", strlen("camel")));
    const char* dspace3 = tuple->DynamicSpace;
    ASSERT(strcmp("camel", dspace3), ==, 0);
    dspace3 += sizeof("camel");
    CommonHeaderBlob *chb3 = (CommonHeaderBlob *)dspace3;
    ASSERT(chb3->numOfDocument, ==, 222);
    ASSERT(chb3->numOfOccurence, ==, 26);
    ASSERT(strcmp(chb3->term,"camel"), ==, 0);
    ASSERT(chb3->type,==,4);
    //size_t term = sizeof(chb3->term);
    dspace3 = dspace3 + sizeof(int) + 2 * sizeof(size_t) + sizeof("camel");
    // Assert postingsListsOffsets
    for(size_t i = 0; i < 26; ++i) {
        ASSERT(*(size_t *)dspace3, ==, i);
        dspace3 += sizeof(size_t);
    }
    // Assert postsOffset
    for(size_t i = 0; i < 26; ++i) {
        ASSERT(*(size_t *)dspace3, ==, i);
        dspace3 += sizeof(size_t);
    }
    // Assert post deltas
    for(size_t i = 0; i < 26; ++i) {
        ASSERT(*(size_t *)dspace3, ==, i);
        dspace3 += sizeof(size_t);
    }/*
    
    cout << "TEST: Blob data wrapper functions" << endl;
    TermPostingListRaw raw1(tuple->DynamicSpace);
    ASSERT(raw1.getHeader()->numOfDocument, ==, 222);
    ASSERT(raw1.getHeader()->numOfOccurence, ==, 26);
    ASSERT(strcmp(raw1.getHeader()->term, "camel"), ==, 0);
    ASSERT(raw1.getHeader()->type, ==, 4);
    
    for(unsigned int i = 0; i < 26; ++i) {
        ASSERT(raw1.getPostingsListOffsetAt(i),==,i);
        ASSERT(raw1.getPostAt(i).delta,==,i);
        ASSERT(raw1.getPostLocationAt(i),==,i);
    }
*/
// TEST INDEX CONSTRUCTOR
    
    cout << "TEST: Basic insert" << endl;
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
    cout << "TEST: Insert multiple inserts, same term" << endl;
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
    cout << "TEST: Insert two seperate terms" << endl;
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
    ASSERT(strcmp("cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 2);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 2);
    if(pl->posts.size() == 2){
        // 1 cat + 11 dogs  + 1 cat
        ASSERT(pl->posts[1].delta, ==, 12);
    }
    cout << "TEST: Insert multiple terms" << endl;
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
    pl = t->value;//ic3.termIndex.Find(String("Cat"))->value;
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

    ASSERT(strcmp("Hamster", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);

    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex.Find(String("Deer"))->value;

    ASSERT(strcmp("Deer", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }

    cout << "TEST: Create synchronization for hashtable" << endl;
    pl = ic3.termIndex.Find(String("Deer"))->value;

    ic3.createSynchronization();
    SyncIndex &deerSync = ic3.termIndex.Find("Deer")->value->syncIndex;
    ASSERT(deerSync.postLocation.size(), ==, 8);
    ASSERT(deerSync.postingsListOffset.size(), ==, 8);
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,i*2);
    }
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,-1);
    }
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(deerSync.postLocation[i],==,i*8 + 3);
    }
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postLocation[i],==,-1);
    }
    
    SyncIndex &dogSync = ic3.termIndex.Find("Dog")->value->syncIndex;
    ASSERT(dogSync.postLocation.size(), ==, 8);
    ASSERT(dogSync.postingsListOffset.size(), ==, 8);
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(dogSync.postingsListOffset[i],==,i*2);
    }
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(dogSync.postingsListOffset[i],==,-1);
    }
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(dogSync.postLocation[i],==,i*8);
    }
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(dogSync.postLocation[i],==,-1);
    }

    cout << "TEST: Large hashblob creation and access" << endl;
    HashBlob * lb1 = blob.Create(&ic3.termIndex);
    tuple = lb1->Find("Dog");
    TermPostingListRaw raw2(tuple->DynamicSpace);
    ASSERT(strcmp(raw2.getHeader()->term, "Dog"),==, 0);
    ASSERT(raw2.getHeader()->numOfOccurence, ==, 10);
    ASSERT(raw2.getHeader()->numOfDocument, ==, 1);
    ASSERT(raw2.getHeader()->type, ==, 4);
    
    tuple = lb1->Find("Deer");
    TermPostingListRaw raw2b(tuple->DynamicSpace);
    ASSERT(strcmp(raw2b.getHeader()->term, "Deer"),==, 0);
    ASSERT(raw2b.getHeader()->numOfOccurence, ==, 10);
    ASSERT(raw2b.getHeader()->numOfDocument, ==, 1);
    ASSERT(raw2b.getHeader()->type, ==, 4);
    
    tuple = lb1->Find("Cat");
    TermPostingListRaw raw2c(tuple->DynamicSpace);
    ASSERT(strcmp(raw2c.getHeader()->term, "Cat"),==, 0);
    ASSERT(raw2c.getHeader()->numOfOccurence, ==, 10);
    ASSERT(raw2c.getHeader()->numOfDocument, ==, 1);
    ASSERT(raw2c.getHeader()->type, ==, 4);
    
    tuple = lb1->Find("Hamster");
    TermPostingListRaw raw2d(tuple->DynamicSpace);
    ASSERT(strcmp(raw2d.getHeader()->term, "Hamster"),==, 0);
    ASSERT(raw2d.getHeader()->numOfOccurence, ==, 10);
    ASSERT(raw2d.getHeader()->numOfDocument, ==, 1);
    ASSERT(raw2d.getHeader()->type, ==, 4);
    
    cout << "TEST: Basic hashblob seek" << endl;
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(raw2.getPostingsListOffsetAt(i),==,i*2);
    }
    /* TODO: Need to convert stuff to int64 or int32
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,-1);
    }
     */
    
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(raw2.getPostLocationAt(i),==,i*8);
    }
    /*
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,-1);
    }
     */
    
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(raw2b.getPostingsListOffsetAt(i),==,i*2);
    }
    /* TODO: Need to convert stuff to int64 or int32
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,-1);
    }
     */
    
    for(unsigned int i = 0; i < 5; ++i) {
        ASSERT(raw2b.getPostLocationAt(i),==,i*8 + 3);
    }
    /*
    for(unsigned int i = 5; i < deerSync.postingsListOffset.size(); ++i) {
        ASSERT(deerSync.postingsListOffset[i],==,-1);
    }
     */

    cout << "TEST: Optimzed index have same values" << endl;
    ic3.termIndex.Optimize();
    pl = ic3.termIndex.Find(String("Dog"))->value;
    ASSERT(strcmp("Dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex.Find(String("Cat"))->value;
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

    ASSERT(strcmp("Hamster", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);

    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex.Find(String("Deer"))->value;

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
    // TODO: Creation of sync index with EndDoc
    cout << "TEST: Synchronization table creation" << endl;

*/


   cout << "TEST: Synchronization table seek from blob" << endl;
   HashTable<String, TermPostingList*> syncTableHash1;
   syncTableHash1.Find("list3", &list3);
   HashBlob * syncTableHash1Blob = blob.Create(&syncTableHash1);

   syncTableHash1Blob->Find("list3");
   TermPostingListRaw rawSyncTable1(syncTableHash1Blob->Find("list3")->DynamicSpace);
   ASSERT(strcmp(rawSyncTable1.getHeader()->term, "cow"), ==, 0);
   ASSERT(rawSyncTable1.getHeader()->numOfDocument, ==, 333);
   ASSERT(rawSyncTable1.getHeader()->numOfOccurence, ==, 1024);
   ASSERT(rawSyncTable1.getHeader()->type, ==, 4);

   loc = seekTermTarget(&rawSyncTable1, 0, indexPos, 7, 8);
   ASSERT(loc,==,0);
   ASSERT(indexPos,==,0);
   
   loc = seekTermTarget(&rawSyncTable1, 1, indexPos, 7, 8);
   ASSERT(loc,==,1);
   ASSERT(indexPos,==,1);
   
   loc = seekTermTarget(&rawSyncTable1, 88, indexPos, 7, 8);
   ASSERT(loc,==,88);
   ASSERT(indexPos,==,88);
   
   loc = seekTermTarget(&rawSyncTable1, 512, indexPos, 7, 8);
   ASSERT(loc,==,512);
   ASSERT(indexPos,==,512);
   
   loc = seekTermTarget(&rawSyncTable1, 831, indexPos, 7, 8);
   ASSERT(loc,==,831);
   ASSERT(indexPos,==,831);
   
   loc = seekTermTarget(&rawSyncTable1, 1023, indexPos, 7, 8);
   ASSERT(loc,==,1023);
   ASSERT(indexPos,==,1023);
   
   loc = seekTermTarget(&rawSyncTable1, 1024, indexPos, 7, 8);
   ASSERT(loc,==, -1);
   ASSERT(indexPos,==,1023);
    
   // complex iterations
    HashTable<String, TermPostingList*> syncTableHash2;
    syncTableHash2.Find("list4", &list4);
    HashBlob * syncTableHash2Blob = blob.Create(&syncTableHash2);

    syncTableHash2Blob->Find("list4");
    TermPostingListRaw rawSyncTable2(syncTableHash2Blob->Find("list4")->DynamicSpace);
    ASSERT(rawSyncTable2.getHeader()->numOfOccurence, ==, 16);
    loc = seekTermTarget(&rawSyncTable2, 0,indexPos, 4, 8);
    ASSERT(loc,==, 0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&rawSyncTable2, 2,indexPos, 4, 8);
    ASSERT(loc,==,3);
    ASSERT(indexPos,==,2);
    
    loc = seekTermTarget(&rawSyncTable2, 10, indexPos, 4, 8);
    ASSERT(loc,==, 10);
    ASSERT(indexPos,==,4);
    
    loc = seekTermTarget(&rawSyncTable2, 11, indexPos, 4, 8);
    ASSERT(loc,==, 15);
    ASSERT(indexPos,==,5);
    
    loc = seekTermTarget(&rawSyncTable2, 55, indexPos, 4, 8);
    ASSERT(loc,==, 55);
    ASSERT(indexPos,==,10);
    
    loc = seekTermTarget(&rawSyncTable2, 91, indexPos, 4, 8);
    ASSERT(loc,==, 91);
    ASSERT(indexPos,==,13);
    
    loc = seekTermTarget(&rawSyncTable2, 104, indexPos, 4, 8);
    ASSERT(loc,==, 105);
    ASSERT(indexPos,==,14);
    
    loc = seekTermTarget(&rawSyncTable2, 120, indexPos, 4, 8);
    ASSERT(loc,==, 120);
    ASSERT(indexPos,==,15);
    
    loc = seekTermTarget(&rawSyncTable2, 125, indexPos, 4, 8);
    ASSERT(loc,==, -1);
    ASSERT(indexPos,==,15);

    return 0; 
}

