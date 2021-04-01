#include "ISR.h"
#include "PostingList.h"
#include "stdlib.h"

// ISRWord Functions

Post* ISRWord::Next () {
    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex+1].delta;
    currentPostingsIndex++;
    currentPost = Post(indexLocation);
    return &currentPost;    
}

Post* ISRWord::NextEndDoc() {
    return nullptr;
}

Post* ISRWord::Seek( Location target){
   size_t indexLocation = 0;//seekTermTarget(&postingList, target, currentPostingsIndex);
   // term after target not found;
   if(indexLocation == -1) {
       return nullptr;
   }
    currentPost = Post(indexLocation);
   return &currentPost;
}

Location ISRWord::GetStartLocation( ){
    size_t throwaway;
    //return seekTermTarget(&postingList, 0, throwaway);
    return -1;
}
// TODO
Location ISRWord::GetEndLocation( ){
    return -1;
}

unsigned ISRWord::GetDocumentCount( ){
    return postingList.header.numOfDocument;
}

unsigned ISRWord::GetNumberOfOccurrences( ){
    return postingList.header.numOfOccurence;
}

Post* ISRWord::GetCurrentPost( ){
    return &currentPost;
}

// ISREndDoc Functions

Post* ISREndDoc::Next () {
    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex+1].delta;
    currentPostingsIndex++;
    currentPost = Post(indexLocation);
    return &currentPost;   
}

Post* ISREndDoc::NextEndDoc() {
    size_t indexLocation = currentPost.GetEndLocation() + postingList.posts[currentPostingsIndex+1].delta;
    currentPostingsIndex++;
    currentPost = Post(indexLocation);
    return &currentPost;   
}

Post* ISREndDoc::Seek( Location target){
   //size_t indexLocation = seekEndDocTarget(&postingList, target, currentPostingsIndex);
   // term after target not found;
   /*
   if(indexLocation == -1) {
       return nullptr;
   }
   */
    currentPost = Post(2);//Post(indexLocation);
   return &currentPost;
}

Location ISREndDoc::GetStartLocation( ){
    size_t throwaway;
    //return seekEndDocTarget(&postingList, 0, throwaway);
    return -1;
}

Location ISREndDoc::GetEndLocation( ){
    return -1;
}

unsigned ISREndDoc::GetDocumentLength() {
   return postingList.posts[currentPostingsIndex].lengthOfDocument;
}

unsigned ISREndDoc::GetTitleLength() {
    char c = postingList.posts[currentPostingsIndex].title[0];
    unsigned int count = 0;
    while(c != '\0') {
        count++;
    }
   
    return count;
   
}

unsigned ISREndDoc::GetUrlLength() {
    char c = postingList.posts[currentPostingsIndex].URL[0];
    unsigned int count = 0;
    while(c != '\0') {
        count++;
    }
    return count;
}

