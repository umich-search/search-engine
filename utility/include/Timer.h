#pragma once

// A simple timer that can measure the number of clock ticks elapsed.
// Modified from Nicole's simple Timer.h

#include <iostream>
#include <chrono>
#include "Concurrency.h"

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

      void Reset( )
         {
         start = std::chrono::high_resolution_clock::now( );
         }

      unsigned long Elapsed( ) const
         {
         return ( std::chrono::high_resolution_clock::now( ) - start ).count( );
         }

      unsigned long ElapsedSeconds() const
         {
         return std::chrono::duration_cast<std::chrono::seconds>( std::chrono::high_resolution_clock::now( ) - start ).count(); 
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

class ThreadSafeTimer
   {
   private:
      Timer timer;
      mutex_t mutex;

   public:
      ThreadSafeTimer( )
         {
         pthread_mutex_init(&mutex, nullptr);
         }

      void Start( )
         {
         CriticalSection s(&mutex);
         timer.Start();
         }

      void Finish( )
         {
         CriticalSection s(&mutex);
         timer.Finish();
         }

      void Reset( )
         {
         CriticalSection s(&mutex);
         timer.Reset();
         }

      unsigned long Elapsed( )
         {
         CriticalSection s(&mutex);
         return timer.Elapsed();
         }

      unsigned long ElapsedSeconds( )
         {
         CriticalSection s(&mutex);
         return timer.ElapsedSeconds();
         }
   };
