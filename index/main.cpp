#include "Dictionary.h"
#include "EndDocSerializer.h"
#include "../utility/Common.h"
#include "IndexConstructor.h"
#include "PostingListBlob.h"
#define ASSERT(left,operator,right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; } }

using namespace std;
// returns number of low bits given total count and number of syncs
size_t getNumLowBits2(size_t count, size_t spacing) {
    
    size_t leftShift = ( count + spacing - 1 )/ spacing;
    leftShift--;
    int numLowBits=0;
    // While loop will run until we get n = 0
    while(leftShift)
    {
        numLowBits++;
        leftShift=leftShift>>1;
    }
    return numLowBits;
}

int main (int argc, char *argv[]) 
{
    if(WRITE_TO_DISK) {
        IndexConstructor ic2;
        for(unsigned int j = 0; j < 3; ++ j) {
            ic2.Insert("dog", Body);
        for(unsigned int i = 0; i < 15; ++i) {
            int gap = i;
            while(gap > 0) {
                ic2.Insert("cat", Body);
                gap--;
            }
            
            ic2.Insert("dog", Body);
        }
        ic2.resolveChunkMem();
        }
        ic2.fileManager.GetTermList("dog",1);
        for(unsigned int i =0; i < 3; ++i) {
            TermPostingListRaw rawSync = ic2.fileManager.GetTermList("dog",i);
            size_t indexPos = 0;
            size_t lowBits = getNumLowBits2(121, NUM_SYNC_POINTS);
            ASSERT(rawSync.getHeader()->numOfOccurence, ==, 16);
            size_t loc = seekTermTarget(&rawSync, 0,indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 0);
            ASSERT(indexPos,==,0);
            
            loc = seekTermTarget(&rawSync, 2,indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==,3);
            ASSERT(indexPos,==,2);
            
            loc = seekTermTarget(&rawSync, 10, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 10);
            ASSERT(indexPos,==,4);
            
            loc = seekTermTarget(&rawSync, 11, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 15);
            ASSERT(indexPos,==,5);
            
            loc = seekTermTarget(&rawSync, 55, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 55);
            ASSERT(indexPos,==,10);
            
            loc = seekTermTarget(&rawSync, 91, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 91);
            ASSERT(indexPos,==,13);
            
            loc = seekTermTarget(&rawSync, 104, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 105);
            ASSERT(indexPos,==,14);
            
            loc = seekTermTarget(&rawSync, 120, indexPos, lowBits, NUM_SYNC_POINTS);
            ASSERT(loc,==, 120);
            ASSERT(indexPos,==,15);
            try {
                loc = seekTermTarget(&rawSync, 125, indexPos, lowBits, NUM_SYNC_POINTS);
            }
            catch (const char* e){
                ASSERT(indexPos,==,15);
            }
        }
        ASSERT(ic2.fileManager.getNumDocuments(), ==, 0);
        ASSERT(ic2.fileManager.getNumChunks(), ==, 3);
        ASSERT(ic2.fileManager.getIndexWords(), ==, 363);
        ::vector<Location> v = ic2.fileManager.getChunkEndLocations();
        ASSERT(v.size(), ==, 3);
        ASSERT(v[0],==,121);
        ASSERT(v[1], ==, 242);
        ASSERT(v[2], ==, 363);
        
        
        IndexConstructor ic3;
        for(unsigned int i = 0; i < 3; ++i) {
            for(unsigned int i = 0; i < 5; ++i) {
                for(unsigned int i = 0;  i < 16; ++i) {
                    ic3.Insert("dog", Body);
                }
                ic3.Insert("title", "url");
            }
            ic3.resolveChunkMem();
        }
        
        for(unsigned int i = 0; i < 3; ++i) {
            for (unsigned int j = 0; j < 5; ++j) {
                if(i == 2 && j == 1) {
                    
                }
                size_t docId = i * 5 + j;
                DocumentDetails d = ic3.fileManager.GetDocumentDetails(docId, i);
                cout << "i: " << i << "j: " << j << endl;
                ASSERT(d.title, ==, "title");
                ASSERT(d.url, ==, "url");
                ASSERT(d.numUniqueWords, ==, 1);
                ASSERT(d.lengthOfDocument, ==, 16);
            }
        }
        return 0;
    }
    
    size_t lowBits;
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

    TermPostingList list3(NUM_SYNC_POINTS);
    list3.header.term = "cow";
    list3.header.numOfDocument = 333;
    list3.header.numOfOccurence = 1024;
    list3.header.type = Body;
    //list3.header = lis;
    list3.posts.pushBack(IPostTerm(0));

    for(unsigned int i = 0; i < list3.header.numOfOccurence - 1; ++i) {
        list3.posts.pushBack(IPostTerm(1));
    }
    lowBits = getNumLowBits2(1024, NUM_SYNC_POINTS);

    // Should be 7 lowbits, so we'll have 1024/2^7 == 8 sync points
    createSeekIndex(&list3, 0, lowBits);
    if (NUM_SYNC_POINTS == 8 ) {
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
    }

    
    cout << "TEST: Create Synchronization Index with non-continous deltas" << endl;
    TermPostingList list4(NUM_SYNC_POINTS);
    list4.header.numOfOccurence = 16;
    list4.posts.pushBack(IPostTerm(0));

    for(unsigned int i = 1; i < list4.header.numOfOccurence; ++i) {
        list4.posts.pushBack(IPostTerm(i));
    }
    lowBits = getNumLowBits2(120., NUM_SYNC_POINTS);
    createSeekIndex(&list4, 0, lowBits);
    if(NUM_SYNC_POINTS == 8) {
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
    }

    
    cout << "TEST: Seek using Synchronization Index" << endl;
    size_t indexPos = 0;
    size_t loc;
    // test with basic lsit
    lowBits = getNumLowBits2(1024, NUM_SYNC_POINTS);
    loc = seekTermTarget(&list3, 0, indexPos, lowBits);
    ASSERT(loc,==,0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&list3, 1, indexPos, lowBits);
    ASSERT(loc,==,1);
    ASSERT(indexPos,==,1);
    
    loc = seekTermTarget(&list3, 88, indexPos, lowBits);
    ASSERT(loc,==,88);
    ASSERT(indexPos,==,88);
    
    loc = seekTermTarget(&list3, 512, indexPos, lowBits);
    ASSERT(loc,==,512);
    ASSERT(indexPos,==,512);
    
    loc = seekTermTarget(&list3, 831, indexPos, lowBits);
    ASSERT(loc,==,831);
    ASSERT(indexPos,==,831);
    
    loc = seekTermTarget(&list3, 1023, indexPos, lowBits);
    ASSERT(loc,==,1023);
    ASSERT(indexPos,==,1023);
    try {
        loc = seekTermTarget(&list3, 1024, indexPos, lowBits);
    }
    catch(const char *excp){
        ASSERT(indexPos,==,1023);
    }

    // Test with complex list
    lowBits = getNumLowBits2(120, NUM_SYNC_POINTS);
    loc = seekTermTarget(&list4, 0,indexPos, lowBits);
    ASSERT(loc,==, 0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&list4, 2,indexPos, lowBits);
    ASSERT(loc,==,3);
    ASSERT(indexPos,==,2);
    
    loc = seekTermTarget(&list4, 10, indexPos, lowBits);
    ASSERT(loc,==, 10);
    ASSERT(indexPos,==,4);
    
    loc = seekTermTarget(&list4, 11, indexPos, lowBits);
    ASSERT(loc,==, 15);
    ASSERT(indexPos,==,5);
    
    loc = seekTermTarget(&list4, 55, indexPos, lowBits);
    ASSERT(loc,==, 55);
    ASSERT(indexPos,==,10);
    
    loc = seekTermTarget(&list4, 91, indexPos, lowBits);
    ASSERT(loc,==, 91);
    ASSERT(indexPos,==,13);
    
    loc = seekTermTarget(&list4, 104, indexPos, lowBits);
    ASSERT(loc,==, 105);
    ASSERT(indexPos,==,14);
    
    loc = seekTermTarget(&list4, 120, indexPos, lowBits);
    ASSERT(loc,==, 120);
    ASSERT(indexPos,==,15);
    try {
        loc = seekTermTarget(&list4, 125, indexPos, lowBits);
    }
    catch(const char *excp) {
        ASSERT(indexPos,==,15);
    }

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
    ASSERT(sizeof(*blobHeader), ==, sizeof(int) +  sizeof(w_Occurence) + sizeof(d_Occurence));//+ sizeof("dog"));
    dynamicSpace += sizeof(*blobHeader) + sizeof("dog");
    
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
        size_t bytes = IntBytes((uint8_t *)dynamicSpace);
        size_t delta = UtfToInt((uint8_t *)dynamicSpace);// dynamicSpace
        ASSERT(delta, ==, i);
        dynamicSpace += bytes;
    }
    
    // Caller blob
    HashBlob blob;

    // TEST WRTIE TO HASHTABLE (1 Entry)
    cout << "TEST: Convert to hashblob (1 Entry)" << endl;

    SharedPointer<HashTable< String, TermPostingList *>> tbl1(new HashTable< String , TermPostingList *>());
    tbl1->Find("dog", &list1);
    HashBlob *b1 = blob.Create(tbl1);
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
    ASSERT(sizeof(*chb2), ==, sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence));// + sizeof("dog"));
    dspace2 += sizeof(*chb2) + sizeof("dog");
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
        size_t bytes = IntBytes((uint8_t *)dspace2);
        size_t delta = UtfToInt((uint8_t *)dspace2);// dynamicSpace
        ASSERT(delta, ==, i);
        dspace2 += bytes;
    }
    
    // TEST WRTIE TO HASHTABLE (1 Entry)
    cout << "TEST: Convert to hashblob (2 Entries)" << endl;
    tbl1->Find("camel", &list2);
    HashBlob *b2 = blob.Create(tbl1);
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
    dspace3 = dspace3 + sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence) + sizeof("camel");
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
        size_t bytes = IntBytes((uint8_t *)dspace3);
        size_t delta = UtfToInt((uint8_t *)dspace3);// dynamicSpace
        ASSERT(delta, ==, i);
        dspace3 += bytes;
    }
    
// TEST INDEX CONSTRUCTOR
    
    cout << "TEST: Basic insert" << endl;
    TermPostingList *pl;
    IndexConstructor indexConstructor;
    indexConstructor.Insert("cat", Body);
    pl = indexConstructor.termIndex->Find(String("cat"))->value;
    ASSERT(std::strcmp("cat",pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    ASSERT(pl->posts.size(), ==, 1);
    ASSERT(pl->posts[0].delta, ==, 0);
    //cout << "Inserting same term again" << endl;
    cout << "TEST: Insert multiple inserts, same term" << endl;
    for(unsigned int i = 0; i < 10; ++i) {
        indexConstructor.Insert("dog", Body);
        pl = indexConstructor.termIndex->Find(String("dog"))->value;
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
    IndexConstructor indexConstructor2;
    indexConstructor2.Insert("cat", Body);
    pl = indexConstructor2.termIndex->Find("cat")->value;
    ASSERT(strcmp("cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    indexConstructor2.Insert("dog", Body);
    pl = indexConstructor2.termIndex->Find("dog")->value;
    ASSERT(strcmp("dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 1);
    // inserting dog 10 times
    for(unsigned int i = 0; i < 10; ++i) {
        indexConstructor2.Insert("dog", Body);
        //pl = indexConstructor2.termIndex.Find(String("dog"))->value;
    }
    indexConstructor2.Insert("cat", Body);
    pl = indexConstructor2.termIndex->Find("cat")->value;
    ASSERT(strcmp("cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 2);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 2);
    if(pl->posts.size() == 2){
        // 1 cat + 11 dogs  + 1 cat
        ASSERT(pl->posts[1].delta, ==, 12);
    }
    cout << "TEST: Insert multiple terms" << endl;
    IndexConstructor ic3;
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
    pl = ic3.termIndex->Find(String("Dog"))->value;
    ASSERT(strcmp("Dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    Tuple<String, TermPostingList*> *t = ic3.termIndex->Find(String("Cat"));
    pl = t->value;
    ASSERT(strcmp("Cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex->Find(String("Hamster"))->value;

    ASSERT(strcmp("Hamster", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);

    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex->Find(String("Deer"))->value;

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
    pl = ic3.termIndex->Find(String("Deer"))->value;

    ic3.createSynchronization();
    SyncIndex &deerSync = ic3.termIndex->Find("Deer")->value->syncIndex;
    
    ASSERT(deerSync.postLocation.size(), ==, NUM_SYNC_POINTS);
    ASSERT(deerSync.postingsListOffset.size(), ==, NUM_SYNC_POINTS);
    if(NUM_SYNC_POINTS == 8) {
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
    }

    SyncIndex &dogSync = ic3.termIndex->Find("Dog")->value->syncIndex;
    ASSERT(dogSync.postLocation.size(), ==, NUM_SYNC_POINTS);
    ASSERT(dogSync.postingsListOffset.size(), ==, NUM_SYNC_POINTS);
    if(NUM_SYNC_POINTS == 8){
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
    }

    cout << "TEST: Large hashblob creation and access" << endl;
    HashBlob * lb1 = blob.Create(ic3.termIndex);
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
    
    if (NUM_SYNC_POINTS == 8) {
        cout << "TEST: Basic hashblob seek" << endl;
        for(unsigned int i = 0; i < 5; ++i) {
            ASSERT(raw2.getPostingsListOffsetAt(i),==,i*2);
        }
        
        for(unsigned int i = 0; i < 5; ++i) {
            ASSERT(raw2.getPostLocationAt(i),==,i*8);
        }
        for(unsigned int i = 0; i < 5; ++i) {
            ASSERT(raw2b.getPostingsListOffsetAt(i),==,i*2);
        }
        
        for(unsigned int i = 0; i < 5; ++i) {
            ASSERT(raw2b.getPostLocationAt(i),==,i*8 + 3);
        }
    }


    cout << "TEST: Optimzed index have same values" << endl;
    ic3.termIndex->Optimize();
    pl = ic3.termIndex->Find(String("Dog"))->value;
    ASSERT(strcmp("Dog", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex->Find(String("Cat"))->value;
    ASSERT(strcmp("Cat", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex->Find(String("Hamster"))->value;

    ASSERT(strcmp("Hamster", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);

    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }
    pl = ic3.termIndex->Find(String("Deer"))->value;

    ASSERT(strcmp("Deer", pl->header.term.cstr()), ==, 0);
    ASSERT(pl->header.numOfOccurence, ==, 10);
    ASSERT(pl->header.numOfDocument, ==, 1);
    ASSERT(pl->posts.size(), ==, 10);
    if(pl->posts.size() == 10){
        for(unsigned int i = 1; i < 10; ++i) {
            ASSERT(4, ==, pl->posts[i].delta);
        }
    }

   cout << "TEST: Synchronization table seek from blob" << endl;
   SharedPointer<HashTable<String, TermPostingList*>> syncTableHash1(new HashTable<String, TermPostingList*>());
   syncTableHash1->Find("list3", &list3);
   HashBlob * syncTableHash1Blob = blob.Create(syncTableHash1);
   syncTableHash1Blob->Find("list3");
   TermPostingListRaw rawSyncTable1(syncTableHash1Blob->Find("list3")->DynamicSpace);
   ASSERT(strcmp(rawSyncTable1.getHeader()->term, "cow"), ==, 0);
   ASSERT(rawSyncTable1.getHeader()->numOfDocument, ==, 333);
   ASSERT(rawSyncTable1.getHeader()->numOfOccurence, ==, 1024);
   ASSERT(rawSyncTable1.getHeader()->type, ==, 4);
    lowBits = getNumLowBits2(rawSyncTable1.getHeader()->numOfOccurence, NUM_SYNC_POINTS);
   loc = seekTermTarget(&rawSyncTable1, 0, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,0);
   ASSERT(indexPos,==,0);
   
   loc = seekTermTarget(&rawSyncTable1, 1, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,1);
   ASSERT(indexPos,==,1);
   
   loc = seekTermTarget(&rawSyncTable1, 88, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,88);
   ASSERT(indexPos,==,88);
   
   loc = seekTermTarget(&rawSyncTable1, 512, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,512);
   ASSERT(indexPos,==,512);
   
   loc = seekTermTarget(&rawSyncTable1, 831, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,831);
   ASSERT(indexPos,==,831);
   
   loc = seekTermTarget(&rawSyncTable1, 1023, indexPos, lowBits, NUM_SYNC_POINTS);
   ASSERT(loc,==,1023);
   ASSERT(indexPos,==,1023);
    try {
        loc = seekTermTarget(&rawSyncTable1, 1024, indexPos, lowBits, NUM_SYNC_POINTS);
    }
    catch(const char * excp) {
        ASSERT(indexPos,==,1023);
    }
    
   // complex iterations
    SharedPointer<HashTable<String, TermPostingList*>> syncTableHash2(new HashTable<String, TermPostingList*>());
    syncTableHash2->Find("list4", &list4);
    HashBlob * syncTableHash2Blob = blob.Create(syncTableHash2);

    syncTableHash2Blob->Find("list4");
    TermPostingListRaw rawSyncTable2(syncTableHash2Blob->Find("list4")->DynamicSpace);
    lowBits = getNumLowBits2(120, NUM_SYNC_POINTS);
    ASSERT(rawSyncTable2.getHeader()->numOfOccurence, ==, 16);
    loc = seekTermTarget(&rawSyncTable2, 0,indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 0);
    ASSERT(indexPos,==,0);
    
    loc = seekTermTarget(&rawSyncTable2, 2,indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==,3);
    ASSERT(indexPos,==,2);
    
    loc = seekTermTarget(&rawSyncTable2, 10, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 10);
    ASSERT(indexPos,==,4);
    
    loc = seekTermTarget(&rawSyncTable2, 11, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 15);
    ASSERT(indexPos,==,5);
    
    loc = seekTermTarget(&rawSyncTable2, 55, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 55);
    ASSERT(indexPos,==,10);
    
    loc = seekTermTarget(&rawSyncTable2, 91, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 91);
    ASSERT(indexPos,==,13);
    
    loc = seekTermTarget(&rawSyncTable2, 104, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 105);
    ASSERT(indexPos,==,14);
    
    loc = seekTermTarget(&rawSyncTable2, 120, indexPos, lowBits, NUM_SYNC_POINTS);
    ASSERT(loc,==, 120);
    ASSERT(indexPos,==,15);
    try {
        loc = seekTermTarget(&rawSyncTable2, 125, indexPos, lowBits, NUM_SYNC_POINTS);
    }
    catch (const char* e){
        ASSERT(indexPos,==,15);
    }
    
    cout << "TEST: Write document to blob" << endl;
    char testTitle[] = "title1";
    char testUrl[] = "www.test_url.com";
    //DocumentDetails details1("www.test_url.com", "title1", 20, 100);
    SharedPointer<DocumentDetails> details1(new DocumentDetails("www.test_url.com", "title1", 20, 100));
    DocumentBlob * dblob;
    
    size_t docBytes = dblob->BytesRequired(details1);
    ASSERT(docBytes, ==, 2576);
    //cout << bucketBytes << endl;
    char *docBuffer = (char *)malloc(docBytes);
    dblob->Write(docBuffer, docBuffer + docBytes, details1);
    dblob = (DocumentBlob *)docBuffer;
    ASSERT(strcmp(dblob->title, testTitle), ==, 0);
    ASSERT(strcmp(dblob->URL, testUrl), ==, 0);
    ASSERT(dblob->lengthOfDocument, ==, 20);
    ASSERT(dblob->numUniqueWords,==,100);
    
    cout << "TEST: Multiple Documents, just EndDocList" << endl;
    IndexConstructor ic4;
    ic4.Insert("title1", "www.hello.com");
    ic4.Insert("title2", "www.hello2.com");
    ASSERT(ic4.numberOfDocuments, ==, 2);
    ASSERT(ic4.endDocPostings->posts.size(), ==, 2);
    ASSERT(ic4.endDocPostings->posts[0].delta, ==, 0);
    ASSERT(ic4.endDocPostings->posts[1].delta, ==, 2);
    for(unsigned int i = 0; i < 10; ++i) {
        ic4.Insert("title", "url");
    }
    for(unsigned int i = 1; i < 12; ++i) {
        ASSERT(ic4.endDocPostings->posts[i].delta, ==, 2);
    }
    
    cout << "TEST: Multiple Docs, Single Term" << endl;
    IndexConstructor ic5;
    for(unsigned int i = 0; i < 3; ++i) {
        ic5.Insert("dog", Body);
    }
    ASSERT(ic5.currDocInfo.getNumberOfUniqueWords(), ==, 1);
    ASSERT(ic5.currDocInfo.getNumberOfWords(), ==, 3);
    ic5.Insert("dog_title", "dog.com");
    ASSERT(ic5.endDocPostings->posts[0].delta, ==, 0);
    ASSERT(ic5.docDetails[0]->lengthOfDocument, ==, 3);
    ASSERT(ic5.docDetails[0]->numUniqueWords, ==, 1);
    ASSERT(ic5.endLocation, ==, 5);
    ASSERT(strcmp(ic5.docDetails[0]->title.cstr(), "dog_title"), ==, 0);
    ASSERT(strcmp(ic5.docDetails[0]->url.cstr(), "dog.com"), ==, 0);
    for(unsigned int i = 0; i < 3; ++i) {
        ic5.Insert("dog", Body);
    }
    ASSERT(ic5.currDocInfo.getNumberOfWords(), ==, 3);
    ASSERT(ic5.currDocInfo.getNumberOfUniqueWords(), ==, 1);
    ASSERT(ic5.currDocInfo.getPrevEndLocation(), ==, 3);
    ASSERT(ic5.numberOfWords, ==, 6);
    ASSERT(ic5.numberOfDocuments, ==, 1);
    ASSERT(ic5.numberOfUniqueWords, ==, 1);
    ic5.Insert("doggy", "doggy.com");
    ASSERT(ic5.endLocation, ==, 10)
    ASSERT(ic5.endDocPostings->posts[1].delta, ==, 5);
    ASSERT(ic5.docDetails[1]->lengthOfDocument, ==, 3);
    ASSERT(ic5.docDetails[1]->numUniqueWords, ==, 1);
    ASSERT(strcmp(ic5.docDetails[1]->title.cstr(), "doggy"), ==, 0);
    ASSERT(strcmp(ic5.docDetails[1]->url.cstr(), "doggy.com"), ==, 0);
    // STRUCT: dog dog dog end1 buf dog dog dog end2 buf endPos
    for(unsigned int i = 0; i < 6; ++i) {
        ic5.Insert("cat", Body);
        ic5.Insert("dog", Body);
        ic5.Insert("hamster", Body);
        ic5.Insert("deer", Body);
        ic5.Insert("a", "a.com");
    }
    // STRUCT: dog dog dog end1 buf dog dog dog end2 buf cat dog hamster deer end3 buf ...
    ASSERT(ic5.endDocPostings->posts.size(), ==, 8);
    ASSERT(ic5.docDetails.size(), ==, 8);
    for(unsigned int i = 2; i < ic5.docDetails.size(); ++i) {
        ASSERT(ic5.docDetails[i]->numUniqueWords, ==, 4);
        ASSERT(ic5.docDetails[i]->lengthOfDocument, ==, 4);
        ASSERT(ic5.endDocPostings->posts[i].delta, ==, 6);
    }
    ASSERT(ic5.endDocPostings->header.numOfDocument, ==, 8);
    ASSERT(ic5.numberOfDocuments, ==, 8);
    ASSERT(ic5.numberOfWords, ==, 30);
    ASSERT(ic5.numberOfUniqueWords, ==, 4);

    TermPostingList * mw1 = ic5.termIndex->Find("cat")->value;
    ASSERT(mw1->header.numOfDocument, ==, 6);
    ASSERT(mw1->header.numOfOccurence, ==, 6);
    ASSERT(strcmp(mw1->header.term.cstr(), "cat"), ==, 0);
    
    mw1 = ic5.termIndex->Find("deer")->value;
    ASSERT(mw1->header.numOfDocument, ==, 6);
    ASSERT(mw1->header.numOfOccurence, ==, 6);
    ASSERT(strcmp(mw1->header.term.cstr(), "deer"), ==, 0);
    
    mw1 = ic5.termIndex->Find("hamster")->value;
    ASSERT(mw1->header.numOfDocument, ==, 6);
    ASSERT(mw1->header.numOfOccurence, ==, 6);
    ASSERT(strcmp(mw1->header.term.cstr(), "hamster"), ==, 0);
    
    mw1 = ic5.termIndex->Find("dog")->value;
    ASSERT(mw1->header.numOfDocument, ==, 8);
    ASSERT(mw1->header.numOfOccurence, ==, 12);
    ASSERT(strcmp(mw1->header.term.cstr(), "dog"), ==, 0);
    
    cout << "TEST: Full Index synhroniation" << endl;
    ic5.createSynchronization();
    TermPostingList * dogPostings = ic5.termIndex->Find("dog")->value;
    size_t fisIndex;
    size_t fisLoc;
    lowBits = getNumLowBits2(ic5.endLocation, NUM_SYNC_POINTS);
    // Check dog postings
    fisLoc = seekTermTarget(dogPostings, 0, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 0);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekTermTarget(dogPostings, 1, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 1);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekTermTarget(dogPostings, 2, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 2);
    ASSERT(fisIndex, ==, 2);
    fisLoc = seekTermTarget(dogPostings, 3, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 5);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekTermTarget(dogPostings, 6, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 6);
    ASSERT(fisIndex, ==, 4);
    fisLoc = seekTermTarget(dogPostings, 7, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 7);
    ASSERT(fisIndex, ==, 5);
    fisLoc = seekTermTarget(dogPostings, 9, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 11);
    ASSERT(fisIndex, ==, 6);
    fisLoc = seekTermTarget(dogPostings, 12, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 17);
    ASSERT(fisIndex, ==, 7);
    fisLoc = seekTermTarget(dogPostings, 20, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 23);
    ASSERT(fisIndex, ==, 8);
    fisLoc = seekTermTarget(dogPostings, 29, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 29);
    ASSERT(fisIndex, ==, 9);
    fisLoc = seekTermTarget(dogPostings, 31, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 35);
    ASSERT(fisIndex, ==, 10);
    fisLoc = seekTermTarget(dogPostings, 41, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 41);
    ASSERT(fisIndex, ==, 11);
    try {
        fisLoc = seekTermTarget(dogPostings, 43, fisIndex, lowBits);
    }
    catch (const char * e) {
        ASSERT(fisIndex, ==, 11);
    }
    // Check deer postings
    TermPostingList * deerPostings = ic5.termIndex->Find("deer")->value;
    fisLoc = seekTermTarget(deerPostings, 0, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 13);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekTermTarget(deerPostings, 3, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 13);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekTermTarget(deerPostings, 19, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 19);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekTermTarget(deerPostings, 37, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 37);
    ASSERT(fisIndex, ==, 4);
    fisLoc = seekTermTarget(deerPostings, 43, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 43);
    ASSERT(fisIndex, ==, 5);
    // Check endDoc postings
    SharedPointer<EndDocPostingList> endDoc1 = ic5.endDocPostings;
    fisLoc = seekEndDocTarget(endDoc1, 0, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 3);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekEndDocTarget(endDoc1, 4, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 8);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekEndDocTarget(endDoc1, 5, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 8);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekEndDocTarget(endDoc1, 11, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 14);
    ASSERT(fisIndex, ==, 2);
    fisLoc = seekEndDocTarget(endDoc1, 20, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 20);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekEndDocTarget(endDoc1, 17, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 20);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekEndDocTarget(endDoc1, 26, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 26);
    ASSERT(fisIndex, ==, 4);
    fisLoc = seekEndDocTarget(endDoc1, 39, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 44);
    ASSERT(fisIndex, ==, 7);
    fisLoc = seekEndDocTarget(endDoc1, 44, fisIndex, lowBits);
    ASSERT(fisLoc, ==, 44);
    ASSERT(fisIndex, ==, 7);
    try {
        fisLoc = seekEndDocTarget(endDoc1, 45, fisIndex, lowBits);
    }
    catch (const char* e){
        ASSERT(fisIndex, ==, 7);
    }
    
    cout << "TEST: Term postings list hashblob synchronization" << endl;
    
    HashBlob * sTableBlob = blob.Create(ic5.termIndex);
    sTableBlob->Find("dog");
    TermPostingListRaw rawDogList(sTableBlob->Find("dog")->DynamicSpace);
    // Check dog postings
    fisLoc = seekTermTarget(&rawDogList, 0, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 0);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekTermTarget(&rawDogList, 1, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 1);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekTermTarget(&rawDogList, 2, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 2);
    ASSERT(fisIndex, ==, 2);
    fisLoc = seekTermTarget(&rawDogList, 3, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 5);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekTermTarget(&rawDogList, 6, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 6);
    ASSERT(fisIndex, ==, 4);
    fisLoc = seekTermTarget(&rawDogList, 7, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 7);
    ASSERT(fisIndex, ==, 5);
    fisLoc = seekTermTarget(&rawDogList, 9, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 11);
    ASSERT(fisIndex, ==, 6);
    fisLoc = seekTermTarget(&rawDogList, 12, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 17);
    ASSERT(fisIndex, ==, 7);
    fisLoc = seekTermTarget(&rawDogList, 20, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 23);
    ASSERT(fisIndex, ==, 8);
    fisLoc = seekTermTarget(&rawDogList, 29, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 29);
    ASSERT(fisIndex, ==, 9);
    fisLoc = seekTermTarget(&rawDogList, 31, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 35);
    ASSERT(fisIndex, ==, 10);
    fisLoc = seekTermTarget(&rawDogList, 41, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 41);
    ASSERT(fisIndex, ==, 11);
    try {
        fisLoc = seekTermTarget(&rawDogList, 43, fisIndex, lowBits, NUM_SYNC_POINTS);
    }
    catch (const char* e) {
        ASSERT(fisIndex, ==, 11);
    }

    
    cout << "TEST: EndDoc postings list hashblob synchronization" << endl;
    SerialEndDocs * sBlob;
    SerialEndDocs * endDocBlob = sBlob->Create(endDoc1);
    EndDocPostingListRaw endDoc1Raw(endDocBlob->DynamicSpace);
    ASSERT(endDoc1Raw.getHeader()->numOfDocument, ==, 8);
    // Check endDoc postings
    fisLoc = seekEndDocTarget(&endDoc1Raw, 0, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 3);
    ASSERT(fisIndex, ==, 0);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 4, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 8);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 5, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 8);
    ASSERT(fisIndex, ==, 1);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 11, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 14);
    ASSERT(fisIndex, ==, 2);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 20, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 20);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 17, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 20);
    ASSERT(fisIndex, ==, 3);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 26, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 26);
    ASSERT(fisIndex, ==, 4);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 39, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 44);
    ASSERT(fisIndex, ==, 7);
    fisLoc = seekEndDocTarget(&endDoc1Raw, 44, fisIndex, lowBits, NUM_SYNC_POINTS);
    ASSERT(fisLoc, ==, 44);
    ASSERT(fisIndex, ==, 7);
    try {
        fisLoc = seekEndDocTarget(&endDoc1Raw, 45, fisIndex, lowBits, NUM_SYNC_POINTS);
    }
    catch (const char * e) {
        ASSERT(fisIndex, ==, 7);
    }
     
    

    return 0; 
};

