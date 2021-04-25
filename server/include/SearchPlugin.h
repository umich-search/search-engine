#pragma once
#include <cstring>
#include "Plugin.h"
//#include "../../ranker/include/ranker.h"
#include "mString.h"
#include "Concurrency.h"
#include "Vector.h"
#include "RankerManager.h"

// Handle requests for the path /search?query=university+of+michigan

class SearchPlugin : public PluginObject
    {
    public:
        SearchPlugin() 
            {
            }
        ~SearchPlugin() { };

        bool MagicPath( const std::string path )
            {
            return strncmp( path.c_str(), "/search", 7 ) == 0;
            }
        
        std::string ProcessRequest( std::string action, std::string path )
            {
            if ( action != "GET" )
                return "";
            std::string query = queryServer.deserializeQueryMsg( path );
            ::vector<url_score> scores = queryServer.CollectRanks( query );
            std::string html;
            for ( size_t i = 0; i < scores.size(); ++i )
                {
                std::string result = "<div class=\"resultTag\"><h4><a href=\"";
                result += scores[i].URL;
                result += "\">";
                result += scores[i].title;
                result += "</a></h4><p>";
                result += scores[i].URL;
                result += "</p></div>";
                html += result;
                }
            return html;
            }

    private:
        QueryServer queryServer;
    };