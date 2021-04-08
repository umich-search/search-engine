#pragma once

// A simple timer that can measure the number of clock ticks
// between a start and a finish.

// Nicole Hamilton  nham@umich.edu


#include <iostream>
#include <chrono>

class Timer
   {
   private:

      std::chrono::high_resolution_clock::time_point start, finish;

   public:

      void Start( )
         {
         start = std::chrono::high_resolution_clock::now( );
         }

      void Finish( )
         {
         finish = std::chrono::high_resolution_clock::now( );
         }

      unsigned long Elapsed( )
         {
         return ( finish - start ).count( );
         }

      void PrintElapsed( )
         {
         std::cout << "Elapsed time = " << Elapsed( ) <<
            " ticks" << std::endl << std::endl;
         }
   };