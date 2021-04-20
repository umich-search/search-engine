#include "ranker.h"

::vector<url_score> getHighest(::vector<Match*>* matches, ISR* queryRoot)
    {
    for ( int i = 0; i < (*matches).size(); ++i ) 
        {
        float totalScore = 0;
        Match *document = (*matches)[i];
        //if ( queryRoot->GetTermNum() == 0 ) getDynamic( document ); // only searching for a word
        float dynScore = getDynamic( document, queryRoot); // searching for an abstract ISR

        }

    }

float getDynamic(Match* document, ISR* queryRoot)
    {
    // if ( queryRoot->GetTermNum() == 0 ) // reached leaf node
    //     {
    //     return 0;
    //     }
    // else if ( queryRoot->GetTermNum() > 0 ) // at abstract ISR
    //     { // assume that a queryRoot cannot start as a wordISR
        // see if there is a non-word ISR. If yes, then turn the wordISRs to phraseISR
        bool abstractISRExists = false;
        for ( int i = 0; i < queryRoot->GetTermNum(); ++i )
            {
            if ((*( queryRoot->GetTerms() + i ))->GetTermNum() > 0) abstractISRExists = true;
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
            return queryRoot->GetCombinedScore(document, scores);
    //        }
    }