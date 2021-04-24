#include <iostream>
#include "RankerManager.h"
#include "mString.h"
#include "Vector.h"

const int port = 8888;

int main( )
    {
    std::string query1 = "Google | Facebook", query2 = "amazon", query3 = "( google wikipedia ) | amazon";
    QueryServer qServer;
    try
        {
        ::vector< url_score > res1 = qServer.CollectRanks( query1 );
        std::cout << "Results for query=" << query1 << std::endl;
        printRanks( res1 );
        ::vector< url_score > res2 = qServer.CollectRanks( query1 );
        std::cout << "Results for query=" << query2 << std::endl;
        printRanks( res2 );
        ::vector< url_score > res3 = qServer.CollectRanks( query1 );
        std::cout << "Results for query=" << query3 << std::endl;
        printRanks( res3 );
        }
    catch( const char *e )
        {
        std::cout << e << std::endl;
        }
    catch( std::string& es )
        {
        std::cout << es << std::endl;
        }
    
    }