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
#include <pqxx/pqxx>

using namespace pqxx;
int main(int argc, char *argv[]) {


   
//   patGeoCoordinates g1(46.528736, 6.567139) ;
//   patGeoCoordinates g2(46.514873, 6.559953) ;
//   DEBUG_MESSAGE(g1.distanceTo(g2)) ;
//   exit(0) ;

  patError* err(NULL) ;
patNBParameters::the()->readFile(argv[1],err);
//patNBParameters::the()->showAll();
patNBParameters::the()->init(err);
if(err!=NULL){
exit(-1);
}

  //  patString fileName("yverdon.xml") ;
   patString fileName(patNBParameters::the()->OsmNetworkFileName) ;
  // patString fileName("paris.xml") ;
   //patString fileName("rondPointEtoile.xml") ;
  // patString fileName("epfl.xml") ;
//patString fileName("workTrip.xml") ;
   DEBUG_MESSAGE("Start to read xml network");
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
theNetwork.writeKML(fileName+".kml",err) ;
  //theNetwork.writeKML(patString("lausanne.kml"),err) ;
 
  
	patSample theSample;
	
	patReadSample theReadSample(&theSample,&theNetwork,err);
	vector< list <  pair<patArc*,patULong> > > adjList = theReadSample.getAdjacencyLists();
	theNetwork.adjList_arc = &adjList;
	theReadSample.readDataDirectory(err);
	/*
	patNode* o = theNetwork.getNodeFromUserId(patULong(258755905));
	patNode* d = theNetwork.getNodeFromUserId(patULong(253204512));
	theSample.addOd(patOdJ(o,d));
	*/
/*
	if (err != NULL) {
     WARNING(err->describe()) ;
     exit(-1) ;
   }
 */
	/*
        theSample.assignPathIds();
	patString rfn("result/");

        rfn+= theSample.getTripIdsStr();
	
	theSample.writeKML("sample.kml");
	
	patWriteBiogemeInput theBiogemeInput(&theSample,&theNetwork,err);
	patString modFN(rfn) ;
	rfn+=".dat";
	
	patString datFN(rfn) ;
	
	theBiogemeInput.writeSampleFile(datFN);
	theBiogemeInput.writeSpecFile(modFN);
  */
}
