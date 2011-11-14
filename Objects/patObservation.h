#ifndef patObservation_h
#define patObservation_h

#include "patType.h"
#include <map>
#include "patPathDDR.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include "patTraveler.h"
#include <iostream>
#include "patError.h"
#include "patPathSampling.h"
class patObservation{
 public:
	friend class patTripParser;
	friend class patPathDevelop;
 	friend patBoolean operator==(const patObservation& observation1, 
				const patObservation& observation2);
	friend patBoolean operator<(const patObservation& observation1, 
				const patObservation& observation2);
	patObservation();
	patObservation( patTraveler* theTraveler, patULong& theTipId);
	
	void setStartTime(const patULong& theStartTime);
	
	void setEndTime(const patULong& theEndTime);
	
	void setMapBounds(patReal minLat, 
			      patReal maxLat, 
			      patReal minLon, 
			      patReal maxLon);
				  
				  
	void setPathDDRs(const map<patPathJ*,patPathDDR>& thePathDDRs);
	
	map<patPathJ*,patPathDDR>* getPathDDRs() ;
	
	void setOdDDRs(const map<patOdJ*,patReal>& theOdDDRs);
	
	map<patOdJ*,patReal>* getOdDDRs();
	
	 patTraveler* getTraveler() ;
	
	 patULong getId() ;
	vector<patReal> calDDRAll();
	patReal getDDRAll();
patReal getDDRRawAll();
	map<patPathJ*,vector<patReal> >* getPathDDRsRealNormalized();

	map<patOdJ*,map<patPathJ*,patPathDDR> > getPathDDRsOrderByOd_Object();
map<patOdJ*,map<patPathJ*,vector<patReal> > >  getPathDDRsOrderByOd_Real();
map<patPathJ*,vector<patReal> >* getPathDDRsReal();
	map<patPathJ*,patReal>* getPathDDRsRealRaw();
void setOdPathSetByDDRs();
void normalizeDDRs();
map<patPathJ*, patPathSampling>* getChoiceSet();
void sampleChoiceSet(patNetwork* theNetwork,vector< list < pair<patArc*, patULong> > >* adjList,patError*& err);
patPathSampling* getPathSampling(patPathJ* aPath);
 protected:
	patULong id;
	patTraveler* traveler;
	patULong startTime;
	patULong endTime;
	patReal minLatitude ;
	patReal maxLatitude ;
	patReal minLongitude ;
	patReal maxLongitude ;

	map<patPathJ*,patPathDDR> pathDDRs;
	map<patPathJ*,vector<patReal> > pathDDRsReal;
	map<patPathJ*,vector<patReal> > pathDDRsRealNormalized;
	map<patPathJ*,patReal> pathDDRsRealSimple;
	map<patPathJ*,patReal> pathDDRsRealRaw;
	patReal ddrAll;
	patReal ddrSimpleAll;
	patReal ddrRawAll;
	patReal ddrMMAll;
	map<patOdJ*, patReal> odDDRs;
	map<patPathJ*, patPathSampling> ChoiceSet;
}
;
#endif
