// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "ISR.h"
#include "abstractISR.h"
#include "Vector.h"
#include "Dictionary.h"
#include "constraint_solver.h"

// using namespace std;    // FOR TESTING ONLY!

ConstraintSolver::ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
   {
   currentLocation = 0;
   this->EndDoc = EndDoc;
   this->queryRoot = queryRoot;
   }

Match* ConstraintSolver::findMatch()
   {
   if (currentLocation == SIZE_MAX)
      return nullptr;
   Post* match = nullptr;
   std::cout << "Searching for match for query" << std::endl;
   match = queryRoot->Seek( currentLocation );
   if (!match)    // no more matches
      {
      currentLocation = SIZE_MAX;
      return nullptr;
      }
   std::cout << "Found matching query" << std::endl;
   // find the next endDoc location in "Post endDoc"
   std::cout << "Seeking on end doc list" << std::endl;
   Post* endDoc = EndDoc->Seek(match->GetStartLocation());
   std::cout << "Finish end doc seek" << std::endl;
   currentLocation = endDoc->GetStartLocation();
   // output index + start/end location of matching doc
   return new Match(EndDoc->GetCurrIndex(), currentLocation - EndDoc->GetDocumentLength(), currentLocation);
   }

void ConstraintSolver::setLocation(Location location)
   {
   currentLocation = location;
   }