//----------------------------------------------------------------
// File: patCoordinates.cc
// Author: Michel Bierlaire
// Creation: Fri Oct 31 08:35:23 2008
//----------------------------------------------------------------

#include <sstream>
#include "patCoordinates.h"
#include "patConst.h"
#include "patNBParameters.h"
patCoordinates::patCoordinates(double lat, double lon) :
		latitudeInDegrees(lat), longitudeInDegrees(lon) {
	latitudeInRadians = latitudeInDegrees * pi / 180.0;
	longitudeInRadians = longitudeInDegrees * pi / 180.0;

	latOrientation = (latitudeInDegrees >= 0) ? 'N' : 'S';
	double tmp =
			(latitudeInDegrees >= 0) ? latitudeInDegrees : -latitudeInDegrees;
	latDegree = floor(tmp);
	tmp -= double(latDegree);
	tmp *= 60;
	latMinutes = floor(tmp);
	tmp -= double(latMinutes);
	tmp *= 60;
	latSeconds = tmp;

	lonOrientation = (longitudeInDegrees >= 0) ? 'E' : 'W';
	tmp = (longitudeInDegrees >= 0) ? longitudeInDegrees : -longitudeInDegrees;
	lonDegree = floor(tmp);
	tmp -= double(lonDegree);
	tmp *= 60;
	lonMinutes = floor(tmp);
	tmp -= double(lonMinutes);
	tmp *= 60;
	lonSeconds = tmp;

}

double patCoordinates::distanceTo(
		const patCoordinates& anotherPoint) const {
	// Formula from http://mathforum.org/library/drmath/view/51711.html

	if (patNBParameters::the()->coordinatesSystem == "simple") {
		return sqrt(
				(latitudeInDegrees - anotherPoint.latitudeInDegrees)
						* (latitudeInDegrees - anotherPoint.latitudeInDegrees)
						+ (longitudeInDegrees - anotherPoint.longitudeInDegrees)
								* (longitudeInDegrees
										- anotherPoint.longitudeInDegrees));
	} else {
		double earthRadius = 6372000.7976;

		double A = latitudeInRadians;
		double B = longitudeInRadians;
		double C = anotherPoint.latitudeInRadians;
		double D = anotherPoint.longitudeInRadians;

		if ((A == C) && (B == D)) {
			return 0;
		}
		double tmp = sin(A) * sin(C) + cos(A) * cos(C) * cos(B - D);
		if (tmp > 1.0) {
			return earthRadius * acos(1);
		}
		return earthRadius * acos(tmp);
	}
}

patString patCoordinates::getKML() const {
	stringstream str;
	str.precision(15);
	str << longitudeInDegrees << "," << latitudeInDegrees;
	return patString(str.str());
}

ostream& operator<<(ostream &str, const patCoordinates& x) {
	return str << x.latDegree << "d" << x.latMinutes << "'" << x.latSeconds
			<< "\"" << x.latOrientation << "; " << x.lonDegree << "d"
			<< x.lonMinutes << "'" << x.lonSeconds << "\"" << x.lonOrientation;
}

