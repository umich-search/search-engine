#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <iostream>

class ParsedUrl
   {
   public:
      const char *CompleteUrl;
      char *Service, *Host, *Port, *Path;

      ParsedUrl( const char *url )
         {
         // Assumes url points to static text but
         // does not check.

         CompleteUrl = url;

         pathBuffer = new char[ strlen( url ) + 1 ];
         const char *f;
         char *t;
         for ( t = pathBuffer, f = url; *t++ = *f++; )
            ;

         Service = pathBuffer;

         const char Colon = ':', Slash = '/';
         char *p;
         for ( p = pathBuffer; *p && *p != Colon; p++ )
            ;

         if ( *p )
            {
            // Mark the end of the Service.
            *p++ = 0;

            if ( *p == Slash )
               p++;
            if ( *p == Slash )
               p++;

            Host = p;

            for ( ; *p && *p != Slash && *p != Colon; p++ )
               ;

            if ( *p == Colon )
               {
               // Port specified.  Skip over the colon and
               // the port number.
               *p++ = 0;
               Port = +p;
               for ( ; *p && *p != Slash; p++ )
                  ;
               }
            else
               Port = p;

            if ( *p )
               // Mark the end of the Host and Port.
               *p++ = 0;

            // Whatever remains is the Path.
            Path = p;
            }
         else
            Host = Path = p;
         }

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
   };

const char * findSubstring( const char *start, const char *end, const char *substr, size_t strLen )
   {
   const char *sptr = start; 
   size_t ind = 0;
   for ( ;  sptr < end && ind < strLen; )
      {
      if ( *sptr == *( substr + ind ) )
         {
         ++sptr;
         ++ind;
         }
      else 
         {
         ++sptr;
         ind = 0;
         }
      }
   if ( ind < strLen )
      return start;
   return sptr;
   }

int main( int argc, char **argv )
   {

   if ( argc != 2 )
      {
      std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
      return 1;
      }

   // Parse the URL
   ParsedUrl url( argv[ 1 ] );
   // std::cout << url.CompleteUrl << std::endl;

   // Get the host address.
   struct addrinfo *address, hints;
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   getaddrinfo( url.Host, "80", &hints, &address );
   // freeaddrinfo( &hints );

   // Create a TCP/IP socket.
   int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

   // Connect the socket to the host address.
   int connectResult = connect( socketFD, address->ai_addr, address->ai_addrlen );

   // Send a GET message.
   std::string message = "GET /";
   message += url.Path;
   message += " HTTP/1.1\r\nHost: "; 
   message += std::string(url.Host);
   message += "\r\nUser-Agent: LinuxGetUrl/2.0 ziyuanh@umich.edu (Linux)\r\nAccept: */*\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";

   // write( 1, message.c_str(), message.length() );

   send( socketFD, message.c_str( ), message.length( ), 0 );

   // Read from the socket until there's no more data, copying it to
   // stdout.
   const int bufferLength = 10240;
   char buffer[ bufferLength ];
   int bytes, cumsum = 0;

   // read header
   while ( ( bytes = recv( socketFD, buffer + cumsum, bufferLength - cumsum, 0 ) ) > 0 )
      {
      const char *matchNext = findSubstring( buffer + cumsum - 4, buffer + cumsum + bytes, "\r\n\r\n", 4 );
      if ( matchNext  > buffer + cumsum - 4 )
         {
         write( 1, matchNext, buffer + cumsum + bytes - matchNext );
         break;
         }
      else  
         cumsum += bytes;
      }
   
   while ( ( bytes = recv( socketFD, buffer, sizeof( buffer ), 0 ) ) > 0 )
      write( 1, buffer, bytes );

   // Close the socket and free the address info structure.
   close( socketFD );
   freeaddrinfo( address );

   }
