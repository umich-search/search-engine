#include "ranker.h"

::vector<url_score> getHighest(::vector<Match*>* matches, ISR* queryRoot)
    {
    for ( int i = 0; i < (*matches).size(); ++i ) 
        {
        Match *document = (*matches)[i];
        if ( queryRoot->GetTermNum() == 0 ) getDynamic( document, queryRoot, true ); // only searching for a word
        else getDynamic( document, queryRoot, false ); // searching for an abstract ISR
        }

    }

int getDynamic(Match* document, ISR* queryRoot, bool wordToPhrase)
    {
    if ( queryRoot->GetTermNum() == 0 && !wordToPhrase ) // reached leaf node
        {
        return 0;
        }
    else if ( queryRoot->GetTermNum() > 0 ) // at abstract ISR
        {
        // see if there is a non-word ISR. If yes, then turn the wordISRs to phraseISR
        bool abstractISRExists = false;
        for ( int i = 0; i < queryRoot->GetTermNum(); ++i )
            {
            if ((*( queryRoot->GetTerms() + i ))->GetTermNum() > 0) abstractISRExists = true;
            }
        // this ISR has only WordISRs, calculate score according to heuristics
        if ( !abstractISRExists ) return queryRoot->GetHeuristicScore(document);

        }
    }