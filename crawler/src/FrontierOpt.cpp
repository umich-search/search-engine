#include "../include/FrontierOpt.h"
#include <iostream>
#include <random>
#include <stdlib.h>
#include <stdio.h>

extern uint32_t fvnHash( const char * );

void *Frontier::randomSelector( void *vft )
    {
    // busy wait because it doesn't know when one of the queue becomes non-empty. TODO: make it more efficient.
    // it is assumed that consuming is much faster than providing because there are hundreds of crawlers popping the domain scheduler but only one random selector putting urls into the domain scheduler
    static int bias[] = {-1, 0.5, 1.5,   3,   5, 7.5, 
                        10.5,  14,  18,22.5,27.5,
                          33,  39,45.5,52.5,  60,
                          68,76.5,85.5,  95, 100};
    Frontier *ft = ( Frontier * )vft;

    Lock( &ft->haltLock );
    while ( !ft->halt )
        {
        Unlock( &ft->haltLock );

        int toss = std::rand( ) % 100;
        size_t psIdx = 0;
        for ( int i = 1; i <= NUM_PSCHEDULER; i++ )
            if ( toss >= bias[ i - 1 ] && toss < bias[ i ] )
                {
                psIdx = i - 1;
                break;
                }

        psIdx = NUM_PSCHEDULER - 1 - psIdx;  // coz test bias is set up in mistake, will be removed in the mature version

        Lock( &ft->psLock );
        DiskQueue *targetq = ft->priorityScheduler[ psIdx ];
        if ( targetq->empty( ) )
            continue;
        String sUrl = targetq->PopFront( );
        Unlock( &ft->psLock );

        ParsedUrl url( sUrl.cstr( ) );
        size_t dsIdx = fvnHash( url.Host ) % NUM_DSCHEDULER;
        
        Lock( &ft->dsLock );
        ft->domainScheduler[ dsIdx ]->PushBack( sUrl );
        Unlock( &ft->dsLock );

        Lock( &ft->haltLock );
        }
    Unlock( &ft->haltLock );
    }

void Frontier::linkScheduler( vector< DiskQueue *>& scheduler, const char *dirName )
    {
    String dir( dirName );
    dir += '/';
    DIR *handle = opendir( dirName );
    if ( !handle )
        std::cerr << "Error openning the directory " << dirName << " with errno = " << errno << std::endl;
    else
        {
        struct dirent *entry;
        while ( entry = readdir( handle ) )
            {
            struct stat statbuf;
            String childPath( dir );
            childPath += String( entry->d_name );
            // stat and check if childPath is directory
            if ( stat( childPath.cstr( ), &statbuf ) )
                std::cerr << "stat of  " << childPath << " failed with errno = " << errno << std::endl;
            else
                if ( ( statbuf.st_mode & S_IFMT ) != S_IFDIR )
                    {
                    std::cerr << dirName << " contains non-directory " << entry->d_name << std::endl;
                    continue;
                    }
            // check if is dot name
            if ( DotName( entry->d_name ) )
                continue;
            // link the the disk queue
            int dqIdx = atoi( entry->d_name );
            if ( dqIdx >= scheduler.size( ) )
                {
                std::cerr << "Disk queue folders have names greater than or equal to " << scheduler.size( ) << " ! Rename the folders!\n";
                exit( 1 );
                }
            scheduler[ dqIdx ] = new DiskQueue( childPath.cstr( ) );
            }
        // create directories that are not in the folder
        for ( int i = 0; i < scheduler.size( ); ++i )
            if ( !scheduler[ i ] )  // not linked disk queue
                {
                String pathName( dir );
                pathName += ltos( i );
                if ( mkdir( pathName.cstr( ), S_IRWXU ) )
                    {
                    std::cerr << "Cannot make directory " << pathName << " with errno = " << errno << std::endl;
                    exit( 1 );
                    }
                scheduler[ i ] = new DiskQueue( pathName.cstr( ) );
                }
        closedir( handle );
        }
    }

Frontier::Frontier( const char *psDir, const char *dsDir, int ( *f )( const Link& ) ) : priorityCalculator( f ), halt( false )
    {
    priorityScheduler.reserve( NUM_PSCHEDULER );
    domainScheduler.reserve( NUM_DSCHEDULER );
    for ( int i = 0; i < NUM_PSCHEDULER; i++ )
        priorityScheduler.pushBack( nullptr );
    for ( int i = 0; i < NUM_DSCHEDULER; i++ )
        domainScheduler.pushBack( nullptr );
    linkScheduler( priorityScheduler, psDir );
    linkScheduler( domainScheduler, dsDir );
    for ( size_t i = 0; i < NUM_DSCHEDULER; ++i )
        timeScheduler.Push( pair( i ) );
    MutexInit( &psLock, nullptr );
    MutexInit( &dsLock, nullptr );
    MutexInit( &tsLock, nullptr );
    CvInit( &psWait, nullptr );
    CvInit( &dsWait, nullptr );
    CvInit( &tsWait, nullptr );
    MutexInit( &haltLock, nullptr );
    CvInit( &haltWait, nullptr );

    rs = new pthread_t;
    pthread_create( rs, nullptr, randomSelector, this );
    }

Frontier::~Frontier( )
    {
    for ( int i = 0; i < priorityScheduler.size( ); i++ )
        delete priorityScheduler[ i ];
    for ( int i = 0; i < domainScheduler.size( ); i++ )
        delete domainScheduler[ i ];
    MutexDestroy( &psLock );
    MutexDestroy( &dsLock );
    MutexDestroy( &tsLock );
    CvDestroy( &psWait );
    CvDestroy( &dsWait );
    CvDestroy( &tsWait );
    MutexDestroy( &haltLock );
    CvDestroy( &haltWait );

    Lock( &haltLock );
    halt = true;
    Unlock( &haltLock );
    pthread_join( *rs, nullptr );
    delete rs;
    }

void Frontier::PushUrl( Link& url )
    {
    int score = priorityCalculator( url );
    Lock( &psLock );
    priorityScheduler[ score - 1 ]->PushBack( url.URL );
    Unlock( &psLock );
    }

const String Frontier::GetUrl( )
    {
    Lock( &tsLock );

    pair top = timeScheduler.Top( );
    // if even the oldest one is accessed too often
    while ( std::chrono::high_resolution_clock::now( ) < top.t )
        usleep( 5000 );  // sleep for 5ms, no need to release the lock
    timeScheduler.Pop( );
    
    Lock( &dsLock );  // deadlock danger?
    String sUrl;
    if ( domainScheduler[ top.dsIdx ]->empty( ) )
        {
        timeScheduler.Push( pair( top.dsIdx ) );
        Unlock( &tsLock );
        Unlock( &dsLock );
        return GetUrl( );
        }
    else
        sUrl = domainScheduler[ top.dsIdx ]->PopFront( );
    timeScheduler.Push( pair( top.dsIdx ) );  // refresh the time scheduler

    Unlock( &tsLock );
    Unlock( &dsLock );
    }
