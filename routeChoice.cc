//----------------------------------------------------------------
// File: routeChoice.cc
// Author: Michel Bierlaire
// Creation: Thu Oct 30 14:14:12 2008
//----------------------------------------------------------------

#include "patReadUserInfoFromXml.h"
#include "patDisplay.h"
#include "patGeoCoordinates.h"
#include "patRandomWalk.h"
#include "patMonteCarlo.h"
#include "patAngle.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    DEBUG_MESSAGE("argc = " << argc) ;
    WARNING("Usage: " << argv[0] << " xmlFile") ;
    exit( -1 );
  }

  

//   patGeoCoordinates g1(46.528736, 6.567139) ;
//   patGeoCoordinates g2(46.514873, 6.559953) ;
//   DEBUG_MESSAGE(g1.distanceTo(g2)) ;
//   exit(0) ;
  patError* err(NULL) ;

  //  patString fileName("yverdon.xml") ;
  // patString fileName("lausanne.xml") ;
  // patString fileName("paris.xml") ;
  // patString fileName("rondPointEtoile.xml") ;
  // patString fileName("epfl.xml") ;
  //patString fileName("emma.xml") ;
  //patString fileName("stockholm.xml") ;

  patString fileName = patString(argv[1]) ;
  patReadUserInfoFromXml theXml(fileName) ;

  theXml.readFile(err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }
  patNetwork theNetwork = theXml.getNetwork() ;


//   Debug_Message("Subpaths") ;
//   Debug_Message(Thenetwork.Displaysubpaths(Err)) ;

//   patRandomWalk theRandomWalkAlgorithm(&theNetwork) ;

//   theRandomWalkAlgorithm.setKumaParameters(4.0, 1.0) ;
//   theNetwork.generatePaths(&theRandomWalkAlgorithm,10,err) ;

//   patMonteCarlo theMonteCarloAlgorithm(&theNetwork) ;
//   theMonteCarloAlgorithm.setStandardError(1.0) ;
//   theNetwork.generatePaths(&theMonteCarloAlgorithm,10,err) ;
//   if (err != NULL) {
//     WARNING(err->describe()) ;
//     exit(-1) ;
//   }
  
//   theNetwork.computePathAttributes(err) ;
//   if (err != NULL) {
//     WARNING(err->describe()) ;
//     exit(-1) ;
//   }

  
  stringstream str ;
  str << theNetwork.getName() << ".kml" ;
  theNetwork.writeKML(patString(str.str()),err) ;
  if (err != NULL) {
    WARNING(err->describe()) ;
    exit(-1) ;
  }

  
}
