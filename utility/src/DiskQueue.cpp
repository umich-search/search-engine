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
    dirName = String( dir );
    DIR *handle = opendir( dirName.cstr( ) );
    readFileID = writeFileID = -1;
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
                long fileID = atol( entry->d_name );
                if ( fileID < readFileID || readFileID == -1 )
                    readFileID = fileID;
                if ( fileID > writeFileID || writeFileID == -1 )
                    writeFileID = fileID;
                }
            }
        closedir( handle );   
        }
    else
        {
        std::cerr << "Cannot open directory " << dirName << std::endl;
        exit( 1 );
        }

    if ( readFileID == writeFileID )  // no read files exists
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

                writeFileID++;
                writeFd = openFileWR( writeFileID );
                bytesWrite = 0;
                }
            }
        }
    else
        {
        readFd = openFileRD( readFileID );
        readFp = fdopen( readFd, "r" );

        writeFd = openFileWR( writeFileID );
        bytesWrite = FileSize( writeFd );
        if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
            {
            close( writeFd );
            writeFileID++;
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

String DiskQueue::PopFront( ) 
    {
    if ( readFileID == -1 )
        return String( "" );
    char *linePtr = nullptr;
    size_t len = 0;
    ssize_t nread = getline( &linePtr, &len, readFp );
    if ( nread == -1 )  // EOF
        {
        if ( readFileID + 1 == writeFileID )  // the next file is in writing mode
            {
            String pathName = dirName;
            pathName += ltos( readFileID );
            if ( unlink( pathName.cstr( ) ) )  // delete the file
                std::cerr << "Error deleting the file " << pathName << " with the errno = " << strerror(errno) << std::endl;
            fclose( readFp );  // this will close the file pointer as well as the file descriptor, 
                               // and delete the file if it is unlinked
            
            readFileID = -1;  // wait for the write file to complete
            free( linePtr );  // free linPtr
            return String( "" );
            }
        else
            {
            String pathName = dirName;
            pathName += ltos( readFileID );
            if ( unlink( pathName.cstr( ) ) )  // delete the file
                std::cerr << "Error deleting the file " << pathName << " with the errno = " << strerror(errno) << std::endl;
            fclose( readFp );

            ++readFileID;
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
    if ( item[ item.size( ) - 1 ] != '\n' )
        item += '\n';
    // write the new line into the file
    if ( write( writeFd, item.cstr( ), item.size( ) ) == -1 )
        std::cerr << "Error writing to file " << item << " with errno = " << strerror(errno) << std::endl;
    bytesWrite += item.size( );
    // if the write file exceeds the max file size
    if ( bytesWrite >= MAX_FILE_BLOCK_IN_BYTES )
        {
        close( writeFd );

        // create new file to write
        writeFileID++;
        writeFd = openFileWR( writeFileID );
        bytesWrite = 0;

        if ( readFileID == -1 )  // link the read file to the recently closed written file
            {
            readFileID = writeFileID - 1;
            readFd = openFileRD( readFileID );
            readFp = fdopen( readFd, "r" );
            }
        }
    }

void DiskQueue::PrintStatus( ) const
    {
    std::cout << "Directory Name: " << dirName << std::endl
        << "Number of files in the directory: " 
        << ( readFileID == -1 ? ( writeFileID == -1 ? 0 : 1 ) : writeFileID - readFileID ) << std::endl
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