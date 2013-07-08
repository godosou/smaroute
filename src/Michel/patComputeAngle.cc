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

double patComputeAngle::getAngle() {
  // Vector a = p2 - p1

  double alat = point2.latitudeInDegrees - point1.latitudeInDegrees ;
  double alon = point2.longitudeInDegrees - point1.longitudeInDegrees ;

  double norma = sqrt(alat * alat + alon * alon) ;
  alat /= norma ;
  alon /= norma ;
  // Vector b = p3 - p2

  double blat = point3.latitudeInDegrees - point2.latitudeInDegrees ;
  double blon = point3.longitudeInDegrees - point2.longitudeInDegrees ;
  double normb = sqrt(blat * blat + blon * blon) ;
  blat /= normb ;
  blon /= normb ;

  // innerproduct

  double innerProduct = alat * blat + alon * blon ;

  double theta = acos(innerProduct) ;

  // outer product for the sign
  // left turn = negative sign
  // right turn = positive sign
  double outerProduct = - alon * blat + alat * blon ;

  return (theta * patSgn(outerProduct)) ;
}

