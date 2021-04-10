// Implementation of Queue and PriorityQueue using MaxHeap ( from EECS281 slides )

#pragma once
#include "Vector.h"
#include <iostream>
#include "Common.h"

template< typename T >
class PriorityQueue
    {
    // Heap implementation of the priority queue
    // given a node i: the children of the node are 2 * i + 1 and 2 * i + 2
    // given a node i: the parent of the node is ( i - 1 ) / 2
    private:
        vector< T > heap;

        // fix the heap up starting at the index i
        void fixUp( int i )
            {
            while ( i > 0 && heap[ ( i - 1 ) / 2 ] < heap[ i ] )
                {
                Swap( heap[ ( i - 1 ) / 2 ], heap[ i ] );
                i = ( i - 1 ) / 2;
                }
            }

        // fix the heap down starting at the index i
        void fixDown( int i )
            {
            int child;
            while ( ( child = i * 2 + 1 ) < heap.size( ) )
                {
                if ( child + 1 < heap.size( ) && heap[ child ] < heap[ child + 1 ] )
                    ++child;
                if ( heap[ child ] <= heap[ i ] )
                    break;
                Swap( heap[ child ], heap[ i ] );
                i = child;
                }
            }

    public:
        // Default Constructor
        PriorityQueue( ) { }

        // Copy Constructor
        PriorityQueue( const PriorityQueue& other ) : heap( other.heap )
            {
            }

        // Assignment Operator
        PriorityQueue& operator= ( const PriorityQueue& other )
            {
            heap = other.heap;
            }

        // size
        size_t size( ) const 
            {
            return heap.size( );
            }

        // empty
        bool empty( ) const
            {
            return size( ) == 0;
            }    

        // push one element to the last position of the heap
        // and fix the heap, taking O( logN )
        void Push( const T& x )
            {
            heap.pushBack( x );
            fixUp( heap.size( ) - 1 );
            }

        // pop the root element and fix the heap, taking O( logN )
        void Pop( )
            {
            heap[ 0 ] = heap[ heap.size( ) - 1 ];
            heap.popBack( );
            fixDown( 0 );
            }

        // return the root element
        T& Top( )
            {
            return heap[ 0 ];
            }

        // debug function
        void PrintHeap( )
            {
            for ( int i = 0; i < heap.size( ); )
                {
                // print a line
                int numInLine = heap.size( ) - i < i + 1 ? heap.size( ) - i : i + 1;
                int spaceInLine = heap.size( ) - numInLine;
                int spaceBetween = spaceInLine / ( numInLine + 1 );
                if ( !spaceBetween )
                    spaceBetween = 1;
                for ( int j = 0; j < numInLine; ++j )
                    {
                    // print space
                    for ( int s = 0; s < spaceBetween; ++s )
                        std::cout << ' ';
                    // print the number
                    std::cout << heap[ i++ ];
                    }
                std::cout << std::endl;
                }
            }
        
        // DEBUG fuction
        void PrintHeapSort( )
            {
            PriorityQueue tmp( *this );
            while ( !tmp.empty( ) )
                {
                std::cout << tmp.Top( ) << ' ';
                tmp.Pop( );
                }
            std::cout << std::endl;
            }

    };