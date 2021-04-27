// implementation of bitmap in file "bitmap_f"

#include "Bitmap.h"
#include "DiskQueue.h"

bitmap_f::bitmap_f( const char *filename, size_t numBits )
    : f_name( filename ), bitSize( numBits )
    {
    // open storage file
    size_t fileSize;
    int fd = open( f_name.cstr( ), O_RDWR );
    if ( fd != -1 )  // file opened successfully
        fileSize = FileSize( fd );
    else if  ( errno == 2 )  // file not exists
        {
        fd = open( f_name.cstr( ), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );  // create file
        fileSize = ( bitSize / 32 + 1 ) * sizeof( uint32_t );  // calculate file size
        ftruncate( fd, fileSize );  // allocate enough memory
        }
    else  // erro openning the file
        std::cerr << "Fail to open file: " << f_name << " with errno = " << strerror( errno ) << std::endl;
    
    // map file into memory
    v = ( uint32_t * )mmap( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
    if ( v == MAP_FAILED )
        std::cerr << "Map failed with errno = " << strerror( errno ) << std::endl;
    mapSize = fileSize;
    close( fd );
    }

bitmap_f::~bitmap_f( )
    {
    }

void bitmap_f::set( size_t i )
    {
    v[ i >> 5 ] |= ( 1 << ( i & 31 ) );
    }

bool bitmap_f::isTrue( size_t i ) const
    {
    size_t r = 1 << ( i & 31 );
    if ( r == ( v[i >> 5 ] & r ) )
        return true;
    else return false;
    }

size_t bitmap_f::size( ) const
    {
    return bitSize;
    }