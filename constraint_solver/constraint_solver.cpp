// Constraint solver function
// Input: ISR Tree from query compiler "queryRoot"
// Output: Array of Post with (start, end) of all matching documents "posts"
#include <iostream>
#include "../index/include/ISR.h"
#include "AbstractISR.h"
#include "../utility/include/Vector.h"
#include "../index/include/Dictionary.h"

// FOR TESTING ONLY!
using namespace std;

::vector<Post*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot)
    {
    Location currentLocation = 0;
    Post* match = nullptr;
    ::vector<Post*>* posts = new ::vector<Post*>(); 
    while ( match = queryRoot->Seek(currentLocation) )
        {
        EndDoc->Seek(match->GetStartLocation());
        cout<<"EndDoc start: "<<EndDoc->GetStartLocation()<<" EndDoc end: "<<EndDoc->GetEndLocation()<<endl;
        currentLocation = EndDoc->GetStartLocation();
        Location startLocation = currentLocation - EndDoc->GetDocumentLength();
        cout << " DocLength: " << EndDoc->GetDocumentLength() << " Start: " << startLocation << ", End: " << currentLocation << endl;
        Post* document = new Post(startLocation, currentLocation);
        posts->pushBack(document);
        }
    return posts;
    }
