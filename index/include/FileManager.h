#pragma once

#include "Global.h"
#include "PostingList.h"
#include "PostingListBlob.h"
#include "DocumentsSerializer.h"
#include "DictionarySerializer.h"
#include "EndDocSerializer.h"
#include <cstring>

using TermHash = HashTable< String, TermPostingList* >;

class FileManager 
   {
   private:
      // Get file size
      static size_t FileSize( int f) 
         {
         struct stat fileInfo;
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

      struct ChunksMetadata 
         {
         Offset numChunks;
         w_Occurence numWords;
         w_Occurence numUniqueWords;
         d_Occurence numDocs;
         Location endLocation;
         char dynamicSpace[ ];
         };
      Offset managerNumChunks = -1;
   
      // Resolve URL paths
      static int resolveChunkPath(size_t offset, char * pathname, size_t threadID);
      static int resolveDocsChunkPath(size_t offset, char * pathname, size_t threadID);
      static int resolveMetadataPath(size_t offset, char * pathname, size_t threadID);
    
      // Reads metadata file
      int ReadMetadata(Offset givenChunk = -1);

      // Write to file
      static int writePostingListsToFile(SharedPointer<TermHash> termIndex, SharedPointer<EndDocPostingList> endDocList, const char *pathname);
      static int writeDocsToFile(::vector<SharedPointer<DocumentDetails>> &docDetails, const char *pathname );
      static int writeMetadataToFile(w_Occurence numWords, w_Occurence numUniqueWords, d_Occurence numDocs, Location endLocation, size_t numChunks, const char * pathname, const char * prevMetadata);

      // Unmap previously mapped files
      void unmapMetadata( );
      void unmapChunk( );
      void unmapDocs( );

   public: 
      // attributes
      HashBlob * termIndexBlob;
      SerialEndDocs * endDocListBlob;
      ChunksMetadata * chunksMetadata;
      const char * docsBlob;
      // void * chunkDetails;
      size_t threadID;

      size_t chunksMetadataSize;  // size of the metadata file to unmap
      size_t chunkSize;  // size of the chunk to unmap
      size_t docsBlobSize;  // size of the document to unmap
      
      // constructor
      FileManager( size_t thread ) 
         : termIndexBlob( nullptr ), endDocListBlob( nullptr ), chunksMetadata( nullptr ), docsBlob( nullptr ),
         threadID( thread ), chunksMetadataSize( 0 ), chunkSize( 0 ), docsBlobSize( 0 ) 
         {
         // threadID = thread;
         ReadMetadata( );
         }

    static int WriteChunk(SharedPointer<TermHash> termIndex, 
                  SharedPointer<EndDocPostingList> endDocList,
                  w_Occurence numWords,
                  w_Occurence numUniqueWords, 
                  d_Occurence numDocs, 
                  Location endLocation,
                  ::vector<SharedPointer<DocumentDetails>> docDetails,
                  size_t chunkIndex,
                  size_t threadID);
    // Bring chunk into memory
    int ReadChunk(Offset chunkIndex);
    // Bring documents into memory
    int ReadDocuments(Offset docsChunkIndex);
    // Returns term list given term and optional chunk_path
    TermPostingListRaw GetTermList(const char* term , size_t chunkIndex = 0);
    // Returns end doc list given term and optional chunk_path
    EndDocPostingListRaw GetEndDocList(size_t chunkIndex = 0);
    // Returns document details of doc index
    DocumentDetails GetDocumentDetails(Offset docIndex, Offset docsChunkIndex );
    // Get the number of chunks in the index
    Offset getNumChunks();
    // Get total number of words in index
    w_Occurence getIndexWords();
    // get end location of index
    Location getIndexEndLocation();
    // get number of documents in index
    d_Occurence getNumDocuments();
    // get vector of chunk endlocations
    ::vector<Location> getChunkEndLocations();
    // get vector of total doc count up to chunk
    ::vector<d_Occurence> getDocCountsAfterChunk();
};