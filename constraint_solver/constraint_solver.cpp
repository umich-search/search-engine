#include <iostream>
#include "ISR.h"
#include "../utility/HashTable.h"
#include "../utility/Common.h"
#include "../index/dictionary.h"

::vector<Post*>* ConstraintSolver(ISR* queryRoot)
    {
    // TODO: assumed index start at location 1
    ISR* EndDoc = Dictionary::OpenISREndDoc();
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