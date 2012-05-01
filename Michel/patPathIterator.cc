//----------------------------------------------------------------
// File: patPathIterator.cc
// Author: Michel Bierlaire
// Creation: Tue Jun  9 07:02:22 2009
//----------------------------------------------------------------

#include "patPath.h"
#include "patPathIterator.h"
#include "patDisplay.h"

patPathIterator::patPathIterator(map<patULong,patPath>* aMap): 
  theMap(aMap) {

}

void patPathIterator::first() {
  if (theMap != NULL) {
    theIter = theMap->begin() ;
  }
}
void patPathIterator::next() {
  if (theMap != NULL) {
    ++theIter ;
  }  
}
patBoolean patPathIterator::isDone() {
  if (theMap != NULL) {
    return (theIter == theMap->end()) ;
  }  
  else {
    return patTRUE ;
  }
}

patPath* patPathIterator::currentItem() {
  if (theMap != NULL) {
    return &(theIter->second) ;
  }
  else {
    return NULL ;
  }
}
