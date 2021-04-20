#include "../index/include/Dictionary.h"
#include "../index/include/ISR.h"
#include "../index/include/Post.h"
#include "../index/include/PostingList.h"
#include "../utility/include/Vector.h"
#include "../constraint_solver/constraint_solver.h"

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

struct Weights {
    float weightShortSpan;
    float weightOrderSpan;
    float weightPhrase;
    float weightTopSpan;
    float weightAll;
    float weightMost;
    float weightSome;
};

class ISRSpan {
public:

    ISRSpan(::vector<Match *> *matchDocs, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm,
            struct Weights weights, size_t totalWords) : numTerms(numTerms), positionRarestTerm(positionRarestTerm),
                                                         Terms(Terms),
                                                         docIndex(0), matchDocs(matchDocs), weights(weights),
                                                         location(::vector<Location>(numTerms, 0)),
                                                         totalWords(totalWords) {
        statistics.numPhrases = 0;
        statistics.numOrderSpans = 0;
        statistics.numTopSpans = 0;
        statistics.numShortSpans = 0;
        statistics.numFrequentWords = 0;
    }

    //Move the rarest term to the next occurrence
    //Move all the other terms to the nearest location corresponding to the rarest term;
    //Return false if the end of the doc is passed
    bool Next();

    //Move the rarest term to the first occurrence in the document specified by docIndex
    //Move all other terms to the nearest location corresponding to the rarest term;
    //Update the docIndex to be equal to index
    void Start(size_t index);

    //Update the score corresponding to the doc specified by docIndex
    //The location of all terms in current span is given by location
    void update_score();

    //Return the calculated score
    ::vector<float> get_score();

private:
    //the total number of terms in the phrase
    size_t numTerms;

    size_t positionRarestTerm;

    size_t totalWords;

    //the ISRWord abstract for other terms
    ISRWord **Terms;

    //the current index of document in ConstraintSolver
    size_t docIndex;

    ::vector<Match *> *matchDocs;

    ::vector<Location> location;

    //score for each document
    ::vector<float> scores;

    Location smallest, farthest;

    struct Statistics statistics;

    struct Weights weights;

    bool ifShortSpan();

    bool ifOrderSpan();

    bool ifExactPhrases();

    bool ifNearTop();

    void calculate_num_frequent_words();

};

::vector<float>
calculate_scores(::vector<Match *> *matchDocs, ISRWord **Terms, size_t numTerms, size_t positionRarestTerm,
                 struct Weights weights, size_t totalWords);
