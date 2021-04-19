#include "DiskQueue.h"
#include <string.h>

int DiskQueue::openFileRD( int id )
    {
    String filePath = dirName;
    filePath += ltos( id );
    int fd = open( filePath.cstr( ), O_RDONLY );
    if ( fd == -1 )
        std::cerr << "Cannot open file " << filePath << " with errno = " << strerror(errno) << std::endl;
    return fd;
    }

int DiskQueue::openFileWR( int id )
    {
    String filePath = dirName;
    filePath += ltos( id );
    int fd = open( filePath.cstr( ), O_CREAT | O_WRONLY | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO );
    if ( fd == -1 )
        std::cerr << "Cannot open file " << filePath << " with errno = " << strerror(errno) << std::endl;
    return fd;
    }

DiskQueue::DiskQueue( const char *dir )
    {
    bool files[ MAX_NUM_FILE_BLOCKS ];
    for ( size_t i = 0; i < MAX_NUM_FILE_BLOCKS; ++i )
        files[ i ] = false;

    // iterate over existing files to fill "files"
    dirName = String( dir );
    DIR *handle = opendir( dirName.cstr( ) );
    if ( handle )
        {
        dirName += '/';
        struct dirent *entry;
        while ( entry = readdir( handle ) )
            {
            struct stat statbuf;
            String childPath = dirName;
            childPath += String( entry->d_name );
            if ( stat( childPath.cstr( ), &statbuf ) )
                std::cerr << "stat of " << childPath << " failed, errno = " << strerror( errno ) << std::endl;
            else if ( ( statbuf.st_mode & S_IFMT ) != S_IFREG && DotName( childPath.cstr( ) ) )
                std::cerr << "stat of " << childPath << " contains non-file entries" << std::endl;
            else if ( !DotName( entry->d_name ) )
                {
                int fileID = atoi( entry->d_name );
                // if ( fileID < readFileID || readFileID == -1 )
                //     readFileID = fileID;
                // if ( fileID > writeFileID || writeFileID == -1 )
                //     writeFileID = fileID;
                files[ fileID ] = true;
                }
            }
        closedir( handle );   
        }
    else
        {
        std::cerr << "Cannot open directory " << dirName << std::endl;
        exit( 1 );
        }

    // find the head and tail of the files
    readFileID = writeFileID = -1;
    bool prev = files[ MAX_NUM_FILE_BLOCKS - 1 ];
    for ( size_t i = 0; i < MAX_NUM_FILE_BLOCKS; ++i )
        {
        if ( !prev && files[ i ] )  // (0,1) -> head
            readFileID = i;
        else if ( prev && !files[ i ] )  // (1,0) -> tail
            writeFileID = i == 0 ? MAX_NUM_FILE_BLOCKS - 1 : i - 1;
        prev = files[ i ];
        }

    // open files
    // no read files exists (no file or 1 file)
    if ( readFileID == writeFileID ) 
        {
        readFileID = -1;  // no read file
        if  ( writeFileID == -1 )  // empty folder
            {
            writeFileID = 0;
            writeFd = openFileWR( writeFileID );
            bytesWrite = 0;
            }
        else  // only write file but no read file
            {
            writeFd = openFileWR( writeFileID );
            bytesWrite = FileSize( writeFd );
            if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
                {
                close( writeFd );

                // this full file can be used as reader
                readFileID = writeFileID;
                readFd = openFileRD( readFileID );
                readFp = fdopen( readFd, "r" );

                writeFileID = ( writeFileID + 1 ) % MAX_NUM_FILE_BLOCKS;
                writeFd = openFileWR( writeFileID );
                bytesWrite = 0;
                }
            }
        }
    // read file exists
    else
        {
        readFd = openFileRD( readFileID );
        readFp = fdopen( readFd, "r" );

        writeFd = openFileWR( writeFileID );
        bytesWrite = FileSize( writeFd );
        if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
            {
            close( writeFd );
            writeFileID = ( writeFileID + 1 ) % MAX_NUM_FILE_BLOCKS;
            writeFd = openFileWR( writeFileID );
            bytesWrite = 0;
            }
        }
    }

DiskQueue::~DiskQueue( )
    {
    if ( readFileID != -1 )
        // close( readFd );
        fclose( readFp );
    close( writeFd );
    }

const String& DiskQueue::GetDirName( ) const
    {
    return dirName;
    }

bool DiskQueue::empty( ) const
    {
    return readFileID == -1;
    }

bool DiskQueue::full( ) const
    {
    return ( writeFileID + 1 ) % MAX_NUM_FILE_BLOCKS == readFileID;
    }

String DiskQueue::PopFront( ) 
    {
    if ( readFileID == -1 )
        throw DiskQueueExceptions::EmptyDiskQueue;

    char *linePtr = nullptr;
    size_t len = 0;
    ssize_t nread = getline( &linePtr, &len, readFp );
    if ( nread == -1 )  // EOF
        {
        if ( ( readFileID + 1 ) % MAX_NUM_FILE_BLOCKS == writeFileID )  // the next file is in writing mode
            {
            String pathName = dirName;
            pathName += ltos( readFileID );
            if ( unlink( pathName.cstr( ) ) )  // delete the file
                std::cerr << "Error deleting the file " << pathName << " with the errno = " << strerror(errno) << std::endl;
            fclose( readFp );  // this will close the file pointer as well as the file descriptor, 
                               // and delete the file if it is unlinked
            
            readFileID = -1;  // wait for the write file to complete
            free( linePtr );  // free linPtr
            throw DiskQueueExceptions::EmptyDiskQueue;
            }
        else
            {
            String pathName = dirName;
            pathName += ltos( readFileID );
            if ( unlink( pathName.cstr( ) ) )  // delete the file
                std::cerr << "Error deleting the file " << pathName << " with the errno = " << strerror(errno) << std::endl;
            fclose( readFp );

            readFileID = ( readFileID + 1 ) % MAX_NUM_FILE_BLOCKS;
            readFd = openFileRD( readFileID );
            readFp = fdopen( readFd, "r" );
            nread = getline( &linePtr, &len, readFp );
            assert( nread != -1 );
            String ret( linePtr, nread - 1 );  // nread - 1 to exclude the terminating '\n'
            free( linePtr );
            return ret;
            }
        }
    else  // normal reading a line
        {
        String ret( linePtr, nread - 1 );
        free( linePtr );
        return ret;
        }
    }

void DiskQueue::PushBack( String& item )
    {
    // if the file block is full
    if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
        {
        // if the disk queue is full
        if ( ( writeFileID + 1 ) % MAX_NUM_FILE_BLOCKS == readFileID )
            throw DiskQueueExceptions::FullDiskQueue;
        // if there is still space
        else
            {
            close( writeFd );

            // create new file to write
            int oldWriteId = writeFileID;
            writeFileID = ( writeFileID + 1 ) % MAX_NUM_FILE_BLOCKS;
            writeFd = openFileWR( writeFileID );
            bytesWrite = 0;

            // link the read file to the recently closed written file
            if ( readFileID == -1 )
                {
                readFileID = oldWriteId;
                readFd = openFileRD( readFileID );
                readFp = fdopen( readFd, "r" );
                }
            }
        }
    
    // new item must end with newline
    if ( item[ item.size( ) - 1 ] != '\n' )
        item += '\n';
    // write the new line into the file
    if ( write( writeFd, item.cstr( ), item.size( ) ) == -1 )
        std::cerr << "Error writing to file " << item << " with errno = " << strerror( errno ) << std::endl;
    bytesWrite += item.size( );

    // // new item must end with newline
    // if ( item[ item.size( ) - 1 ] != '\n' )
    //     item += '\n';
    // // write the new line into the file
    // if ( write( writeFd, item.cstr( ), item.size( ) ) == -1 )
    //     std::cerr << "Error writing to file " << item << " with errno = " << strerror( errno ) << std::endl;
    // bytesWrite += item.size( );
    // // if the write file exceeds the max file size
    // if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
    //     {
    //     close( writeFd );

    //     // create new file to write
    //     writeFileID++;
    //     writeFd = openFileWR( writeFileID );
    //     bytesWrite = 0;

    //     if ( readFileID == -1 )  // link the read file to the recently closed written file
    //         {
    //         readFileID = writeFileID - 1;
    //         readFd = openFileRD( readFileID );
    //         readFp = fdopen( readFd, "r" );
    //         }
    //     }
    }

void DiskQueue::PrintStatus( ) const
    {
    std::cout << "Directory Name: " << dirName << std::endl
        << "Number of files in the directory: " 
        << ( readFileID == -1 ? ( writeFileID == -1 ? 0 : 1 ) : writeFileID + 1 - readFileID ) << std::endl
        << "ReadFile: " << readFileID << ", WriteFile: " << writeFileID << std::endl;
    }    

bool DotName( const char *name )
    {
    return name[ 0 ] == '.' &&
        ( name[ 1 ] == 0 || name[ 1 ] == '.' && name[ 2 ] == 0 );
    }

size_t FileSize( int f )
    {
    struct stat fileInfo;
    fstat( f, &fileInfo );
    return fileInfo.st_size;
    }