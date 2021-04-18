#include "DiskQueue.h"
#include "mString.h"
#include <iostream>
#include <fstream>

const char DNAME[] = "./dq";

int main( )
    {
//     DiskQueue a( DNAME );
//     a.PrintStatus( );

//     String google = "http://google.com\n",
//             baidu = "https://baidu.com\n",
//             facebook = "https://facebook.com\n";
//     a.PushBack( google );
//     std::cout << "Sequential Pops: " << a.PopFront( ) << a.PopFront( ) << a.PopFront( ) << std::endl;
    DiskQueue a( DNAME );
    a.PrintStatus( );
    std::ifstream sin;
    sin.open( "testUrl.txt" );
    try
        {
        String s;
        while ( sin >> s )
            {
            a.PushBack( s );
            std::cout << s;
            }
        }
   catch ( DiskQueueExceptions& e )
      {
      if ( e == DiskQueueExceptions::EmptyDiskQueue )
         {
         std::cout << "Empty\n";
         assert( a.empty( ) );
         }
      else if ( e == DiskQueueExceptions::FullDiskQueue )
         {
         std::cout << "Full\n";
         assert( a.full( ) );
         }
      else
         std::cout << "Other issues\n";
      a.PrintStatus( );
      }
   std::cout << "\nDisplay inserted urls:\n" << "Sequential pop:\n";
   try
      {
      String s;
      while ( !a.empty( ) )
         std::cout << a.PopFront( ) << std::endl;
      }
   catch( DiskQueueExceptions& e )
      {
      if ( e == DiskQueueExceptions::EmptyDiskQueue )
         {
         std::cout << "Empty\n";
         assert( a.empty( ) );
         }
      else if ( e == DiskQueueExceptions::FullDiskQueue )
         {
         std::cout << "Full\n";
         assert( a.full( ) );
         }
      else
         std::cout << "Other issues\n";
      a.PrintStatus( );
      }
      std::cout << "\nContinue pushing 6 urls:\n";
      try
      {
         for ( int i = 0; i < 4; ++i )
            {
            String s;
            sin >> s;
            a.PushBack( s );
            }
      }catch( DiskQueueExceptions& e )
      {
      if ( e == DiskQueueExceptions::EmptyDiskQueue )
         {
         std::cout << "Empty\n";
         assert( a.empty( ) );
         }
      else if ( e == DiskQueueExceptions::FullDiskQueue )
         {
         std::cout << "Full\n";
         assert( a.full( ) );
         }
      else
         std::cout << "Other issues\n";
      a.PrintStatus( );
      }
      a.PrintStatus();
      sin.close( );
   }