//----------------------------------------------------------------
// File: patPathGenerationAlgo.h
// Author: Michel Bierlaire
// Creation: Tue May 19 16:51:11 2009
//----------------------------------------------------------------

#ifndef patPathGenerationAlgo_h
#define patPathGenerationAlgo_h

#include "patConst.h"
#include "patPath.h"
#include "patString.h"
#include "patError.h"
#include "patOd.h"
#include <map>


class patNetwork ;

class patPathGenerationAlgo {

 public:
  patPathGenerationAlgo(patString aName) ;
  // Set mnt to 0 if no upper bound is defined, typically with a
  // deterministic algorithm
  void setMaximumNumberOfTrials(unsigned long mnt) ;
  virtual void generateAllPaths(patNetwork* aNetwork, patOd aOd, patError*& err) ;
  virtual void generateNextPath(patNetwork* aNetwork, patOd aOd, patError*& err) = PURE_VIRTUAL ;
  virtual patString getName() ;
  virtual patString getReport() = PURE_VIRTUAL ;

  // The pair contains the probability for the ath to be generated,
  // and the number of times it has been drawn.
  map<patPath,pair<double,unsigned long> > listOfPaths ;

 protected:
  patString algoName ;

  unsigned long maximumNumberOfTrials ;

};

#endif
