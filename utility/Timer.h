#pragma once

// A simple timer that can measure the number of clock ticks elapsed.
// Modified from Nicole's simple Timer.h

#include <iostream>
#include <chrono>

class Timer
   {
   private:

      std::chrono::high_resolution_clock::time_point start;

   public:

      void Reset( )
         {
         start = std::chrono::high_resolution_clock::now( );
         }

      unsigned long Elapsed( ) const
         {
         return ( std::chrono::high_resolution_clock::now( ) - start ).count( );
         }

      bool operator< ( const Timer& other ) const
         {
         return Elapsed( ) < other.Elapsed( );
         }

      bool operator<= ( const Timer& other ) const
         {
         return Elapsed( ) <= other.Elapsed( );
         }

      bool operator> ( const Timer& other ) const
         {
         return !( *this <= other );
         }

      bool operator>= ( const Timer& other ) const
         {
         return !( *this < other );
         }

      void PrintElapsed( )
         {
         std::cout << "Elapsed time = " << Elapsed( ) <<
            " ticks" << std::endl << std::endl;
         }
   };