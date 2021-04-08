
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
    if(docDetailsSize == 0) {
        return 0;
    }
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
    if (docDetailsBlob == MAP_FAILED ) {
       throw "Mapping failed";
    }
    
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
    
    int f_metadata = open( CHUNKS_METADATA_PATH,
                           O_CREAT |
                           O_RDWR,
                           S_IRWXU | S_IRWXG | S_IRWXO );
    
    if (f_metadata == -1) {
        std::cout << "Failed to open file: " << f_metadata << ", with error number " << errno << std::endl;
         throw "file open failed";
    }
    if(FileSize(f_metadata) == 0) {
        ftruncate(f_metadata, sizeof(ChunksMetadata) + sizeof(Location) + sizeof(d_Occurence));
        metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        if (metadata == MAP_FAILED ) {
           throw "Mapping failed";
        }
        metadata->numWords = numWords;
        metadata->numDocs = numDocs;
        metadata->endLocation = endLocation;
        metadata->numUniqueWords = numUniqueWords;
        metadata->numChunks = 1;
        char * curr = ((char*)metadata + sizeof(ChunksMetadata));
        memcpy(curr, (char*)&endLocation, sizeof(Location));
        memcpy(curr + sizeof(Location), (char*)&numDocs, sizeof(d_Occurence));
    }
    else {
        if(numChunks == 0) {
            ftruncate(f_metadata, 0);
        }
        ftruncate(f_metadata, FileSize(f_metadata) + sizeof(Location) + sizeof(d_Occurence));
        metadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        if (mmap == MAP_FAILED ) {
           throw "Mapping failed";
        }
        metadata->numWords = numWords;
        metadata->numDocs = numDocs;
        metadata->endLocation = endLocation;
        metadata->numUniqueWords = numUniqueWords;
        Location chunkEnd;
        if(metadata->numChunks == 0) {
            chunkEnd = endLocation;
        }
        else {
            chunkEnd = *(Location*)((char*)metadata + sizeof(ChunksMetadata) + (sizeof(Location) + sizeof(d_Occurence)) * (metadata->numChunks - 1)) + endLocation;
        }
        char * curr = (char*)metadata + sizeof(ChunksMetadata) + (sizeof(Location) + sizeof(d_Occurence)) * metadata->numChunks;
        memcpy(curr, (char*)&chunkEnd, sizeof(Location));
        memcpy(curr + sizeof(Location), (char*)&numDocs, sizeof(d_Occurence));
        metadata->numChunks = numChunks + 1;


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
                  size_t numChunks)
   {
       char chunkFile[MAX_PATHNAME_LENGTH];
       char docsFile[MAX_PATHNAME_LENGTH];
       resolveChunkPath(numChunks, chunkFile);
       resolveDocsChunkPath(numChunks, docsFile);
       writePostingListsToFile(termIndex, endDocList, chunkFile);
       writeDocsToFile(docDetails, docsFile);
       writeMetadataToFile(numWords, numUniqueWords, numDocs, endLocation, numChunks );
       return 0;
   }

    int FileManager::ReadChunk(size_t chunkIndex) {
        if(chunkIndex > chunksMetadata->numChunks) {
            throw "Error: Attempting to read more than available chunks";
        }
        char chunkFile[MAX_PATHNAME_LENGTH];
        resolveChunkPath(chunkIndex, chunkFile);
        void * blob;
        int f_chunk = open( chunkFile, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
        if (f_chunk == -1 ) {
            std::cerr << "Error openning file " << chunkFile << " with errno = " << strerror( errno ) << std::endl;
            return -1;
         }
        size_t fileSize = FileSize(f_chunk);
        blob = ( HashBlob *)mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, f_chunk, 0);
        if (blob == MAP_FAILED ) {
           throw "Mapping failed";
        }
        endDocListBlob = (SerialEndDocs *)blob;
        size_t endDocEnd = RoundUp(endDocListBlob->Length, sizeof(size_t));
        char* curr = (char*)blob + endDocEnd;
        termIndexBlob = (HashBlob *)curr;
        close(f_chunk);
        return 0;
    }

int FileManager::ReadDocuments(Offset docsChunkIndex) {
    if(docsChunkIndex > chunksMetadata->numChunks) {
        throw "Error: Attempting to read more than available chunks";
    }
    char docsChunkFile[MAX_PATHNAME_LENGTH];
    resolveDocsChunkPath(docsChunkIndex, docsChunkFile);
    void * blob;
    int f_doc_chunk = open( docsChunkFile, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
    if (f_doc_chunk == -1 ) {
        std::cerr << "Error openning file " << docsChunkFile << " with errno = " << strerror( errno ) << std::endl;
        return -1;
     }
    size_t fileSize = FileSize(f_doc_chunk);
    blob = ( HashBlob *)mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, f_doc_chunk, 0);
    if (blob == MAP_FAILED ) {
       throw "Mapping failed";
    }
    docsBlob = (const char *)blob;
    close(f_doc_chunk);
    return 0;
}

// May return refernece
TermPostingListRaw FileManager::GetTermList(const char * term, size_t chunkIndex) {
    if(chunkIndex == -1) {
        throw "Error: No chunk initialized";
    }
    ReadChunk(chunkIndex);
    if(!termIndexBlob) {
        throw "Error: No chunk has been read";
    }
    
    const SerialTuple * tuple = termIndexBlob->Find(term);
    if(!tuple) {
        throw "Error: Term does not exist";
    }
    else {
        return TermPostingListRaw(tuple->DynamicSpace);
    }
}

EndDocPostingListRaw FileManager::GetEndDocList(size_t chunkIndex) {
    if(chunkIndex > chunksMetadata->numChunks) {
        throw "Error: Attempting to read more than available chunks";
    }
    ReadChunk(chunkIndex);
    if(!endDocListBlob) {
        throw "Error: No chunk has been read";
    }
    return EndDocPostingListRaw(endDocListBlob->DynamicSpace);
}

DocumentDetails FileManager::GetDocumentDetails(Offset docIndex, Offset docsChunkIndex) {
    if(docsChunkIndex == -1) {
        throw "Error: No chunk initalized";
    }
    ReadDocuments(docsChunkIndex);
    if(!docsBlob) {
        throw "Error: No document chunk has been read";
    }
    Offset normalize = 0;
    if(docsChunkIndex > 0) {
        normalize = *(d_Occurence *)((chunksMetadata->dynamicSpace + ( docsChunkIndex - 1) * (sizeof(Location) + sizeof(d_Occurence))) + sizeof(Location));
    }
    
    const char * curr = (docsBlob + DOCUMENT_SIZE * (docIndex - normalize));
    w_Occurence numWords = *(w_Occurence *)curr;
    w_Occurence numUniqueWords = *(w_Occurence *)(curr + sizeof(w_Occurence));
    const char * url = curr + 2 * sizeof(w_Occurence);
    const char * title = curr + 2 * sizeof(w_Occurence) + MAX_URL_LENGTH;
    

    return DocumentDetails(url, title, numWords, numUniqueWords);
}


int FileManager::ReadMetadata() {
    int f_metadata = open( CHUNKS_METADATA_PATH,
                           O_CREAT | O_RDWR,
                           S_IRWXU | S_IRWXG | S_IRWXO );
    if(f_metadata == -1) {
        std::cerr << "Error openning file " << CHUNKS_METADATA_PATH << " with errno = " << strerror( errno ) << std::endl;
        return -1;
    }
    if(FileSize(f_metadata) == 0) {
        ftruncate(f_metadata, sizeof(ChunksMetadata));
        chunksMetadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        if (chunksMetadata == MAP_FAILED ) {
           throw "Mapping failed";
        }
        chunksMetadata->numWords = 0;
        chunksMetadata->numDocs = 0;
        chunksMetadata->endLocation = 0;
        chunksMetadata->numUniqueWords = 0;
        chunksMetadata->endLocation = 0;
        chunksMetadata->numChunks = 0;
    }
    else {
        chunksMetadata = (ChunksMetadata*)mmap( nullptr, FileSize(f_metadata), PROT_READ | PROT_WRITE, MAP_SHARED, f_metadata, 0);
        if (chunksMetadata == MAP_FAILED ) {
           throw "Mapping failed";
        }
    }
    close(f_metadata);
    return 0;
}
   
w_Occurence FileManager::getIndexWords() {
    return chunksMetadata->numWords;
}

Offset FileManager::getNumChunks() {
    return chunksMetadata->numChunks;
}

d_Occurence FileManager::getNumDocuments() {
    return chunksMetadata->numDocs;
}

Location FileManager::getIndexEndLocation() {
    return *(Location *)(chunksMetadata->dynamicSpace + (sizeof(Location) + sizeof(d_Occurence))* (chunksMetadata->numChunks - 1));
}

::vector<Location> FileManager::getChunkEndLocations() {
    ::vector<Location> endLocs;
    
    for(unsigned int i = 0; i < chunksMetadata->numChunks; ++i) {
        endLocs.pushBack((*(Location *)(chunksMetadata->dynamicSpace + (sizeof(Location) + sizeof(d_Occurence))* i)));
    }
    return endLocs;
}
