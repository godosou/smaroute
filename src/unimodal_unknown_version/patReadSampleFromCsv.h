//----------------------------------------------------------------
// File: patReadSampleFromCsv.h
// Author:
// Creation:
//----------------------------------------------------------------

#ifndef patReadSampleFromCsv_h
#define patReadSampleFromCsv_h

#include "patString.h"
#include "patType.h"
#include "patSample.h"
#include "patError.h"


class patReadSampleFromCsv {


 public:
  patReadSampleFromCsv(patString fName,patNetwork* network);
  patBoolean readFile(patError*& err) ;
void buildAdjacencyLists( vector< list <  pair<patArc*,patULong> > >* adjacencyLists, patError*& err);
  
 protected:
  patString fileName ;
  patULong tripNumber;
	patNetwork* theNetwork;
};

#endif
