#include "Frontier.h"
#include "Common.h"
#include "BloomFilter.h"
#include <fstream>

// to use this function, compile with HashTable.cpp

bool Frontier::url_t::operator== ( const url_t& other ) const
    {
    // TODO: insert more comparisons here
    return this->url == other.url;
    }

bool Frontier::url_t::operator< ( const url_t& other ) const
    {
    // TODO: insert more comparisons here
    return this->url.size() > other.url.size( );  // shorter urls first
    }

bool Frontier::url_t::operator<= ( const url_t& other ) const
    {
    return ( *this < other ) || ( *this == other );
    }

Frontier::url_t::url_t( )
    {
    }

Frontier::url_t::url_t( const String& param ) : url( param )
    {
    }

int Frontier::randomSelect( ) const
    {
    return rand( ) % urlPool.size( );
    }

void Frontier::linkToRoot( const char *root )
    {
    String rtDir( root );
    rtDir += '/';
    DIR *handle = opendir( root );
    if ( handle )
        {
        struct dirent *entry;
        while ( entry = readdir( handle ) )
            {
            // if entry is the "dot" files
            if ( DotName( entry->d_name ) )
                continue;
            // if entry is not directory
            String childName = rtDir;
            childName += entry->d_name;
            struct stat statbuf;
            if ( !stat( childName.cstr( ), &statbuf ) )
                {
                if ( ( statbuf.st_mode & S_IFMT ) != S_IFDIR )
                    {
                    std::cerr << root << " contains non-directory inode " << entry->d_name << std::endl;
                    continue;
                    }
                }
            else
                std::cerr << "Cannot stat file " << childName << " with errno = " << strerror( errno ) << std::endl;
            // link to the disk queue
            int qIdx = atoi( entry->d_name );
            if ( qIdx > urlPool.size( ) )
                {
                std::cerr << "Disk queue folders have names exceed the urlPool size" << std::endl;
                continue;
                }
            urlPool[ qIdx ] = new DiskQueue( childName.cstr( ) );
            }
        closedir( handle );
        }
    else
        std::cerr << "Cannot open root dir: " << root << " with errno = " << strerror( errno ) << std::endl;
    }

Frontier::Frontier( const char *root, size_t numq, size_t pqSize_, 
    int ( *f )( const Link& ) ) 
    : rootDir( root ), priorityCalculator( f ), pqSize( pqSize_ )
    {
    // initialize urlPool
    urlPool.reserve( numq );
    for ( size_t i = 0; i < numq; ++i )
        urlPool.pushBack( nullptr );
    linkToRoot( root );
    // create directories for non-existing queues
    for ( size_t i = 0; i < numq; ++i )
        if ( !urlPool[ i ] )
            {
            String pathName( root );
            pathName += '/';
            pathName += ltos( i );
            if ( mkdir( pathName.cstr( ), S_IRWXU | S_IRWXG | S_IRWXO ) )
                {
                std::cerr << "Cannot make directory " << pathName << " with errno = " << strerror( errno ) << std::endl;
                exit( 1 );
                }
            urlPool[ i ] = new DiskQueue( pathName.cstr( ) );
            }
    // initialize poolMutexes
    for ( size_t i = 0; i < numq; ++i )
        {
        mutex_t *mtx = new mutex_t;
        MutexInit( mtx, nullptr );
        poolMutexes.pushBack( mtx );
        }
    // initialize pqMutex
    MutexInit( &pqMutex, nullptr );
    }

Frontier::~Frontier( )
    {
    for ( size_t i = 0; i < urlPool.size( ); ++i )
        {
        delete urlPool[ i ];
        MutexDestroy( poolMutexes[ i ] );
        delete poolMutexes[ i ];
        }
    }

void Frontier::FrontierInit( const char *seedFile, FileBloomfilter *filter )
    {
    FILE *fp = fopen( seedFile, "r" );
    char *linePtr = nullptr;
    size_t bufferSize = 0;
    ssize_t bytes;
    while ( ( bytes = getline( &linePtr, &bufferSize, fp ) ) != -1 )
        {
        String url ( "1" );  // 1 stands for https, 0 for http
        url += String( linePtr, bytes - 2 ); // excluding the trailing '\n'
        // String url( linePtr, bytes - 2 );
        filter->insert( url );
        Link lk( url );  
        PushUrl( lk );
        }
    free( linePtr );
    fclose( fp );
    }

void Frontier::PushUrl( Link& link )
    {
    // filter out protocols to save space, 1 for https, 0 for http
    if ( !strncmp( link.URL.cstr( ), "https://", 8 ) )
        {
        String protocol( '1' );
        protocol += String( link.URL.buffer + 8, link.URL.size( ) - 8 );
        link.URL = protocol;
        }
    else if ( !strncmp( link.URL.cstr( ), "http://", 7 ) )
        {
        String protocol( '0' );
        protocol += String( link.URL.buffer + 7, link.URL.size( ) - 7 );
        link.URL = protocol;
        }
    else if ( link.URL[ 0 ] != '1' && link.URL[ 0 ] != '0' )
        {
        String protocol( '1' );
        protocol += link.URL;
        link.URL = protocol;
        }
    // determine which to disk queue to insert
    size_t dqIdx = 0;  // disk queue index
    if ( priorityCalculator )  // use priority calculator if provided
        dqIdx = priorityCalculator( link );
    else  // other wise simply hash and map
    // TODO: should hash over domains, not url
        dqIdx = fnvHash( link.URL.cstr( ), link.URL.size( ) ) % urlPool.size( );
    
    // start inserting into diskqueues
    Lock( poolMutexes[ dqIdx ] );
    try
        {
        urlPool[ dqIdx ]->PushBack( link.URL );
        }
    catch( DiskQueueExceptions& e )
        {
        // std::cerr << "DiskQueue #" << dqIdx << " full, pushing to the next dq\n";
        Unlock( poolMutexes[ dqIdx ]);
        size_t i = ( dqIdx + 1 ) % urlPool.size( );
        while ( i != dqIdx )
            {
            Lock( poolMutexes[ i ] );
            try
                {
                urlPool[ i ]->PushBack( link.URL );
                }
            catch( DiskQueueExceptions& e )
                {
                // std::cerr << "DiskQueue #" << i << " full, pushing to the next dq\n";
                Unlock( poolMutexes[ i ] );
                i = ( i + 1 ) % urlPool.size( );
                continue;
                }
            catch( ... )
                {
                std::cerr << "Unkown exception in dq, url dropped\n";
                }
            Unlock( poolMutexes[ i ] );
            break;
            }
        if ( i == dqIdx )
            std::cerr << "Frontier " << dqIdx 
                << " is full, url " << link.URL << " dropped" << '\n';
        return;
        }
    Unlock( poolMutexes[ dqIdx ]);
    return;
    }

String Frontier::PopUrl( bool alive )
    {
    Lock( &pqMutex );
    // if not alive and the pq is empty
    if ( !alive && urlPq.empty( ) )
        {
        Unlock( &pqMutex );
        return String( "" );
        }
    // if alive and the pq is empty, refills it
    if ( urlPq.empty( ) )
        {
        while ( urlPq.size( ) < pqSize )
            {
            int poolIdx = randomSelect( );
            Lock( poolMutexes[ poolIdx ] );
            if ( urlPool[ poolIdx ]->empty( ) )
                {
                // TODO: busy waiting here, may need to optimize
                // largely depends on the random algorithm to generate a new index that is not empty
                Unlock( poolMutexes[ poolIdx ] );
                continue;
                }
            else
                {
                String poppedUrl;
                try
                    {
                    poppedUrl = urlPool[ poolIdx ]->PopFront( );
                    }
                catch( DiskQueueExceptions& e )
                    {
                    Unlock( poolMutexes[ poolIdx ] );
                    continue;
                    }
                // if the read file pointer is at the EOF of the first file
                if ( poppedUrl.size( ) == 0 )
                    {
                    Unlock( poolMutexes[ poolIdx ] );
                    continue;
                    }
                urlPq.Push( poppedUrl );
                Unlock( poolMutexes[ poolIdx ] );
                }
            }
        }

    // not empty, pop a url off and return
    String nextUrl = urlPq.Top( ).url;
    urlPq.Pop( );
    Unlock( &pqMutex );
    assert( nextUrl[ 0 ] == '0' || nextUrl[ 0 ] == '1' );
    if ( nextUrl[ 0 ] == '1' )
        return String( "https://" ) + String( nextUrl.buffer + 1, nextUrl.size( ) - 1 );
    else
        return String( "http://" ) + String( nextUrl.buffer + 1, nextUrl.size( ) - 1 );
    }