// vector

#pragma once 

#include <cstddef>
#include <utility>

class VectorIndexOutOfBoundsException {};

template <typename T> class vector
   {
   public:
      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs an empty vector with capacity 0
      vector() :  arr ( nullptr ), _size ( 0 ), _capacity ( 0 ) {}

      // Destructor
      // REQUIRES: Nothing
      // MODIFIES: Destroys *this
      // EFFECTS: Performs any neccessary clean up operations
      ~vector()
         {
         delete[] arr;
         }

      // Resize Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs a vector with size num_elements,
      //    all default constructed
      vector ( size_t num_elements ) :
         _size ( num_elements ),
         _capacity ( num_elements ),
         arr ( new T[num_elements] )
         {
         }

      // Fill Constructor
      // REQUIRES: Capacity > 0
      // MODIFIES: *this
      // EFFECTS: Creates a vector with size num_elements, all assigned to val
      vector ( size_t num_elements, const T& val ) :
         _size ( num_elements ),
         _capacity ( num_elements ),
         arr ( new T[num_elements] )
         {
         for ( size_t i = 0; i < _size; ++i )
            arr[i] = val;
         }

      // Copy Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates a clone of the vector other
      vector ( const vector<T>& other ) :
         _size ( other._size ),
         _capacity ( other._capacity ),
         arr ( new T[other._capacity] )
         {
         for ( size_t i = 0; i < _size; ++i )
            arr[i] = other[i];
         }

      // Assignment operator
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Duplicates the state of other to *this
      vector operator= ( const vector<T>& other )
         {
         if ( this != &other )
            {
            delete[] arr;
            _size = other._size;
            _capacity = other._capacity;
            arr = new T[_capacity];
            for ( size_t i = 0; i < _size; ++i )
               arr[i] = other[i];
            }
         return *this;
         }

      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves other in a default constructed state
      // EFFECTS: Takes the data from other into a newly constructed vector
      vector ( vector<T>&& other ) :
         _size ( std::move( other._size ) ),
         _capacity ( std::move( other._capacity ) ),
         arr ( std::move( other.arr ) )
         {
         other._size = 0;
         other._capacity = 0;
         other.arr = nullptr;
         }

      // Move Assignment Operator
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves otherin a default constructed state
      // EFFECTS: Takes the data from other in constant time
      vector operator= ( vector<T>&& other )
         {
         if ( this != &other )
            {
            delete[] arr;
            _size = std::move( other._size );
            _capacity = std::move( other._capacity );
            arr = std::move( other.arr );
            }
         return *this;
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void reserve ( size_t newCapacity )
         {
         if ( newCapacity <= _capacity )
            return;
         _capacity = newCapacity;
         T* new_arr = new T[_capacity];
         for ( int i = 0; i < _size; ++i )
            new_arr[i] = arr[i];
         delete[] arr;
         arr = new_arr;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of elements in the vector
      size_t size() const
         {
         return _size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the maximum size the vector can attain before resizing
      size_t capacity() const
         {
         return _capacity;
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Allows modification of data[i]
      // EFFECTS: Returns a mutable reference to the i'th element
      T& operator[] ( size_t i )
         {
         if ( i < 0 || i >= _size )
            throw VectorIndexOutOfBoundsException{};
         return arr[i];
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Nothing
      // EFFECTS: Get a const reference to the ith element
      const T& operator[] ( size_t i ) const
         {
         if ( i < 0 || i >= _size )
            throw VectorIndexOutOfBoundsException{};
         return arr[i];
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size(), capacity()
      // EFFECTS: Appends the element x to the vector, allocating
      //    additional space if neccesary
      void pushBack ( const T& x )
         {
         if ( _size == _capacity )
            {
            _capacity = _capacity == 0 ? 1 : _capacity * 2;
            T* new_arr = new T[_capacity];
            for ( size_t i = 0; i < _size; ++i )
               new_arr[i] = arr[i];
            delete[] arr;
            arr = new_arr;
            }
         arr[_size] = x;
         ++_size;
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size()
      // EFFECTS: Removes the last element of the vector,
      //    leaving capacity unchanged
      void popBack()
         {
         if ( _size > 0 )
            --_size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to the
      //    first element of the vector
      T* begin()
         {
         return &arr[0];
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to
      //    one past the last valid element of the vector
      T* end()
         {
         return &arr[_size];
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the first element of the
      // vector
      const T* begin() const
         {
         return &arr[0];
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to
      //    one past the last valid element of the vector
      const T* end() const
         {
         return &arr[_size];
         }

   private:
      T* arr;
      size_t _size;
      size_t _capacity;
   };
