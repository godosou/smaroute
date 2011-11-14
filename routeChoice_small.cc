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
#include "patPathJ.h"
#include <time.h>
 
 
int main(int argc, char *argv[]) {

  time_t timeSeed = time(NULL); 
  patULong seed = patNBParameters::the()->randomSeed;
// seed =timeSeed;
	patError* err(NULL) ;
	
patNBParameters::the()->readFile("config_small.xml",err);
patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

patReal parArc = patNBParameters::the()->simNumberOfHorizonArcs ;

patNBParameters::the()->readFile("config.xml",err);
patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

  //  patString fileName("yverdon.xml") ;
 //  patString fileName("lausanne.xml") ;
  // patString fileName("paris.xml") ;
   patString fileName("emmaNetwork.xml") ;
  // patString fileName("epfl.xml") ;
//patString fileName("workTrip.xml") ;
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

  
  theNetwork.writeKML(patString("test.kml"),err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }
	patULong theTripId = 0;
	patReal speed= patNBParameters::the()->simSpeed ;
	
	patSimulator theSimulator(&theNetwork,seed);
	
	
 	patSample theSample;
	patReadSample theReadSample(&theSample,&theNetwork,err);
	patPathJ realPath;
	list<patULong> lrp;
	/*
	lrp.push_back(1100);
	lrp.push_back(1011);
	lrp.push_back(1120);
	lrp.push_back(2019);
	lrp.push_back(1918);
	lrp.push_back(1817);
	lrp.push_back(1723);
	lrp.push_back(2324);
	lrp.push_back(2425);
*/
	lrp.push_back(11000);
	lrp.push_back(101100);
	lrp.push_back(112000);
	lrp.push_back(201900);
	lrp.push_back(191800);
	lrp.push_back(182900);
	lrp.push_back(293000);
	lrp.push_back(303100);
	lrp.push_back(313800);
	for(list<patULong>::iterator uIter = lrp.begin();
		uIter!=lrp.end();
		++uIter){
		patArc* theArc = theNetwork.getArcFromUserId(*uIter);
		if(theArc==NULL){
			DEBUG_MESSAGE("err arc"<<*uIter);
			exit(-1);
		}
		realPath.addArcToBack(theArc);
	}
	for(patULong i=0;i<patNBParameters::the()->simNumberOfTrips;++i){
		vector<patGpsPoint> gpsSequence;
		//stringstream dir;
		//dir<<"speed "<<speed;
			seed *= 2;
			DEBUG_MESSAGE("speed"<<speed);
			theSimulator.setSpeedDelta(speed * patNBParameters::the()->simSpeedDeltaRatio);
			gpsSequence = theSimulator.travel(&realPath, speed);
		
			string kmlFileName;
			stringstream kmlFileNameStream(kmlFileName);
			kmlFileNameStream << "result/";
			kmlFileNameStream << i<<seed<<".csv";
			theSimulator.writeCsv(&gpsSequence,kmlFileNameStream.str());
			DEBUG_MESSAGE(" return gps points"<<gpsSequence.size());
		
			patReal old = patNBParameters::the()->pointProbaPower;
			theReadSample.readGpsTrack(&gpsSequence,++theTripId,lrp);
			patNBParameters::the()->pointProbaPower = 0.0;
			//theReadSample.readGpsTrack(&gpsSequence,++theTripId,lrp);
			patNBParameters::the()->pointProbaPower = old;
			
			patReal cdf = patNBParameters::the()->selectPathCdfThreshold;
			//theReadSample.readGpsTrack(&gpsSequence,++theTripId,lrp);
			patNBParameters::the()->selectPathCdfThreshold = 10.0;
			//theReadSample.readGpsTrack(&gpsSequence,++theTripId,lrp);
			patNBParameters::the()->selectPathCdfThreshold = cdf;
			
		DEBUG_MESSAGE("ok2");
		
	} 
	/*
	patPathSampling thePathSampling(&theNetwork,&theSample);
	thePathSampling.run(patNBParameters::the()->pathSamplingAlgo,err);
	theSample.assignPathIds();
	theSample.writeKML("sample.kml");
	
	patWriteBiogemeInput theBiogemeInput(&theSample,err);
	
	if (err != NULL) {
     WARNING(err->describe()) ;
     exit(-1) ;
   }
	patString datFN("Jingmin.dat") ;
	patString modFN("Jingmin.mod") ;
	
	theBiogemeInput.writeSampleFile(datFN);
	theBiogemeInput.writeSpecFile(modFN);
	*/
	exit(-1);
}


