//----------------------------------------------------------------
// File: patReadNetworkFromXml.h
// Author: Michel Bierlaire
// Creation: Thu Oct 30 12:06:29 2008
//----------------------------------------------------------------

#ifndef patReadNetworkFromXml_h
#define patReadNetworkFromXml_h

#include "patString.h"
#include "patType.h"
#include "patError.h"
#include "patNetwork.h"

class patReadNetworkFromXml {

 public:
  // If ctn is TRUE, the network is cleaned, that is useless nodes are
  // removed, unconnected nodes as well. It means that some ids may be
  // changed.  If ctn is FALSE, nothing is done, and the ids defined
  // in the file are maintained.

  patReadNetworkFromXml(patString fName);
  bool readFile(patError*& err) ;
  patNetwork getNetwork() ;
 private:
  patString fileName ;
  patNetwork theNetwork ;
  unsigned long multiplierForTwoWaysIds ;
  bool cleanTheNetwork ;

};

#endif
