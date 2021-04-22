#include "ISR.h"
#include "Common.h"
#include "Vector.h"

struct Match
   {
   Offset id;
   Location start, end;
   Match(Offset d, Location s, Location e): id(d), start(s), end(e){}
   };

::vector<Match*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot);
