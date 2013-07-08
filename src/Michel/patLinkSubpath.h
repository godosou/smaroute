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
  list<unsigned long> getListOfNodes(patError*& err) ;
  list<unsigned long> getListOfArcs(patError*& err) ;
  unsigned long getOrig(patError*& err) ;
  unsigned long getDest(patError*& err) ;
 protected:
  patArc* theLink ;
};
#endif
