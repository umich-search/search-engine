// This header file defines custom version of std::unique_ptr and std::shared_ptr
// UniquePointer guarantees single reference of any type of memory
// when UniquePointer is a rvalue, it can be assigned to SharedPointer
// SharedPointer allows multiple refernce to a type of memory by retaining a "count"
// variable. Only the last call of dtor will deallocate the memory.

#pragma once

#include <cstddef>

template< class Type >
void Swap( Type& x, Type& y ) noexcept
    {
    Type tmp = x;
    x = y;
    y = tmp;
    }

// unique_ptr
template< class Type >
class UniquePointer
    {
    private:
        Type *memPtr;

    public:
        // explicit constructor
        explicit UniquePointer( Type *ptr = nullptr ) : memPtr( ptr )
            {
            }

        // disable copy constructor
        UniquePointer( const UniquePointer &other ) = delete;

        // move constructor
        UniquePointer( UniquePointer&& other ) noexcept
            : memPtr( nullptr )
            {
            this->swap( other );
            }

        // disable assignment operator
        UniquePointer& operator=( const UniquePointer& other ) = delete;

        // move assignment operator
        UniquePointer& operator=( UniquePointer&& other ) noexcept
            {
            this->swap( other );
            other.memPtr = nullptr;
            return *this;
            }
        
        // destructor
        ~UniquePointer( ) noexcept
            {
            delete memPtr;
            }


        // Modifers
        Type *release( ) noexcept
            {
            Type *tmp = memPtr;
            memPtr = nullptr;
            return tmp;
            }

        void reset( Type *newPtr = nullptr ) noexcept
            {
            Type *oldPtr = memPtr;
            memPtr = newPtr;
            if ( oldPtr )
                delete oldPtr;
            }

        void swap( UniquePointer& other ) noexcept
            {
            Swap( memPtr, other.memPtr );
            }

        
        // Observers
        Type *get( ) const noexcept
            {
            return memPtr;
            }

        explicit operator bool ( ) const noexcept
            {
            return memPtr != nullptr;
            }

        Type *operator-> ( ) const noexcept
            {
            return memPtr;
            }

        // may throw  exceptions if Type defines improper *
        Type& operator* ( ) const
            {
            return *memPtr;
            }

    };

// UniquePointer comparison functions
template< class Type1, class Type2 >
bool operator== ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return x.get( ) == y.get( );
    }

template< class Type1, class Type2 >
bool operator!= ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return !( x == y );
    }

template< class Type1, class Type2 >
bool operator< ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return x.get( ) < y.get( );
    }

template< class Type1, class Type2 >
bool operator> ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return y < x;
    }

template< class Type1, class Type2 >
bool operator<= ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return !( y < x );
    }

template< class Type1, class Type2 >
bool operator>= ( const UniquePointer< Type1 >& x, const UniquePointer< Type2 >& y )
    {
    return !( x < y );
    }

template< class Type >
bool operator== ( const UniquePointer< Type >& x, std::nullptr_t ) noexcept
    {
    return !x;
    }

template< class Type >
bool operator== ( std::nullptr_t, const UniquePointer< Type >& x ) noexcept
    {
    return !x;
    }

template< class Type >
bool operator!= ( const UniquePointer< Type >& x, std::nullptr_t ) noexcept
    {
    return bool( x );
    }

template< class Type >
bool operator!= ( std::nullptr_t, const UniquePointer< Type >& x ) noexcept
    {
    return bool( x );
    }


// shared_ptr
template< class Type >
class SharedPointer
    {
    private:
        Type *memPtr;
        int *count;
    public:
        // explict construction
        explicit SharedPointer( Type *ptr = nullptr )
        try  // in case of allocation failure ( not enough space )
            : memPtr( ptr ), count( nullptr )
            {
            if ( memPtr )
                count = new int ( 1 );
            }
        catch ( ... )
            {
            delete memPtr;
            throw "Bad alloc!";
            }
                

        // copyt constructor
        SharedPointer( const SharedPointer& other )
            : memPtr( other.memPtr ), count( other.count )
            {
            if ( count )
                ++ *( count );
            }

        // move constructor
        SharedPointer( SharedPointer&& other ) noexcept
            : memPtr( nullptr ), count( nullptr )
            {
            this->swap( other );
            }

        // assignment operator
        SharedPointer& operator= ( const SharedPointer& other )
            {
            SharedPointer tmp ( other );
            this->swap( tmp );
            return *this;
            }

        // move assignment operator
        SharedPointer& operator= ( SharedPointer&& other ) noexcept
            {
            this->swap( other );
            other.memPtr = nullptr;
            other.count = nullptr;
            return *this;
            }

        // assignment operator with pointer
        SharedPointer& operator= ( Type *ptr )
            {
            SharedPointer tmp ( ptr );
            this->swap( tmp );
            return *this;
            }

        // assignment operator with UniquePointer
        SharedPointer& operator= ( UniquePointer< Type >&& uPtr )
            {
            SharedPointer tmp( uPtr.release( ) );
            this->swap( tmp );
            return *this;
            }

        // destructor
        ~SharedPointer ( ) noexcept
            {
            if ( count )
                {
                -- *( count );
                if ( *count == 0 )
                    {
                    delete memPtr;
                    delete count;
                    }
                }
            }

        // Modifiers 
        void reset( )
            {
            SharedPointer tmp;
            this->swap( tmp );
            }

        void swap( SharedPointer& other ) noexcept
            {
            Swap( memPtr, other.memPtr );
            Swap( count, other.count );
            }

        // Observers
        Type *get( ) const noexcept
            {
            return memPtr;
            }

        explicit operator bool ( ) const noexcept
            {
            return memPtr != nullptr;
            }

        Type *operator-> ( ) const noexcept
            {
            return memPtr;
            }

        Type& operator* ( ) const
            {
            return *memPtr;
            }

        int numCount( ) const noexcept
            {
            return *count;
            }
    };

// SharedPointer comparison functions
template< class Type1, class Type2 >
bool operator== ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return x.get( ) == y.get( );
    }

template< class Type1, class Type2 >
bool operator!= ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return !( x == y );
    }

template< class Type1, class Type2 >
bool operator< ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return x.get( ) < y.get( );
    }

template< class Type1, class Type2 >
bool operator> ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return y < x;
    }

template< class Type1, class Type2 >
bool operator<= ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return !( y < x );
    }

template< class Type1, class Type2 >
bool operator>= ( const SharedPointer< Type1 >& x, const SharedPointer< Type2 >& y )
    {
    return !( x < y );
    }

template< class Type >
bool operator== ( const SharedPointer< Type >& x, std::nullptr_t ) noexcept
    {
    return !x;
    }

template< class Type >
bool operator== ( std::nullptr_t, const SharedPointer< Type >& x ) noexcept
    {
    return !x;
    }

template< class Type >
bool operator!= ( const SharedPointer< Type >& x, std::nullptr_t ) noexcept
    {
    return bool( x );
    }

template< class Type >
bool operator!= ( std::nullptr_t, const SharedPointer< Type >& x ) noexcept
    {
    return bool( x );
    }
