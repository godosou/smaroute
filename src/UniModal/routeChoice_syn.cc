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
  unsigned long seed = 1250770522;
	patSimulator theSimulator(seed);
	list<unsigned long> realPath ;
	patError* err(NULL) ;
	
patNBParameters::the()->readFile("config.xml",err);
patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

double parArc = patNBParameters::the()->simNumberOfHorizonArcs ;
	double arcLength= patNBParameters::the()->simHorizonArcLength ;
	double turn = patNBParameters::the()->simTurnUp ;
	
	unsigned long theTripId = 0;
	//patReal speed = 60;
	double speed= patNBParameters::the()->simSpeed ;
	
	
	double upDownD = 20.0;
	upDownD =  patNBParameters::the()->simNetworkUpDownDistance ;
	DEBUG_MESSAGE("up down distance"<<upDownD);
	list<unsigned long>* tempList = new list<unsigned long>;
	
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
	
	for(unsigned long i=0;i<patNBParameters::the()->simNumberOfTrips;++i){
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


