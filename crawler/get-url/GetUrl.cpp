#include "./GetUrl.h"

const char * findSubstring( const char *start, const char *end, const char *substr, size_t strLen )
   {
   const char *sptr = start; 
   size_t ind = 0;
   for ( ;  sptr < end && ind < strLen; ++sptr )
      {
      if ( *sptr == *( substr + ind ) )
         ++ind;
      else 
         ind = 0;
      }
   if ( ind < strLen )
      return start;
   return sptr;
   }

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

String LinuxGetUrl( const String& sUrl )
   {
   ParsedUrl url( sUrl.cstr( ) );
   return LinuxGetUrl( url );
   }

String LinuxGetUrl( const ParsedUrl& url )
   {
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
   String message = "GET /";
   message += url.Path;
   message += " HTTP/1.1\r\nHost: "; 
   message += String( url.Host );
   message += "\r\nUser-Agent: LinuxGetUrl/2.0 ziyuanh@umich.edu (Linux)\r\nAccept: */*\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";

   // write( 1, message.c_str(), message.length() );

   send( socketFD, message.cstr( ), message.size( ), 0 );

   // Read from the socket until there's no more data, copying it to
   // stdout.

   String doc, header;
   const int bufferLength = 10240;  // greater than the largest header
   char buffer[ bufferLength ];
   int bytes, cumsum = 0;

   // read header
   while ( ( bytes = recv( socketFD, buffer + cumsum, bufferLength - cumsum, 0 ) ) > 0 )
      {
      const char *matchNext = findSubstring( buffer + cumsum - 4, buffer + cumsum + bytes, "\r\n\r\n", 4 );
      if ( matchNext  > buffer + cumsum - 4 )
         {
         doc = String( matchNext, buffer + cumsum + bytes - matchNext );
         header = String( buffer, matchNext - buffer );
         // write( 1, matchNext, buffer + cumsum + bytes - matchNext );
         break;
         }
      else  
         cumsum += bytes;
      }

   while ( ( bytes = recv( socketFD, buffer, bufferLength, 0 ) ) > 0 )
      doc += String( buffer, bytes );

   // Close the socket and free the address info structure.
   close( socketFD );
   freeaddrinfo( address );

   ParsedResponseHeader parsedHeader ( header.cstr( ) );
   if ( parsedHeader.redirectUrl )
      {
      ParsedUrl url( parsedHeader.redirectUrl );
      if ( !strncmp( url.Service, "http", 4 ) )
         return LinuxGetUrl( url );
      else if ( !strncmp( parsedHeader.redirectUrl, "https", 5 ) )
         return LinuxGetSsl( url );
      else
         std::cerr << "Don't support protocol " << url.Service << std::endl;
      }

   if ( parsedHeader.chunked )
      {
      String newDoc;
      const char *docPtr = doc.cstr( );
      for ( unsigned int chunkSize = htoi( docPtr ); chunkSize > 0; chunkSize = htoi( docPtr ) )
         {
         // move docPtr to the real text
         while ( isalnum( *docPtr ) ) docPtr++;
         docPtr += 2;
         newDoc += String( docPtr, chunkSize );
         docPtr += chunkSize;
         while ( !isalnum( *docPtr ) ) docPtr++;
         }
      return newDoc;
      }

   return doc;
   }

String LinuxGetSsl( const String& sUrl )
   {
   ParsedUrl url( sUrl.cstr( ) );
   return LinuxGetSsl( url );
   }

String LinuxGetSsl( const ParsedUrl& url )
   {
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
   String message = "GET /";
   message += url.Path;
   message += " HTTP/1.1\r\nHost: "; 
   message += String( url.Host );
   message += "\r\nUser-Agent: LinuxGetUrl/2.0 ziyuanh@umich.edu (Linux)\r\nAccept: */*\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";
   SSL_write( ssl, message.cstr( ), message.size( ) );
   
   // Read from the socket until there's no more data, copying it to
   // stdout.
   String doc, header;
   const int bufferLength = 10240;
   char buffer[ bufferLength ];
   int bytes, cumsum = 0;

   // read header
   while ( ( bytes = SSL_read( ssl, buffer + cumsum, bufferLength - cumsum ) ) > 0 )
      {
      const char *matchNext = findSubstring( buffer + cumsum - 4, buffer + cumsum + bytes, "\r\n\r\n", 4 );
      if ( matchNext  > buffer + cumsum - 4 )
         {
         doc = String( matchNext, buffer + cumsum + bytes - matchNext );
         header = String( buffer, matchNext - buffer );
         // write( 1, matchNext, buffer + cumsum + bytes - matchNext );
         break;
         }
      else  
         cumsum += bytes;
      }

   while ( ( bytes = SSL_read( ssl, buffer, bufferLength ) ) > 0 )
      doc += String( buffer, bytes );
      // write( 1, buffer, bytes );

   // Close the socket and free the address info structure.
   SSL_shutdown( ssl );
   SSL_free( ssl );
   SSL_CTX_free( ctx );
   close( socketFD );
   freeaddrinfo( address );

   ParsedResponseHeader parsedHeader ( header.cstr( ) );
   if ( parsedHeader.redirectUrl )
      {
      ParsedUrl url( parsedHeader.redirectUrl );
      if ( !strncmp( url.Service, "http", 4 ) )
         return LinuxGetUrl( url );
      else if ( !strncmp( parsedHeader.redirectUrl, "https", 5 ) )
         return LinuxGetSsl( url );
      else
         std::cerr << "Don't support protocol " << url.Service << std::endl;
      }

   if ( parsedHeader.chunked )
      {
      String newDoc;
      const char *docPtr = doc.cstr( );
      for ( unsigned int chunkSize = htoi( docPtr ); chunkSize > 0; chunkSize = htoi( docPtr ) )
         {
         // move docPtr to the real text
         while ( isalnum( *docPtr ) ) docPtr++;
         docPtr += 2;
         newDoc += String( docPtr, chunkSize );
         docPtr += chunkSize;
         while ( !isalnum( *docPtr ) ) docPtr++;
         }
      return newDoc;
      }

   return doc;
   }
   