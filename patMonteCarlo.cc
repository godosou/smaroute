//----------------------------------------------------------------
// File: patMonteCarlo.cc
// Author: Michel Bierlaire
// Creation: Wed May 20 11:40:21 2009
//----------------------------------------------------------------

#include "patMonteCarlo.h"
#include "patNetwork.h"
#include "patDisplay.h"
#include "patErrMiscError.h"
#include "patParameters.h"

patMonteCarlo::patMonteCarlo() : patPathGenerationAlgo(patString("Monte Carlo")), stderr(0.0), arng(patParameters::the()->getgevSeed()) {

  theNormal.setUniform(&arng) ;
}

void patMonteCarlo::generateNextPath(patNetwork* theNetwork, patOd aOd, patError*& err) {
  theNetwork->generateMonteCarloCosts(&theNormal, stderr, patTRUE, err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  patBoolean success ;
  patPath theShortest = theNetwork->getShortestPath(aOd,&success,err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }
  if (success) {
    map<patPath,pair<patReal,patULong> >::iterator found ;
    found = listOfPaths.find(theShortest) ;
    if (found == listOfPaths.end()) {
      listOfPaths[theShortest] = pair<patReal,patULong>(0,1) ;
    } 
    else {
      if (found->second.first != 0) {
	stringstream str ;
	str << "Path generated with log-proba " << 0 << " and previously generated with log-proba " << found->second.first ;
	err = new patErrMiscError(str.str()) ;
	return ;
      }
      ++found->second.second ;
    }
  }
  theNetwork->restoreOriginalCosts(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    return ;
  }

}

void patMonteCarlo::setStandardError(patReal s) {
  stderr = s ;
}


patString patMonteCarlo::getReport() {

}
