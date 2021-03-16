#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

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
   // find the substring "substr" in [ start, end ) and return the next out of range char
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

class ParsedResponseHeader
   {
   private:
      char *responseHeader;
      
   public:
      char *redirectUrl;
      int status;
      size_t headerLen, urlLen;
      bool chunked;

      ParsedResponseHeader( const char *header )
         : responseHeader( nullptr ), redirectUrl( nullptr ), 
         status( 0 ), headerLen( 0 ), urlLen( 0 ), chunked( false )
         {
         headerLen = strlen( header );
         responseHeader = new char [ headerLen + 1 ];
         strncpy( responseHeader, header, headerLen );
         responseHeader[ headerLen ] = 0;

         const char *statStart = responseHeader;
         if ( !strncmp( responseHeader, "HTTP", 4 ) )
            while ( !isspace( *statStart ) ) statStart++;
         status = atoi( statStart );
         // redirects
         if ( status >= 300 && status < 310 )
            {
            const char *ptr = findSubstring( responseHeader, responseHeader + headerLen, 
               "\r\nLocation:", strlen( "\r\nLocation:" ) );
            while ( isspace( *ptr ) ) ptr++;
            const char *ePtr = ptr;
            while( strncmp( ePtr, "\r\n", 2 ) ) ePtr++;
            urlLen = ePtr - ptr;
            redirectUrl = new char [ urlLen + 1 ];
            strncpy( redirectUrl, ptr, urlLen );
            redirectUrl[ urlLen ] = 0;
            }
         // chunked transferEncoding
         const char *ptr = findSubstring( responseHeader, responseHeader + headerLen,
            "\r\nTransfer-Encoding:", strlen( "\r\nTransfer-Encoding:" ) );
         while ( isspace( *ptr ) ) ptr++;
         if ( !strncmp( ptr, "chunked", strlen( "chunked" ) ) )
            chunked = true;
         }

         ~ParsedResponseHeader( )
            {
            if ( !redirectUrl )
               delete [] redirectUrl;
            delete [] responseHeader;
            }
   };

unsigned int htoi( const char *ptr )
   {
   // convert the first encountered hex number to unsigned int
   // preceding the number can only be white spaces
   const char *start = ptr, *end;
   unsigned int result = 0;

   while ( !isalnum( *start ) ) ++start;
   end = start;
   while ( isalnum( *end ) ) ++end;
   for ( ; start < end; ++start, result = result << 4 )
      {
      if ( isdigit( *start ) )
         result += *start - '0';
      else if ( *start >= 'a' && *start <= 'f' )
         result += *start - 'a' + 10;
      else if ( *start >= 'A' && *start <= 'F' )
         result += *start - 'A' + 10;
      else
         break;
      }
   return result >> 4;
   }

std::string retrieveWebpage( const char *curl )
   {
   // Parse the URL
   ParsedUrl url( curl );

   // Get the host address.
   struct addrinfo *address, hints;
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   getaddrinfo( url.Host, "443", &hints, &address );
   // freeaddrinfo( &hints );

   // Create a TCP/IP socket.
   int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

   // Connect the socket to the host address.
   int connectResult = connect( socketFD, address->ai_addr, address->ai_addrlen );

   // set ssl library
   SSL_library_init( );
   SSL_CTX *ctx = SSL_CTX_new( SSLv23_method( ) );
   SSL *ssl = SSL_new( ctx );
   SSL_set_fd( ssl, socketFD );

   SSL_connect( ssl );

   // Send a GET message.
   std::string message = "GET /";
   message += url.Path;
   message += " HTTP/1.1\r\nHost: "; 
   message += std::string(url.Host);
   message += "\r\nUser-Agent: LinuxGetUrl/2.0 ziyuanh@umich.edu (Linux)\r\nAccept: */*\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";
   SSL_write( ssl, message.c_str( ), message.length( ) );
   
   // Read from the socket until there's no more data, copying it to
   // stdout.
   std::string doc, header;
   const int bufferLength = 10240;
   char buffer[ bufferLength ];
   int bytes, cumsum = 0;

   // read header
   while ( ( bytes = SSL_read( ssl, buffer + cumsum, bufferLength - cumsum ) ) > 0 )
      {
      const char *matchNext = findSubstring( buffer + cumsum - 4, buffer + cumsum + bytes, "\r\n\r\n", 4 );
      if ( matchNext  > buffer + cumsum - 4 )
         {
         doc = std::string( matchNext, buffer + cumsum + bytes - matchNext );
         header = std::string( buffer, matchNext - buffer );
         // write( 1, matchNext, buffer + cumsum + bytes - matchNext );
         break;
         }
      else  
         cumsum += bytes;
      }

   while ( ( bytes = SSL_read( ssl, buffer, bufferLength ) ) > 0 )
      doc += std::string( buffer, bytes );
      // write( 1, buffer, bytes );

   // Close the socket and free the address info structure.
   SSL_shutdown( ssl );
   SSL_free( ssl );
   SSL_CTX_free( ctx );
   close( socketFD );
   freeaddrinfo( address );

   ParsedResponseHeader parsedHeader ( header.c_str( ) );
   if ( parsedHeader.redirectUrl ) 
      return retrieveWebpage( parsedHeader.redirectUrl );

   if ( parsedHeader.chunked )
      {
      std::string newDoc;
      const char *docPtr = doc.c_str( );
      for ( unsigned int chunkSize = htoi( docPtr ); chunkSize > 0; chunkSize = htoi( docPtr ) )
         {
         // move docPtr to the real text
         while ( isalnum( *docPtr ) ) docPtr++;
         docPtr += 2;
         newDoc += std::string( docPtr, chunkSize );
         docPtr += chunkSize;
         while ( !isalnum( *docPtr ) ) docPtr++;
         }
      return newDoc;
      }

   return doc;
   }

int main( int argc, char **argv )
   {

   if ( argc != 2 )
      {
      std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
      return 1;
      }

   std::cout << retrieveWebpage( argv[ 1 ] );

   }
