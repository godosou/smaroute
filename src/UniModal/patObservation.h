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
 	friend bool operator==(const patObservation& observation1, 
				const patObservation& observation2);
	friend bool operator<(const patObservation& observation1, 
				const patObservation& observation2);
	patObservation();
	patObservation( patTraveler* theTraveler, unsigned long& theTipId);
	
	void setStartTime(const unsigned long& theStartTime);
	
	void setEndTime(const unsigned long& theEndTime);
	
	void setMapBounds(double minLat, 
			      double maxLat, 
			      double minLon, 
			      double maxLon);
				  
				  
	void setPathDDRs(const map<patPathJ*,patPathDDR>& thePathDDRs);
	
	map<patPathJ*,patPathDDR>* getPathDDRs() ;
	
	void setOdDDRs(const map<patOdJ*,double>& theOdDDRs);
	
	map<patOdJ*,double>* getOdDDRs();
	
	 patTraveler* getTraveler() ;
	
	 unsigned long getId() ;
	vector<double> calDDRAll();
	double getDDRAll();
double getDDRRawAll();
	map<patPathJ*,vector<double> >* getPathDDRsRealNormalized();

	map<patOdJ*,map<patPathJ*,patPathDDR> > getPathDDRsOrderByOd_Object();
map<patOdJ*,map<patPathJ*,vector<double> > >  getPathDDRsOrderByOd_Real();
map<patPathJ*,vector<double> >* getPathDDRsReal();
	map<patPathJ*,double>* getPathDDRsRealRaw();
void setOdPathSetByDDRs();
void normalizeDDRs();
map<patPathJ*, patPathSampling>* getChoiceSet();
void sampleChoiceSet(patNetwork* theNetwork,vector< list < pair<patArc*, unsigned long> > >* adjList,patError*& err);
patPathSampling* getPathSampling(patPathJ* aPath);
 protected:
	unsigned long id;
	patTraveler* traveler;
	unsigned long startTime;
	unsigned long endTime;
	double minLatitude ;
	double maxLatitude ;
	double minLongitude ;
	double maxLongitude ;

	map<patPathJ*,patPathDDR> pathDDRs;
	map<patPathJ*,vector<double> > pathDDRsReal;
	map<patPathJ*,vector<double> > pathDDRsRealNormalized;
	map<patPathJ*,double> pathDDRsRealSimple;
	map<patPathJ*,double> pathDDRsRealRaw;
	double ddrAll;
	double ddrSimpleAll;
	double ddrRawAll;
	double ddrMMAll;
	map<patOdJ*, double> odDDRs;
	map<patPathJ*, patPathSampling> ChoiceSet;
}
;
#endif
