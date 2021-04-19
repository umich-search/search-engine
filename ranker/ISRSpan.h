#include "../index/include/Dictionary.h"
#include "../index/include/ISR.h"
#include "../index/include/Post.h"
#include "../index/include/PostingList.h"
#include "../utility/include/Vector.h"
#include "../constraint_solver/constraint_solver.h"

class ISRSpan{
public:
    void Next();

    void Start(Location startLocation);

    void update_score();

private:
    //the total number of terms in the phrase
    size_t numTerms;

    //the ISRWord abstract for the rarest term
    ISRWord* rarestTerm;

    //the ISRWord abstract for other terms
    ISRWord** Terms;

    Location startDocLocation;

    Location endDocLocation;

    ::vector<Match*>* ConstraintSolver;

    ::vector<Location> location;

    float score;
};
