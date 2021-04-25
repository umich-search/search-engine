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
      ISREndDoc* EndDoc;
      ISR* queryRoot;
   };