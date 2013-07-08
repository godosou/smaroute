//----------------------------------------------------------------
// File: patSubpath.h
// Author: Michel Bierlaire
// Creation: Sat May 30 19:13:30 2009
//----------------------------------------------------------------

#ifndef patSubpath_h
#define patSubpath_h

#include "patConst.h"
#include "patPath.h"
#include "patError.h"

class patSubpath {

 public:
  patSubpath(patULong aId) ;
  
  virtual patPath getPath(patString aName, patError*& err) ;
  virtual patPath getPath(patError*& err) ;
  virtual list<patULong> getListOfNodes(patError*& err) = PURE_VIRTUAL ;
  virtual list<patULong> getListOfArcs(patError*& err) = PURE_VIRTUAL ;
  virtual patULong getOrig(patError*& err) = PURE_VIRTUAL ;
  virtual patULong getDest(patError*& err) = PURE_VIRTUAL ;
  patBoolean checkConnectivity(patNetwork* theNetwork, patError*& err) ;
  patULong getId() const;
 protected:
  patULong id ;
};
#endif
