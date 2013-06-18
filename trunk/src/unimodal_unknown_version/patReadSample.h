//----------------------------------------------------------------
// File: patReadSampleFromCsv.h
// Author:
// Creation:
//----------------------------------------------------------------

#ifndef patReadSample_h
#define patReadSample_h

#include "patString.h"
#include "patType.h"

#include "patError.h"
#include "patNetwork.h"
#include <list>
#include "patArc.h"
#include <vector>
#include "patString.h"
class patSample;
class patGpsPoint;

class patReadSample {


 public:
  vector< list <  pair<patArc*,unsigned long> > > getAdjacencyLists();
  patReadSample(patSample* sample, 
					 patNetwork* network,
					patError*& err);
	void  buildAdjacencyLists( patError*& err);	
	
	vector<patString> listDirectory(patError*& err);
	
	void readDataDirectory(patError*& err);
	
void calArcPriority();
	void readFile(patString& fileName,patError*& err) ;
	void readGpsTrack(vector<patGpsPoint>* gpsSequence,unsigned long theTripId,list<unsigned long> realPath);
	void finalizeReading();
	
	patSample* getSample();
	
 protected:
  patString fileName ;
  unsigned long tripNumber;
  	vector< list <  pair<patArc*,unsigned long> > > adjacencyLists;

	patNetwork* theNetwork;
	patSample* theSample;
};

#endif

