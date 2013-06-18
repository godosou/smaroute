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
  //  string fileName("yverdon.xml") ;
   //string fileName("syn1.xml") ;
  // string fileName("paris.xml") ;
   //string fileName("rondPointEtoile.xml") ;
   string fileName("lausanne.xml") ;
   //string fileName("syn1.xml") ;
//string fileName("workTrip.xml") ;
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
  theNetwork.writeKML(patString("syn_network"),err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }
  */
	patSample theSample;

	patReadSample theReadSample(&theSample,&theNetwork,err);
	vector< list <  pair<patArc*,unsigned long> > > adjList = theReadSample.getAdjacencyLists();
	theNetwork.adjList_arc = &adjList;
	DEBUG_MESSAGE("get ajacency list"<<adjList.size());
	//theReadSample.readDataDirectory(err);*/
	patNode* o = theNetwork.getNodeFromUserId(unsigned long(393712385));
	patNode* d = theNetwork.getNodeFromUserId(unsigned long(269608414));
	//patNode* o = theNetwork.getNodeFromUserId(patULong(1));
	//patNode* d = theNetwork.getNodeFromUserId(patULong(21));
	if(o==NULL){
		DEBUG_MESSAGE("invalid o");
	}
	if(d==NULL){
		DEBUG_MESSAGE("invalid d");
	}
	theSample.addOd(patOdJ(o,d));
	DEBUG_MESSAGE("start path sampling");
	//patPathSampling thePathSampling(&theNetwork,&theSample);
	//thePathSampling.run(patULong(1),err);
	//theSample.assignPathIds();
	theSample.writeKML("sample.kml");
	/*
	patWriteBiogemeInput theBiogemeInput(&theSample,err);
	string datFN("Jingmin.dat") ;
	string modFN("Jingmin") ;

	theBiogemeInput.writeSampleFile(datFN);
	theBiogemeInput.writeSpecFile(modFN);
  */
}
