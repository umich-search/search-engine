#include "ranker.h"
#include "Dictionary.h"
#include "ISRSpan.h"
#include "constraint_solver.h"


std::string serializeUrlScore( url_score *us )
    {
    // [URL]$[title]#[score]@
    return us->URL + '$' + 
        us->title + '#' + 
        ltos( us->score ).cstr( ) + '@';
    }

void freeResults( ::vector< url_score * >& v )
    {
    while( v.size( ) > 0 )
        {
        delete v.end( );
        v.popBack( );
        }
    }

void printRanks( ::vector< url_score >& v )
    {
    for ( int i = 0; i < v.size( ); ++i )
        std::cout << v[ i ].URL << '\n' 
                << v[ i ].title << '\n' 
                << v[ i ].score << std::endl << std::endl;
    }

float getDynamic(Match* document, ISR* queryRoot);

vector<url_score> Ranker::getHighest(::vector<Match*>* matches, ISR* queryRoot)
    {
    vector<url_score> arr;
    for ( size_t i = 0; i < (*matches).size(); ++i ) 
        {
        Match *document = (*matches)[i];
        float staticScore = calculate_static_scores( document );;
        //if ( queryRoot->GetTermNum() == 0 ) getDynamic( document ); // only searching for a word
        float dynScore = getDynamic( document, queryRoot); // searching for an abstract ISR
        float totalScore = dynScore * dynamicWeight + staticScore * staticWeight;
        String url, title; 
        Dictionary *dict = new Dictionary(0);
        DocumentDetails *docs = dict->GetDocumentDetials(document->id);
        url = docs->url;
        title = docs->title;
        url_score newDoc( url, title, totalScore );
        delete docs;
        delete dict;
        // insertion sort
        if ( arr.size() == 0 ) arr.pushBack( newDoc );
        else if ( arr.size() < N ) 
            {
            int j = arr.size() - 1;
            while ( j >= 0 && arr[ j ].score < totalScore )
                {
                if ( j == arr.size() - 1 ) arr.pushBack(arr[j]);
                else arr[j + 1] = arr[j];
                --j;
                }
            if ( j + 1 < arr.size() ) arr[j + 1] = newDoc;
            else arr.pushBack( newDoc );
            }
        else if ( arr.size() >= N )
            {
            int j = N - 1;
            while ( j >=0 && arr[ j ].score < totalScore )
                {
                if( j < N - 1 ) arr[ j + 1 ] = arr[ j ];
                --j;
                }
            if ( j + 1 < N ) arr[j + 1] = newDoc;
            }
        
        }
    return arr;
    }

float getDynamic(Match* document, ISR* queryRoot)
    {
    bool abstractISRExists = false;
    for ( int i = 0; i < queryRoot->GetTermNum(); ++i )
        {
        std::cout << (*( queryRoot->GetTerms() + i ))->GetTermNum()<<std::endl;
        if ((*( queryRoot->GetTerms() + i ))->GetTermNum() > 0) 
            {
            abstractISRExists = true;
            break;
            }
        }
    // this ISR has only WordISRs, calculate score according to heuristics
    if ( !abstractISRExists ) return queryRoot->GetHeuristicScore(document);
    else 
        {
        vector<float> scores;
        for (int i = 0; i < queryRoot->GetTermNum(); ++i) 
            {
            ISR *root = *( queryRoot->GetTerms() + i );
            if ( root->GetTermNum() > 0 ) // abstract ISR
                {
                scores.pushBack( getDynamic(document, root) );
                }
            else // treat the ISRWord as ISRPhrase
                {
                scores.pushBack(root->GetHeuristicScore(document));
                }
            }
        return queryRoot->GetCombinedScore(scores);
        }
    }


void freeResults( ::vector< url_score * >& v )
    {
    while( v.size( ) > 0 )
        {
        delete v.end( );
        v.popBack( );
        }
    }
