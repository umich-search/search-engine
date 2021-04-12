#include "Dictionary.h"
#include "EndDocSerializer.h"
#include "IndexConstructor.h"
#include "PostingListBlob.h"
#include <filesystem>

#define ASSERT(left, operator, right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; } }

using namespace std;

// returns number of low bits given total count and number of syncs
size_t getNumLowBits2(size_t count, size_t spacing) {

    size_t leftShift = (count + NUM_SYNC_POINTS - 1) / NUM_SYNC_POINTS;
    leftShift--;
    int numLowBits = 0;
    // While loop will run until we get n = 0
    while (leftShift) {
        numLowBits++;
        leftShift = leftShift >> 1;
    }
    return numLowBits;
}

int main(int argc, char *argv[]) {
    std::__fs::filesystem::remove_all(CHUNK_DIRECTORY);
    std::__fs::filesystem::create_directory(CHUNK_DIRECTORY);

    IndexConstructor ic;
    for (unsigned int i = 0; i < 30; ++i) {
        for (unsigned int j = 0; j < 100; ++j) {
            ic.Insert("cat", Body);
            ic.Insert("dog", Body);
        }
        ic.Insert("cat_title", "cat.com");
    }
    ic.FinishConstruction();
    FileManager manager = ic.fileManager;
    Dictionary dict(manager,0);
    char word[] = {'c', 'a', 't', '\0'};
    ISRWord isrword = *dict.OpenISRWord(word);

    ASSERT(isrword.GetDocumentCount(), ==, 30);
    ASSERT(isrword.GetNumberOfOccurrences(), ==, 3000);
    ASSERT(isrword.GetStartLocation(), ==, 0);
    ASSERT(isrword.GetEndLocation(), ==, 5998 + 2 * 29);
    ASSERT(isrword.Seek(0)->GetStartLocation(), ==, 0);
    ASSERT(isrword.Seek(1)->GetStartLocation(), ==, 2);
    ASSERT(isrword.Seek(2)->GetStartLocation(), ==, 2);
    ASSERT(isrword.NextEndDoc()->GetStartLocation(), ==, 200);
    ASSERT(isrword.Seek(5998 + 2 * 29 - 1)->GetStartLocation(), ==, 5998 + 2 * 29);
    ASSERT(isrword.Seek(5998 + 2 * 29)->GetStartLocation(), ==, 5998 + 2 * 29);
    ASSERT(isrword.Seek(0)->GetStartLocation(), ==, 0);
    for (unsigned int i = 0; i < 2999; i++) {
        isrword.Next();
    }
    ASSERT(isrword.GetCurrentPost()->GetStartLocation(), ==, 5998 + 2 * 29);
    ASSERT(isrword.NextEndDoc()->GetStartLocation(), ==, 5998 + 2 * 29 + 2);
//    for (int i=1;i<3000;i++){
//        ASSERT(isrword.Next()->GetLocation(),==,2*i);
//    }

    char word2[] = {'d', 'o', 'g', '\0'};
    ISRWord isrword2 = *dict.OpenISRWord(word2);
    ASSERT(isrword2.GetDocumentCount(), ==, 30);
    ASSERT(isrword2.GetNumberOfOccurrences(), ==, 3000);
    ASSERT(isrword2.GetStartLocation(), ==, 1);
    ASSERT(isrword2.GetEndLocation(), ==, 5999 + 2 * 29);

    ISREndDoc isrdoc = *dict.OpenISREndDoc();
    ASSERT(isrdoc.GetStartLocation(), ==, 200);
    ASSERT(isrdoc.GetEndLocation(), ==, 6058);
    ASSERT(isrdoc.Seek(0)->GetStartLocation(), ==, 200);
    ASSERT(isrdoc.Seek(200)->GetStartLocation(), ==, 200);
    ASSERT(isrdoc.Seek(201)->GetStartLocation(), ==, 402);
    ASSERT(isrdoc.Seek(402)->GetStartLocation(), ==, 402);
    ASSERT(isrdoc.Seek(604)->GetStartLocation(), ==, 604);

    ASSERT(isrdoc.Seek(0)->GetStartLocation(), ==, 200);
    for (unsigned int i = 1; i < 30; i++) {
        isrdoc.Next();
        ASSERT(isrdoc.GetCurrentPost()->GetStartLocation(),==,202*i+200);
    }
    char word3[] = {'d', 'o','\0'};
    ISRWord isrword3 = *dict.OpenISRWord(word3);
    ASSERT(isrword3.GetDocumentCount(), ==, 0);
//    ASSERT(isrword3.GetNumberOfOccurrences(), ==, 0);
//    ASSERT(isrword3.GetStartLocation(), ==, -1);
    cout<<"hhhhhh"<<endl;
    /*
        Three chunks of 50
        seek(150);
        find manager.EndLocation() first oen that is greater than or equal to 150 = i;
        seekTarget(150 = manager.endLocs[i]);
        return -1 when no term appear after the location in this chunk
        seek function: return the location after the seeking point;

        If you want to find postAt(150); Total index has 150 occurences of term, each chunk has 50 terms;
        getPostAt(150 - 50*2); for this chunk

        IPostTerm getPostAt(Offset i); 0 first term 1 second term

        IPostTerm getPostAt(Offset i, Offset &byteAfterPost);
        IPostTerm getPostAt(Offset i, Offset numBytes);
        IPostTerm getPostAtByte(Offset numBytes, Offset &byteAfterPost);

        make sure that it is not out of range

        header numOccurence;
        getPostAt(numOccurenc-1)
    */
    return 0;
}