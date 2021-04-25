#pragma once
#include "ISR.h"
#include "abstractISR.h"
#include "Common.h"
#include "Vector.h"

class ConstraintSolver
   {
   public:
      ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot);
      // return the first match starting from location currentLocation
      Match* findMatch();
      // manually set currentLocation to begin search
      void setLocation(Location location);

   private:
      Location currentLocation;
<<<<<<< HEAD
=======
      ISREndDoc* EndDoc;
      ISR* queryRoot;
>>>>>>> a9ac9a905d37172b33cdabe9395831b0347bd911
   };