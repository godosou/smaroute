//----------------------------------------------------------------
// File: patCoordinates.h
// Author: Michel Bierlaire
// Creation: Fri Oct 31 08:25:08 2008
//----------------------------------------------------------------

#ifndef patCoordinates_h
#define patCoordinates_h

#include <iostream>
#include "patType.h"
class patGpsPoint;
//added by Jingmin
class patCoordinates {
	friend class patGpsPoint; //added by Jingmin
	friend class patTrip; //added by Jingmin
	friend class patNetwork; //added by Jingmin
	friend class patArc; //added by Jingmin
	friend class patNode; //added by Jingmin
	friend class patSimulator; //added by Jingmin
	friend class patRetrieveOSMFile; //added byJingmin
	friend ostream& operator<<(ostream& str, const patCoordinates& x);

	friend bool operator<(const patGpsPoint& g1, const patGpsPoint& g2);
public:
	/**
	 Constructor
	 @param lat latitude (in degrees)
	 @param lon longitude (in degrees)
	 */
	patCoordinates(patReal lat, patReal lon);
	patCoordinates();
	/**
	 Compute the distance (in meters) between the current point and another point
	 */
	patReal distanceTo(const patCoordinates& anotherPoint) const;

	/**
	 */
	patString getKML() const;

	patReal getLatitude() const;
	patReal getLongitude() const;
	string getGeomText() const;
public:
	patReal latitudeInDegrees;
	patReal longitudeInDegrees;

private:

	patReal latitudeInRadians;
	patReal longitudeInRadians;
	short latDegree;
	short latMinutes;
	patReal latSeconds;
	char latOrientation;
	short lonDegree;
	short lonMinutes;
	patReal lonSeconds;
	char lonOrientation;
};

#endif
