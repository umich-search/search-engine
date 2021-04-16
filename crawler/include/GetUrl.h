// compile command
// g++ -std=c++11 ( list of cpp files ) -lssl -lcrypto -lz -o a.out

#pragma once

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

#include <iostream>
#include "mString.h"

const char *findSubstring( const char *, const char *, const char *, size_t );
unsigned int htoi( const char *ptr );

class ParsedUrl
   {
   public:
      const char *CompleteUrl;
      const char *Service, *Host, *Port, *Path;

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

      bool IsOkay()
         {
         return !strncmp("http", Service, 4) || !strncmp("https", Service, 5);
         }

   private:
      char *pathBuffer;
   };

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

// LinuxGetHTML determines service and calls Url/Ssl respectively
String LinuxGetHTML( const ParsedUrl&, size_t numRedirect );
String LinuxGetUrl( const ParsedUrl&, size_t numRedirect );
String LinuxGetSsl( const ParsedUrl&, size_t numRedirect );