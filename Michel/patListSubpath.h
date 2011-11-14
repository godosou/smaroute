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
  patListSubpath(patULong aId) ;
  list<patULong> getListOfNodes(patError*& err) ;
  list<patULong> getListOfArcs(patError*& err) ;
  void pushFrontSubpath(patSubpath* aSubpath) ;
  void pushBackSubpath(patSubpath* aSubpath) ;
  patULong getOrig(patError*& err) ;
  patULong getDest(patError*& err) ;

 protected:
  list<patSubpath*> theList ;
};
#endif
