//----------------------------------------------------------------
// File: patPathListIterator.h
// Author: Michel Bierlaire
// Creation: Wed Jun 10 22:03:30 2009
//----------------------------------------------------------------

#ifndef patPathListIterator_h
#define patPathListIterator_h

#include <map>
#include "patType.h"
#include "patIterator.h"

class patPath ;

class patPathListIterator: public patIterator<patPath*> {

 public:
  patPathListIterator(list<patPath>* aList) ;
  void first()  ;
  void next()  ;
  bool isDone()  ;
  patPath* currentItem()  ;

 private:
  list<patPath>* theList ;
  list<patPath>::iterator theIter ;
};


#endif
