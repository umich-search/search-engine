#pragma once
#include "ISR.h"
#include "abstractISR.h"
#include "Common.h"
#include "Vector.h"


::vector<Match*>* ConstraintSolver(ISREndDoc* EndDoc, ISR* queryRoot);

class ConstraintSolver
   {
   public:
      ConstraintSolver();
      // return the first match starting from location currentLocation
      Match* findMatch(ISREndDoc* EndDoc, ISR* queryRoot);
      // manually set currentLocation to begin search
      void setLocation(Location location);

   private:
      Location currentLocation;
   };