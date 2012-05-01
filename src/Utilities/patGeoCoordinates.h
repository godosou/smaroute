//----------------------------------------------------------------
// File: patGeoCoordinates.h
// Author: Michel Bierlaire
// Creation: Fri Oct 31 08:25:08 2008
//----------------------------------------------------------------

#ifndef patGeoCoordinates_h
#define patGeoCoordinates_h

#include <iostream>
#include "patType.h"
class patGpsPoint;//added by Jingmin
class patGeoCoordinates {
	friend class patGpsPoint;//added by Jingmin
	friend class patTrip;//added by Jingmin
	friend class patNetwork;//added by Jingmin
	friend class patArc;//added by Jingmin
	friend class patNode;//added by Jingmin
	friend class patSimulator;//added by Jingmin
	friend class patRetrieveOSMFile;//added byJingmin
  friend ostream& operator<<(ostream& str, const patGeoCoordinates& x) ;

friend bool operator<(const patGpsPoint& g1, const patGpsPoint& g2) ;
 public:
  /**
    Constructor
    @param lat latitude (in degrees)
    @param lon longitude (in degrees)
  */
  patGeoCoordinates(patReal lat, patReal lon) ;
patGeoCoordinates();
  /**
     Compute the distance (in meters) between the current point and another point
   */
  patReal distanceTo(const patGeoCoordinates& anotherPoint) const;

  /**
   */
  patString getKML() const ;

 public:
  patReal latitudeInDegrees ;
  patReal longitudeInDegrees ;

 private:

  patReal latitudeInRadians ;
  patReal longitudeInRadians ;
  short latDegree ;
  short latMinutes ;
  patReal latSeconds ;
  char latOrientation ;
  short lonDegree ;
  short lonMinutes ;
  patReal lonSeconds ;
  char lonOrientation ;
};


#endif
