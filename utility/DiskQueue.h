// Implement the interface to a file queue
// the DiskQueue has to be able to reserve the status during breakdown
// I find no efficient ways to erase the first N bytes of a file without reading and writing the remaining part N bytes 
// ahead of their current location. So my solution is to maintain a growing number of block files of fixed size. When 
// popping, we take element sequentially from the first block. When the first block exhausts, we remove it directly. When 
// pushing, we pushback to the last file in the set of blocks. When the last block reaches the maximum size, we create a new 
// block to write.

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cassert>
#include "./string.h"

const unsigned int MAX_FILE_BLOCK_IN_BYTES = 20;

bool DotName( const char * );
String ltos( int );
size_t FileSize( int );

class DiskQueue
    {
    String dirName;

    int readFd, writeFd;  // the open file to read and  write
    int readFileID, writeFileID;
    size_t bytesWrite;  // number of bytes written in writeFd
    FILE *readFp; // assist reading line by line

    // read file maintains:
    //  1. readFileID
    //  2. readFd
    //  3. readFp

    // write file maintains:/
    //  1. writeFileID
    //  2. writeFd
    //  3. bytesWrite

    // read file and write file must operate on different files, so there need not to be locks. Any operations on the file requires checking the FileID first.

    int openFileRD( int );
    int openFileWR( int );

    public:
        // Link the disk queue to the directory dirName
        // link the local blocks
        // dirName should be absolute path
        DiskQueue( const char* );
        ~DiskQueue( );
        const String& GetDirName( ) const;
        bool empty( ) const;
        String PopFront( );
        void PushBack( String& item );
        void PrintStatus( ) const;
    };