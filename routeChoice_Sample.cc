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
#include "patNBParameters.h"



int main(int argc, char *argv[]) {


//   patGeoCoordinates g1(46.528736, 6.567139) ;
//   patGeoCoordinates g2(46.514873, 6.559953) ;
//   DEBUG_MESSAGE(g1.distanceTo(g2)) ;
//   exit(0) ;
  patError* err(NULL) ;

patNBParameters::the()->readFile("config.xml",err);
patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

  //  patString fileName("yverdon.xml") ;
   patString fileName("lausanne.xml") ;
  // patString fileName("paris.xml") ;
   //patString fileName("rondPointEtoile.xml") ;
  // patString fileName("epfl.xml") ;
//patString fileName("workTrip.xml") ;
//patString fileName("syn_network.xml") ;
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

  /*
  theNetwork.writeKML(patString("test.kml"),err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }
  */
	patSample theSample;
	
	patReadSample theReadSample(&theSample,&theNetwork,err);
	//theReadSample.readDataDirectory(err);
	///*
	patNode* o = theNetwork.getNodeFromUserId(patULong(258755905));
	patNode* d = theNetwork.getNodeFromUserId(patULong(253204512));
	theSample.addOd(patOdJ(o,d));
	patPathSampling thePathSampling(&theNetwork,&theSample);
	thePathSampling.run(patULong(1),err);
	theSample.assignPathIds();
	theSample.writeKML("sample.kml");
	///*
	patWriteBiogemeInput theBiogemeInput(&theSample,err);
	patString datFN("Jingmin.dat") ;
	patString modFN("Jingmin.mod") ;
	
	theBiogemeInput.writeSampleFile(datFN);
	theBiogemeInput.writeSpecFile(modFN);
  //*/
}
