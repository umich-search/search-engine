#pragma once

// Plugin interface for the LinuxTinyServer.

#include <string>

class PluginObject
   {
   public:
      // MagicPath returns true if this is a path the plugin
      // intercepts.

      virtual bool MagicPath( const std::string path ) = 0;

      // The request passed to ProcessRequest is the raw contents
      // of the HTTP request as read from the socket (up to the
      // 10K limit currently imposed by LinuxTinyServer.)

      // The c_str( ) contents of whatever is returned will be
      // written unchanged to the socket (and to the client)
      // and should start with a proper HTTP header.

      // ProcessRequest is expected to be thread-safe, e.g., by
      // using Mutex.h to create a lock to ensure that only one
      // thread at a time can procede where necessary for correct
      // operation.  (Alterately, might create a derived
      // SingledThreadedPluginObject that does this.)

      // (Might consider a variation that also passes the talk
      // socket for a ProcessRequest routine that needs more than
      // just the initial 10K or wants to do its own writing.)

      virtual std::string ProcessRequest( std::string action, std::string path ) = 0;

      virtual ~PluginObject( )
         {
         }
   };

 // The constructor for any plugin should set Plugin = this
 // so that LinuxTinyServer knows it exists and can call it.

extern PluginObject *Plugin;
