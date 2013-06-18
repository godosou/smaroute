//----------------------------------------------------------------
// File: patComputeAngle.h
// Author: Michel Bierlaire
// Creation: Wed May 20 16:25:04 2009
//----------------------------------------------------------------

#ifndef patComputeAngle_h
#define patComputeAngle_h

#include "patGeoCoordinates.h"

class patComputeAngle {
  
 public:
  patComputeAngle(patGeoCoordinates p1, 
		  patGeoCoordinates p2,
		  patGeoCoordinates p3) ;

  patReal getAngle() ;

 private:
  patGeoCoordinates point1 ;
  patGeoCoordinates point2 ;
  patGeoCoordinates point3 ;
  
};

#endif
