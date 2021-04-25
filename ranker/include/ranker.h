#pragma once

#include "Vector.h"
#include "constraint_solver.h"
#include "ISR.h"

const size_t N = 10; // number of url_score that a ranker return
const float dynamicWeight = 0.8;
const float staticWeight = 0.2;

struct url_score {
    std::string URL;
    std::string title;
    float score;
    url_score( ) 
        {
        }
    url_score( String URL, String title, float score ) : URL(URL.cstr( )), title(title.cstr( )), score(score)
        { 
        }
    url_score( std::string URL, std::string title, float score ) : URL(URL), title(title), score(score)
        { 
        }
    // url_score( const url_score& other ) : URL( other.URL ), title( other.title ), score( other.score )
    //     {
    //     }
    // url_score& operator= ( const url_score& other )
    //     {
    //     URL = other.URL;
    //     title = other.title;
    //     score = other.score;
    //     return *this;
    //     }
};


class Ranker {
public:
const float static_weight = 0.1;
const float dynamic_weight = 0.9;
::vector<url_score> getHighest(ConstraintSolver& solver, ISR* queryRoot);
};

std::string serializeUrlScore( url_score *us );
void freeResults( ::vector< url_score * >& v );

void printRanks( ::vector< url_score >& v );
