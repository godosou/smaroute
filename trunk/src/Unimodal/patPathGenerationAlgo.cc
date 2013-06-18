//----------------------------------------------------------------
// File: patPathGenerationAlgo.cc
// Author: Michel Bierlaire
// Creation: Tue May 19 17:02:38 2009
//----------------------------------------------------------------

#include "patDisplay.h"
#include "patPathGenerationAlgo.h"
#include "patErrMiscError.h"

patPathGenerationAlgo::patPathGenerationAlgo(patString aName) : 
  algoName(aName),
  maximumNumberOfTrials(0) {

}

void patPathGenerationAlgo::setMaximumNumberOfTrials(patULong mnt) {
  maximumNumberOfTrials = mnt ;
}

void patPathGenerationAlgo::generateAllPaths(patNetwork* theNetwork, patOd aOd, patError*& err) {
  if (maximumNumberOfTrials == 0) {
    err = new patErrMiscError("The maximum number of trials must be specificed") ;
    WARNING(err->describe()) ;
    return ;
  }
  for (patULong trial = 0 ; trial < maximumNumberOfTrials ; ++trial) {
    generateNextPath(theNetwork,aOd,err) ;
    if (err != NULL) {
      WARNING(err->describe()) ;
      return ;
    }
  }
  return ;
}

patString patPathGenerationAlgo::getName() {
  return algoName ;
}
