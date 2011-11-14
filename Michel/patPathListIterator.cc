//----------------------------------------------------------------
// File: patPathListIterator.cc
// Author: Michel Bierlaire
// Creation: Wed Jun 10 22:04:51 2009
//----------------------------------------------------------------

#include "patPath.h"
#include "patPathListIterator.h"

patPathListIterator::patPathListIterator(list<patPath>* aList): 
  theList(aList) {

}

void patPathListIterator::first() {
  if (theList != NULL) {
    theIter = theList->begin() ;
  }
}
void patPathListIterator::next() {
  if (theList != NULL) {
    ++theIter ;
  }  
}
patBoolean patPathListIterator::isDone() {
  if (theList != NULL) {
    return (theIter == theList->end()) ;
  }  
  else {
    return patTRUE ;
  }
}

patPath* patPathListIterator::currentItem() {
  if (theList != NULL) {
    return &(*theIter) ;
  }
  else {
    return NULL ;
  }
}

