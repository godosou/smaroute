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
  patSubpath(unsigned long aId) ;
  
  virtual patPath getPath(patString aName, patError*& err) ;
  virtual patPath getPath(patError*& err) ;
  virtual list<unsigned long> getListOfNodes(patError*& err) = PURE_VIRTUAL ;
  virtual list<unsigned long> getListOfArcs(patError*& err) = PURE_VIRTUAL ;
  virtual unsigned long getOrig(patError*& err) = PURE_VIRTUAL ;
  virtual unsigned long getDest(patError*& err) = PURE_VIRTUAL ;
  bool checkConnectivity(patNetwork* theNetwork, patError*& err) ;
  unsigned long getId() const;
 protected:
  unsigned long id ;
};
#endif
