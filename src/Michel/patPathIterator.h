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
  patPathIterator(map<unsigned long,patPath>* aMap) ;
  void first()  ;
  void next()  ;
  bool isDone()  ;
  patPath* currentItem()  ;

 private:
  map<unsigned long,patPath>* theMap ;
  map<unsigned long,patPath>::iterator theIter ;
};


#endif
