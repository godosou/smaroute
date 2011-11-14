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
#include <time.h>

patNetwork genNetwork(){

	//patSimulator theSimulator(patULong(314371)
	
	patError* err(NULL) ;
	
  patString fileName("syn_network.xml") ;
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
  theNetwork.writeKML(patString("syntheic.kml"),err) ;
 
	return theNetwork;
	
}
int main(int argc, char *argv[]) {


  time_t timeSeed = time(NULL); 
  patULong seed = 1250770522;
	patSimulator theSimulator(seed);
	list<patULong> realPath ;
	patReal parArc = 10;
	patReal arcLength= 80.0;
	patReal turn = -0.7;
	theSimulator.genNetwork_grid(parArc,20.0,arcLength,turn,&realPath);
	
	
	patULong theTripId = 0;
	//patReal speed = 60;
	patReal speed= 40.0;
	
	patError* err(NULL) ;
	
	vector<patString> fileList;
	
	for(patULong k = 0;k<10;++k){
	patReal upDownD = 5.0+k * 5.0;
	DEBUG_MESSAGE("up down distance"<<upDownD);
	list<patULong>* tempList = new list<patULong>;
	patString fileName = theSimulator.genNetwork_grid(
			parArc,5.0+k*10.0,arcLength,turn,tempList);
	
	
 
	fileList.push_back(fileName); 

	}
	DEBUG_MESSAGE("network size"<<fileList.size());
	for(patULong i=0;i<5;++i){
		vector<patGpsPoint> gpsSequence;
		//stringstream dir;
		//dir<<"speed "<<speed;
		for(patULong j= 0;j<3;++j){
		
		theSimulator.setSpeedDelta(speed*(i*0.1+0.15));
		gpsSequence = theSimulator.travel(speed);
		DEBUG_MESSAGE(" return gps points"<<gpsSequence.size());
		
		for(patULong k=0;k<fileList.size();k++){
		
			patSample theSample;
  //patString fileName("syn_network.xml") ;
  patReadNetworkFromXml theXmlNetwork(fileList[k]) ;

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
	stream<<fileList[k];
	stream<<".kml";
	
  theNetwork.writeKML(stream.str(),err) ;
			patReadSample theReadSample(&theSample,&theNetwork,err);
			DEBUG_MESSAGE("OK2");
			
			theReadSample.readGpsTrack(&gpsSequence,++theTripId,realPath);
			
		}
		
		DEBUG_MESSAGE("ok2");
		}
		
	} 
	//patPathSampling thePathSampling(&theNetwork,&theSample);
	//thePathSampling.run(patULong(1),err);
	
	/*patWriteBiogemeInput theBiogemeInput( (const patSample*)&theSample,err);
	theBiogemeInput.writeSampleFile(patString("Jingmin.dat"));
	theBiogemeInput.writeSpecFile(patString("Jingmin.mod"));
  */
}
