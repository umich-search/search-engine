#pragma once

#include "Global.h"
#include "PostingList.h"
#include "PostingListBlob.h"
#include "DocumentsSerializer.h"
#include "DictionarySerializer.h"
#include "EndDocSerializer.h"
#include <cstring>

using TermHash = HashTable< String, TermPostingList* >;

class FileManager {
private:
   static size_t FileSize( int f) {
      struct stat fileInfo;
      fstat( f, &fileInfo );
      return fileInfo.st_size;
   }
   struct ChunksMetadata {
      Offset numChunks;
      w_Occurence numWords;
      w_Occurence numUniqueWords;
      d_Occurence numDocs;
      Location endLocation;
       Offset numDocDetails;
      char docDetailsLoc[];
   };
    
    static int resolveChunkPath(size_t offset, char * pathname);
    static int resolveDocsChunkPath(size_t offset, char * pathname);
    
    // Reads metadata file
    int ReadMetadata();
    static int writePostingListsToFile(SharedPointer<TermHash> termIndex,
                                SharedPointer<EndDocPostingList>
                                endDocList, const char *pathname);
    static int writeDocsToFile(::vector<SharedPointer<DocumentDetails>> &docDetails, const char *pathname );
    static int writeMetadataToFile(w_Occurence numWords, w_Occurence numUniqueWords, d_Occurence numDocs, Location endLocation, size_t numChunks);


public:
   HashBlob * termIndexBlob;
   SerialEndDocs * endDocListBlob;
   ChunksMetadata * chunksMetadata;

   void * docsBlob;
   void * chunkDetails;
    
    FileManager() {
        ReadMetadata();
    }

    static int WriteChunk(SharedPointer<TermHash> termIndex, 
                  SharedPointer<EndDocPostingList> endDocList, 
                  w_Occurence numWords, 
                  w_Occurence numUniqueWords, 
                  d_Occurence numDocs, 
                  Location endLocation,
                  ::vector<SharedPointer<DocumentDetails>> docDetails,
                  size_t numChunks,
                  size_t chunkOffset,
                  size_t docsOffset);

    // Bring chunk into memory
    int ReadChunk(size_t chunkOffset);
    // Bring documents into memory
    int ReadDocuments(size_t docChunkOffset);
    // Returns term list given term and optional chunk_path
    TermPostingListRaw GetTermList(const char* term , size_t chunkOffset = 0);
    // Returns end doc list given term and optional chunk_path
    EndDocPostingListRaw GetEndDocList(size_t chunkOffset = 0);
    // Get the number of chunks in the index
    Offset getNumChunks();
    // Get total number of words in index
    w_Occurence getIndexWords();
    // Get total number of unique words in index
    w_Occurence getIndexUniqueWords();
    // get end location of index
    Location getIndexEndLocation();
    // get number of documents in index
    d_Occurence getNumDocuments();
    // get vector of chunk endlocations
    // TODO: Got to change so num words is reset for each chunk
    ::vector<Location> getChunkEndLocations();
};
