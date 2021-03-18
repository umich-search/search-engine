#include "./DiskQueue.h"
#include <iostream>

const char DNAME[] = "search-engine/utility/test/dq";
   String google = "http://google.com\n",
         baidu = "https://baidu.com\n",
         facebook = "https://facebook.com\n";

DiskQueue a( DNAME );

void ConstructFromEmptyFolder( )
   {
   a.PrintStatus( );
   }

void ReadWrite( )
   {
   String s = a.PopFront( );
   while ( s.size( ) == 0 )
      {
      std::cout << "Write " << google << " of size " << google.size( ) << std::endl;
      a.PushBack( google );
      s = a.PopFront( );
      }
   std::cout << "Write 100 " << baidu << " into the queue\n";
   for ( int i = 0; i < 100; i++ )
      a.PushBack( baidu );
   std::cout << "Read 20 urls from the queue\n";
   for ( int i = 0; i < 20; i++ )
      std::cout << a.PopFront( );
   }

void ReadEmpty( )
   {
   if ( a.empty( ) )
      assert( a.PopFront( ).size( ) == 0 );
   else
      {
      std::cout << "Poping: \n";
      while ( !a.empty( ) )
         std::cout << a.PopFront( );
      std::cout << std::endl;
      assert( a.PopFront( ).size( ) == 0 );
      }
   std::cout << "Read empty success!\n";
   }

int main( )
    {
    
    a.PrintStatus( );


    while ( a.PopFront( ).size( ) == 0 )
        {
        a.PushBack( google );
        a.PushBack( baidu );
        a.PushBack( facebook );
        }
   String top;
   while ( ( top = a.PopFront( ) ).size( ) > 0 )
      {
      std::cout << top << ' ';
      }
   std::cout << std::endl;
   

    }