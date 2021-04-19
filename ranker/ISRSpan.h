#include "../index/include/Dictionary.h"
#include "../index/include/ISR.h"
#include "../index/include/Post.h"
#include "../index/include/PostingList.h"
#include "../utility/include/Vector.h"
#include "../constraint_solver/constraint_solver.h"

class ISRSpan{
public:
    //Move the rarest term to the next occurrence
    //Move all the other terms to the nearest location corresponding to the rarest term;
    //Return True if the end of the doc is reached
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

    //the ISRWord abstract for the rarest term
    ISRWord* rarestTerm;

    //the ISRWord abstract for other terms
    ISRWord** Terms;

    //the current index of document in ConstraintSolver
    size_t docIndex = 0;

    ::vector<Match*>* ConstraintSolver;

    ::vector<Location> location;

    ::vector<float> score;
};
