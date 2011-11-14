//----------------------------------------------------------------
// File: patComputeAngle.cc
// Author: Michel Bierlaire
// Creation: Wed May 20 16:27:28 2009
//----------------------------------------------------------------

#include "patComputeAngle.h"
#include "patMath.h"
#include "patDisplay.h"

patComputeAngle::patComputeAngle(patGeoCoordinates p1, 
		patGeoCoordinates p2,
		patGeoCoordinates p3) :
  point1(p1),
  point2(p2),
  point3(p3) {
}

patReal patComputeAngle::getAngle() {
  // Vector a = p2 - p1 
  
  patReal alat = point2.latitudeInDegrees - point1.latitudeInDegrees ;
  patReal alon = point2.longitudeInDegrees - point1.longitudeInDegrees ;
  
  patReal norma = sqrt(alat * alat + alon * alon) ;
  alat /= norma ;
  alon /= norma ;
  // Vector b = p3 - p2

  patReal blat = point3.latitudeInDegrees - point2.latitudeInDegrees ;
  patReal blon = point3.longitudeInDegrees - point2.longitudeInDegrees ;
  patReal normb = sqrt(blat * blat + blon * blon) ;
  blat /= normb ;
  blon /= normb ;
  
  // innerproduct
  
  patReal innerProduct = alat * blat + alon * blon ;

  patReal theta = acos(innerProduct) ;

  // outer product for the sign
  // left turn = negative sign
  // right turn = positive sign
  patReal outerProduct = - alon * blat + alat * blon ;

  return (theta * patSgn(outerProduct)) ;
}

