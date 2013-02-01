//----------------------------------------------------------------
// File: routeChoice.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 14:14:12 2008
//----------------------------------------------------------------

#include "patReadNetworkFromXml.h"
#include "patGeoCoordinates.h"
#include "patReadSample.h"
#include "patNetwork.h"
#include "patSample.h"
#include "patPathSampling.h"
#include "patWriteBiogemeInput.h"
#include "patDisplay.h"
#include "patGpsPoint.h"
#include "patSimulator.h"
#include "patNBParameters.h"
#include <time.h>
 
 
int main(int argc, char *argv[]) {

  time_t timeSeed = time(NULL); 
  patULong seed = 1250770522;
	patSimulator theSimulator(seed);
	list<patULong> realPath ;
	patError* err(NULL) ;
	
patNBParameters::the()->readFile("config.xml",err);
patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

patReal parArc = patNBParameters::the()->simNumberOfHorizonArcs ;
	patReal arcLength= patNBParameters::the()->simHorizonArcLength ;
	patReal turn = patNBParameters::the()->simTurnUp ;
	
	patULong theTripId = 0;
	//patReal speed = 60;
	patReal speed= patNBParameters::the()->simSpeed ;
	
	
	patReal upDownD = 20.0;
	upDownD =  patNBParameters::the()->simNetworkUpDownDistance ;
	DEBUG_MESSAGE("up down distance"<<upDownD);
	list<patULong>* tempList = new list<patULong>;
	
	DEBUG_MESSAGE(parArc<<","<<upDownD<<","<<arcLength);
	
	patString fileName = theSimulator.genNetwork_grid(
			parArc,upDownD,arcLength,turn,tempList);
	 patReadNetworkFromXml theXmlNetwork(fileName) ;

  theXmlNetwork.readFile(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }
  patNetwork theNetwork = theXmlNetwork.getNetwork() ;

  

  DEBUG_MESSAGE("Before cleaning: " << theNetwork.nbrOfArcs() << " arcs and " <<theNetwork.nbrOfNodes() << " nodes") ;

   theNetwork.finalizeNetwork(err) ;
   if (err != NULL) {
     WARNING(err->describe()) ;
     exit(-1) ;
   }
   
  DEBUG_MESSAGE("After cleaning: " << theNetwork.nbrOfArcs() << " arcs and " <<theNetwork.nbrOfNodes() << " nodes") ;
	stringstream stream;
	stream<<fileName;
	stream<<".kml";
	
  theNetwork.writeKML(stream.str(),err) ;
	
 	patSample theSample;
	patReadSample theReadSample(&theSample,&theNetwork,err);
	
	for(patULong i=0;i<patNBParameters::the()->simNumberOfTrips;++i){
		vector<patGpsPoint> gpsSequence;
		//stringstream dir;
		//dir<<"speed "<<speed;
		
		theSimulator.setSpeedDelta(speed * patNBParameters::the()->simSpeedDeltaRatio);
		gpsSequence = theSimulator.travel(speed);
		DEBUG_MESSAGE(" return gps points"<<gpsSequence.size());
		
			
		theReadSample.readGpsTrack(&gpsSequence,++theTripId,realPath);
		
		
		DEBUG_MESSAGE("ok2");
		
	} 
	
	patPathSampling thePathSampling(&theNetwork,&theSample);
	thePathSampling.run(patNBParameters::the()->pathSamplingAlgo,err);
	theSample.assignPathIds();
	theSample.writeKML("sample.kml");
	///*
	patWriteBiogemeInput theBiogemeInput(&theSample,err);
	/*
	if (err != NULL) {
     WARNING(err->describe()) ;
     exit(-1) ;
   }
   */
	patString datFN("Jingmin.dat") ;
	patString modFN("Jingmin.mod") ;
	
	theBiogemeInput.writeSampleFile(datFN);
	theBiogemeInput.writeSpecFile(modFN);
	exit(-1);
}


