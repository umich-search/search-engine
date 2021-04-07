
#include "FileManager.h"


int FileManager::resolveChunkPath(size_t offset, char * pathname) {
    char buffer[MAX_PATHNAME_LENGTH];
    strcpy(buffer, CHUNK_DIRECTORY);
    strcat(buffer, "%zu.chunk");
    sprintf(pathname, buffer, offset);
    return 0;
}
int FileManager::resolveDocsChunkPath(size_t offset, char * pathname) {
    char buffer[MAX_PATHNAME_LENGTH];
    strcpy(buffer, CHUNK_DIRECTORY);
    strcat(buffer, "%zu.dchunk");
    sprintf(pathname, buffer, offset);
    return 0;
}

int FileManager::writePostingListsToFile(SharedPointer<TermHash> termIndex,
                                        SharedPointer<EndDocPostingList>
                                         endDocList, const char *pathname){
    void * blob;
    int f_chunk = open( pathname,
                        O_CREAT | O_RDWR,
                        S_IRWXU | S_IRWXG | S_IRWXO);
    
    if ( f_chunk == -1) {
       std::cout << "Failed to open file: " << pathname << ", with error number " << errno << std::endl;
       throw "File open failed";
    }
    
    size_t termListSize, endDocEnd;
    
    termListSize = RoundUp(HashBlob::BytesRequired(termIndex), sizeof(size_t));
    endDocEnd = RoundUp(SerialEndDocs::BytesRequired(endDocList), sizeof(size_t));

    size_t fileSize = endDocEnd + termListSize;
    ftruncate( f_chunk, fileSize );

    blob = ( HashBlob * )mmap( nullptr,
                                fileSize,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                f_chunk,
                                0);
    
    if (blob == MAP_FAILED ) {
       throw "Mapping failed";
    }

    else {
        try {
            blob = SerialEndDocs::Write((char*)blob, (char *)blob + endDocEnd, endDocList);
            blob = HashBlob::Write((HashBlob *)blob, termListSize, termIndex);
        }
        catch (const char* excp){
            return -1;
        }
    }
    close(f_chunk);
    return 0;
}



int FileManager::writeDocsToFile(::vector<SharedPointer<DocumentDetails>> &docDetails, const char *pathname ) {
    void * docDetailsBlob;
    size_t docDetailsSize = docDetails.size() * DOCUMENT_SIZE;
    int f_doc_details = open( pathname,
                              O_CREAT | O_RDWR,
                              S_IRWXU | S_IRWXG | S_IRWXO );
    
    if (f_doc_details == -1) {
        std::cout << "Failed to open file: " << pathname << ", with error number " << errno << std::endl;
        throw "file open failed";
    }
    
    ftruncate(f_doc_details, docDetailsSize);
    docDetailsBlob = mmap( nullptr,
                           docDetailsSize,
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED,
                           f_doc_details,
                           0 );
    
     for(size_t i = 0; i < docDetails.size(); ++i){
         try {
             docDetailsBlob = DocumentBlob::Write((char *)docDetailsBlob, (char *)docDetailsBlob + DOCUMENT_SIZE, docDetails[i]);
         }
         catch (const char * excp) {
             return -1;
         }
    }
    close(f_doc_details);
    return 0;
}

int FileManager::writeMetadataToFile(w_Occurence numWords, w_Occurence numUniqueWords, d_Occurence numDocs, Location endLocation, size_t numChunks) {
    ChunksMetadata *metadata;
    size_t numWordsSize, numUniqueWordsSize, numDocsSize, endLocationSize;
    numWordsSize = sizeof(w_Occurence);
    numUniqueWordsSize = sizeof(w_Occurence);
    numDocsSize = sizeof(d_Occurence);
    endLocationSize = sizeof(endLocation);
    
    int f_metadata = open( CHUNKS_META_FILENAME,
                           O_CREAT |
                           O_RDWR,
                           S_IRWXU | S_IRWXG | S_IRWXO );
    
    if (f_metadata == -1) {
        std::cout << "Failed to open file: " << f_metadata << ", with error number " << errno << std::endl;
         throw "file open failed";
    }
    if(FileSize(f_metadata) == 0) {
        ftruncate(f_metadata, sizeof(ChunksMetadata) + sizeof(Location));
        metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        metadata->numWords = 0;
        metadata->numDocs = 0;
        metadata->endLocation = 0;
        metadata->numUniqueWords = 0;
        metadata->numChunks = 0;
        metadata->numDocDetails = 0;
        char * curr = ((char*)metadata + sizeof(ChunksMetadata));
        memcpy(curr, (char*)&endLocation, sizeof(Location));
    }
    else {
        ftruncate(f_metadata, FileSize(f_metadata) + sizeof(Location));
        metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        metadata->numWords = numWords;
        metadata->numDocs = numDocs;
        metadata->endLocation = endLocation;
        metadata->numUniqueWords = numUniqueWords;
        metadata->numChunks = numChunks;
        char * curr = (char*)metadata + sizeof(ChunksMetadata) + sizeof(Location) * metadata->numDocDetails;
        memcpy(curr, (char*)&endLocation, sizeof(Location));
        metadata->numDocDetails++;
    }
    close(f_metadata);
    return 0;

}


int FileManager::WriteChunk(SharedPointer<HashTable<String, TermPostingList *>> termIndex,
                  SharedPointer<EndDocPostingList> endDocList, 
                  w_Occurence numWords, 
                  w_Occurence numUniqueWords, 
                  d_Occurence numDocs, 
                  Location endLocation,
                  ::vector<SharedPointer<DocumentDetails>> docDetails,
                  size_t numChunks,
                  size_t chunkOffset,
                  size_t docsOffset)
   {
       if(chunkOffset > numChunks) {
           throw "Error: Attempting to skip chunk";
       }
       char chunkFile[MAX_PATHNAME_LENGTH];
       char docsFile[MAX_PATHNAME_LENGTH];
       resolveChunkPath(chunkOffset, chunkFile);
       resolveDocsChunkPath(docsOffset, docsFile);
       writePostingListsToFile(termIndex, endDocList, chunkFile);
       writeDocsToFile(docDetails, docsFile);
       writeMetadataToFile(numWords, numUniqueWords, numDocs, endLocation, numChunks );
       return 0;
   }

    int FileManager::ReadChunk(size_t chunkOffset) {
        if(chunkOffset > chunksMetadata->numChunks) {
            throw "Error: Attempting to read more than available chunks";
        }
        char chunkFile[MAX_PATHNAME_LENGTH];
        resolveChunkPath(chunkOffset, chunkFile);
        void * blob;
        int f_chunk = open( chunkFile, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
        if (f_chunk == -1 ) {
            std::cerr << "Error openning file " << chunkFile << " with errno = " << strerror( errno ) << std::endl;
            return -1;
         }
        size_t fileSize = FileSize(f_chunk);
        blob = ( HashBlob *)mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, f_chunk, 0);
        endDocListBlob = (SerialEndDocs *)blob;
        size_t endDocEnd = RoundUp(endDocListBlob->Length, sizeof(size_t));
        char* curr = (char*)blob + endDocEnd;
        termIndexBlob = (HashBlob *)curr;
        close(f_chunk);
        return 0;

    }

// May return refernece
TermPostingListRaw FileManager::GetTermList(const char * term, size_t chunkOffset) {
    if(chunkOffset == -1) {
        throw "Error: No chunk initialized";
    }
    ReadChunk(chunkOffset);
    if(!termIndexBlob) {
        throw "Error: No chunk has been read";
    }
    
    const SerialTuple * tuple = termIndexBlob->Find(term);
    if(!tuple) {
        return nullptr;
    }
    else {
        return TermPostingListRaw(tuple->DynamicSpace);
    }
}

EndDocPostingListRaw FileManager::GetEndDocList(size_t chunkOffset) {
    if(chunkOffset > chunksMetadata->numChunks) {
        throw "Error: Attempting to read more than available chunks";
    }
    ReadChunk(chunkOffset);
    if(!endDocListBlob) {
        throw "Error: No chunk has been read";
    }
    return EndDocPostingListRaw(endDocListBlob->DynamicSpace);
}


int FileManager::ReadMetadata() {
    int f_metadata = open( CHUNKS_META_FILENAME,
                           O_CREAT | O_RDWR,
                           S_IRWXU | S_IRWXG | S_IRWXO );
    if(f_metadata == -1) {
        std::cerr << "Error openning file " << CHUNKS_META_FILENAME << " with errno = " << strerror( errno ) << std::endl;
        return -1;
    }
    if(FileSize(f_metadata) == 0) {
        ftruncate(f_metadata, sizeof(ChunksMetadata));
        chunksMetadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        chunksMetadata->numWords = 0;
        chunksMetadata->numDocs = 0;
        chunksMetadata->endLocation = 0;
        chunksMetadata->numUniqueWords = 0;
        chunksMetadata->endLocation = 0;
        chunksMetadata->numChunks = 0;
    }
    else {
        chunksMetadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
    }
    close(f_metadata);
    return 0;
}
   
