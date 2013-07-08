//----------------------------------------------------------------
// File: patListSubpath.h
// Author: Michel Bierlaire
// Creation: Sat May 30 19:50:27 2009
//----------------------------------------------------------------

#ifndef patListSubpath_h
#define patListSubpath_h

#include "patSubpath.h"

class patArc ;

class patListSubpath : public patSubpath {

 public:
  patListSubpath(unsigned long aId) ;
  list<unsigned long> getListOfNodes(patError*& err) ;
  list<unsigned long> getListOfArcs(patError*& err) ;
  void pushFrontSubpath(patSubpath* aSubpath) ;
  void pushBackSubpath(patSubpath* aSubpath) ;
  unsigned long getOrig(patError*& err) ;
  unsigned long getDest(patError*& err) ;

 protected:
  list<patSubpath*> theList ;
};
#endif
