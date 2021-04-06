#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

struct SerialEndDocs {
public:
    size_t Length;//
    char DynamicSpace[ Unknown ];

    static size_t BytesRequired ( const CommonHeader * header) {
        size_t typeSize, numOfOccurenceSize, numOfDocumentSize, termSize;
        typeSize = sizeof(int);
        numOfDocumentSize = sizeof(w_Occurence);
        numOfOccurenceSize = sizeof(d_Occurence);
        termSize = header->term.size() + 1;
        return typeSize + numOfOccurenceSize + numOfDocumentSize + termSize;
    }
    
    // Bytes required to encode synchronization index
    static size_t BytesRequired( const SyncIndex *syncIndex) {
       return syncIndex->postingsListOffset.size() * 2 * sizeof(size_t);
    }
    // Bytes required to encode TermPostingList
    static size_t BytesRequired( const EndDocPostingList *p) {
       // TODO: Have term in Key and Value, may not need both
    /*
       TERM POSTING LIST DATA TO SERIALIZE:
       CommonHeader header
       SyncIndex syncIndex
       vector<IPostTerm> posts
    */
      size_t headerSize, syncIndexSize, endDocsSize, memberVarSize;
      headerSize = BytesRequired(&p->header);
      syncIndexSize = BytesRequired(&p->syncIndex);
      // TODO: may have to move this posts size if using variable encoding
      endDocsSize = p->posts.size() * sizeof(size_t);
        memberVarSize = sizeof(Offset);
      return headerSize + syncIndexSize + endDocsSize + memberVarSize;
    }
    
    static char *Write( char *buffer, char *bufferEnd, const EndDocPostingList *p ) {
        // TODO: Handle null case better?
       if ( !p ) {
          //SerialTuple *sentinel = new( ( void * )buffer ) SerialTuple;
          //sentinel->Length = 0;
           return buffer + RoundUp( sizeof( SerialEndDocs ), sizeof(size_t));//RoundUp( sizeof( SerialTuple ), sizeof( size_t ) );
       }
        size_t bytes = BytesRequired( p );// + sizeof(size_t);
       if ( bufferEnd - buffer < bytes )
          throw "Not enough memory allocated!";

       SerialEndDocs *head = new( ( void * )buffer ) SerialEndDocs;
        // TODO: Does size need to account for length
        // TODO: Is this length feature necessary?
        head->Length = bytes;// + sizeof(size_t);
       char * curr = head->DynamicSpace;
        
       // Copy header
       memcpy(curr, &p->header.numOfOccurence, sizeof(w_Occurence));
       memcpy(curr + sizeof(w_Occurence), &p->header.numOfDocument, sizeof(d_Occurence));
       memcpy(curr + sizeof(w_Occurence) + sizeof(d_Occurence), &p->header.type, sizeof(int));
        // TODO: Should modify header?
       memcpy(curr + sizeof(int) + sizeof(w_Occurence) + sizeof(d_Occurence), p->header.term.cstr(), p->header.term.size() + 1);
       curr += BytesRequired(&p->header);
       for(size_t i = 0; i < p->syncIndex.postingsListOffset.size(); ++i) {
           memcpy(curr, &p->syncIndex.postingsListOffset[i], sizeof(size_t));
           curr += sizeof(size_t);
       }
       for(size_t i = 0; i < p->syncIndex.postLocation.size(); ++i) {
           memcpy(curr, &p->syncIndex.postLocation[i], sizeof(size_t));
           curr += sizeof(size_t);
       }
        for(size_t i = 0; i < p->posts.size(); ++i) {
            memcpy(curr, &p->posts[i], sizeof(size_t));
            curr += sizeof(size_t);
        }
       return buffer + RoundUp( bytes, sizeof( size_t ) );
       }
    
    // Create allocates memory
    // Caller is responsible for discarding when done.
    static SerialEndDocs *Create( const EndDocPostingList *endDocs )
       {
       size_t bytes = BytesRequired( endDocs );
       char *blobByte = new char[ bytes ];
           Write( blobByte, blobByte + bytes, endDocs );
           return ( SerialEndDocs *) blobByte;
       }

    // Discard
// TODO: Fix discard
    /*
    static void Discard( HashBlob *blob )
       {
       // Your code here.
       char *blobByte = ( char * )blob;
       delete [] blobByte;
       }
     */
    
};
