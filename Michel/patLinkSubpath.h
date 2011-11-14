//----------------------------------------------------------------
// File: patLinkSubpath.h
// Author: Michel Bierlaire
// Creation: Sat May 30 19:21:04 2009
//----------------------------------------------------------------

#ifndef patLinkSubpath_h
#define patLinkSubpath_h

#include "patSubpath.h"

class patArc ;

class patLinkSubpath : public patSubpath {

 public:
  patLinkSubpath(patArc* anArc) ;
  list<patULong> getListOfNodes(patError*& err) ;
  list<patULong> getListOfArcs(patError*& err) ;
  patULong getOrig(patError*& err) ;
  patULong getDest(patError*& err) ;
 protected:
  patArc* theLink ;
};
#endif
