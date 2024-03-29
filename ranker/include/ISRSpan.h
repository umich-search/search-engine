#pragma once

#include "Dictionary.h"
#include "ISR.h"
#include "Post.h"
#include "PostingList.h"
#include "Vector.h"
#include "constraint_solver.h"


#define MAXSHORT 10
#define MINFREQUENT 10
#define MINTOP 100
#define MINMOST 0.6
#define MINSOME 0.3

struct Statistics {
    size_t numShortSpans;
    size_t numOrderSpans;
    size_t numPhrases;
    size_t numTopSpans;
    size_t numFrequentWords;
};


struct StaticWeights{
    float weightDomain = 5;
    float weightURL = 2;
    float weightTitle = 3;

};

static const char* DomainsTable[]{
        "org",
        "com",
        "edu",
        "net",
        "io",
        "edu",
        "gov"
};


class ISRSpan {
public:

    ISRSpan(Match *document, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm, struct Weights *weights) 
        : numTerms( numTerms ), positionRarestTerm( positionRarestTerm ),
        Terms( Terms ), document( document ), location( ::vector< Location >( numTerms, 0 ) ), 
        score( 0 ), smallest( 0 ), farthest( 0 ), weights( weights )
        {
        statistics.numPhrases = 0;
        statistics.numOrderSpans = 0;
        statistics.numTopSpans = 0;
        statistics.numShortSpans = 0;
        // statistics.numFrequentWords = 0;
        }

    //Move the rarest term to the next occurrence
    //Move all the other terms to the nearest location corresponding to the rarest term;
    //Return false if the end of the doc is passed
    bool Next();

    //Move the rarest term to the first occurrence in the document specified by docIndex
    //Move all other terms to the nearest location corresponding to the rarest term;
    bool Start();

    //Update the score corresponding to the doc specified by docIndex
    //The location of all terms in current span is given by location
    void update_score();

    //Return the calculated score
    float get_score();

private:
    //the total number of terms in the phrase
    size_t numTerms;

    size_t positionRarestTerm;

    //the ISRWord abstract for other terms
    ISRWord **Terms;

    Match *document;

    ::vector<Location> location;

    //score for each document
    float score;

    Location smallest, farthest;

    struct Statistics statistics;

    struct Weights *weights;

    bool ifShortSpan();

    bool ifOrderSpan();

    bool ifExactPhrases();

    bool ifNearTop();

    void calculate_num_frequent_words();

};

float
calculate_scores(Match *document, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm, struct Weights *weights);


float
calculate_static_scores( Match *document );