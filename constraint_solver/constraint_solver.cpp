// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "../index/include/ISR.h"
#include "AbstractISR.h"
#include "../utility/include/Vector.h"
#include "../index/include/Dictionary.h"

::vector<Post*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
    {
    // TODO: assumed index start at location 1
    Location currentLocation = 0;
    Post* match = nullptr;
    ::vector<Post*>* posts = new ::vector<Post*>(); 
    while ( match = queryRoot->Seek(currentLocation) )
        {
        EndDoc->Seek(match->GetStartLocation());
        currentLocation = EndDoc->GetEndLocation();
        Location startLocation = currentLocation - EndDoc->GetDocumentLength();
        Post* document = new Post(startLocation, currentLocation);
        posts->pushBack(document);
        }
    return posts;
    }
