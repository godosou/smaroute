//----------------------------------------------------------------
// File: patPathIterator.h
// Author: Michel Bierlaire
// Creation: Tue Jun  9 07:00:01 2009
//----------------------------------------------------------------

#ifndef patPathIterator_h
#define patPathIterator_h

#include <map>
#include "patType.h"
#include "patIterator.h"

class patPath ;

class patPathIterator: public patIterator<patPath*> {

 public:
  patPathIterator(map<patULong,patPath>* aMap) ;
  void first()  ;
  void next()  ;
  patBoolean isDone()  ;
  patPath* currentItem()  ;

 private:
  map<patULong,patPath>* theMap ;
  map<patULong,patPath>::iterator theIter ;
};


#endif
