//----------------------------------------------------------------
// File: patGpsPoint.h
// Author:
// Creation: Thu March 17
//----------------------------------------------------------------

#ifndef patGpsPoint_h
#define patGpsPoint_h

#include "patGeoCoordinates.h"
#include "patArc.h"
#include "patNode.h"
#include "dataStruct.h"

struct base_params {
	struct gps_params gps;
	struct network_params network;
	patTrafficModel* TM;
	patMeasurementModel* MM;
};

class patGpsPoint {
	/**
	 * Compare two GPS points. Order: time, lat, lon
	 * @return true/false.
	 */
	friend bool operator<(const patGpsPoint& g1, const patGpsPoint& g2);

	/**
	 * Export display message.
	 */
	friend ostream& operator<<(ostream& str, const patGpsPoint& x);

public:

	patGpsPoint(unsigned long theUserId, unsigned long theTimeStamp, double lat,
			double lon, double theSpeed, double thehorizonAccuracy,
			double theHeading, double theVerticalAccuracy,
			double theSpeedAccuracy, double theHeadingAccuracy,
			map<string, double>* theAlgoParams);

	/**
	 * Get the time stamp
	 */
	unsigned long getTimeStamp() const;

	/**
	 * Get the speed in km/h.
	 */
	double getSpeed() const;

	/**
	 * Get the speed in m/s.
	 */
	double getSpeedMS() const;

	/**
	 * Get the speed accuracy in m/s.
	 */
	double getSpeedAccuracyMS() const;


	/**
	 * Get speed accuracy in km/h
	 */
	double getSpeedAccuracy() const;

	/**
	 * Set the speed value
	 * @param v: the speed value to be set.
	 */
	void setSpeed(double v);

	/**
	 * Set Speed accuracy.
	 * @param v: The speed accuracy to be set.
	 */
	void setSpeedAccuracy(double v);


	/**
	 * Calculate the maximum speed.
	 */
	double getMaxSpeed(const patGpsPoint* prevGpsPoint) const;



	/**
	 * Get the heading of the GPS.
	 * @return Heading value between 0 and 1.
	 */
	double getHeading() const;

	/**Set the heading of the GPS.
	 * @param v: Heading value to be set.
	 */
	void setHeading(double v) const;
	/**
	 * Get the speed accuracy in km/h.
	 */
	double getHeadingAccuracy() const;


	/**
	 * Calculate the heading to a gps point;
	 */
	double calHeading(const patGpsPoint* nextGpsPoint) const;

	/**
	 * Calculate the headings according to the previous and next GPS point.
	 * @see  calHeading(const patGeoCoordinates* startCoord, const patGeoCoordinates* nextCoord)
	 */
	double calHeading(const patGpsPoint* prevGpsPoint, const patGpsPoint* nextGpsPoint) const;

	/**
	 * Calculate the headings according to the coordinates of previous and next GPS point.
	 */
	double calHeading(const patGeoCoordinates* startCoord,
			const patGeoCoordinates* nextCoord) const;



	/**
	 * Get the pointer to the coordinates
	 */
	const patGeoCoordinates* getGeoCoord() const;
	/**
	 * Get the latitude of the GPS.
	 */
	double getLatitude() const;

	/**
	 * Get the longitude of the GPS.
	 */
	double getLongitude() const;

	/**
	 * Get the horizontal acccuracy
	 */
	double getHorizonAccuracy() const;

	/**
	 * Get the type of the GPS point.
	 * @return "normal" "low speed"
	 */
	string getType() const;

	/**
	 * Set the type of the GPS point according to speed information.
	 * @see getType().
	 */
	void setSpeedType();

	/**
	 * Set the type of the GPS point.
	 * @param theType: the type to be set
	 * @see getType().
	 */
	void setType(string theType);

	/**
	 * Distance to a link.
	 * @param upGeoCoord:the coordinates of the up node.
	 * @param downGeoCoord:the coordinates of the down node.
	 * @return Geometrical information.
	 * ["up"]: distance to the up node;
	 * ["down"]: distance to the down node;
	 * ["length"]: length of the link;
	 * ["position"]: position of the foot;
	 * ["ver"]: vertical distance;
	 * ["link"]: perpendicular distance;
	 */
	map<char*, double> distanceTo(const patGeoCoordinates* upGeoCoord,
			const patGeoCoordinates* downGeoCoord) const;
	/**
	 * Distance to a link.
	 * @param theArc: The pointer to the arc
	 * @see  distanceTo(patGeoCoordinates* upGeoCoord,patGeoCoordinates* downGeoCoord);
	 */
	map<char*, double> distanceTo(const patArc* theArc) const;

	/**
	 * Distance to another GPS ponts.
	 * @return The distance.
	 */
	double distanceTo(const patGpsPoint* aGpsPoint) const;

	/**
	 * Distance to a node.
	 * @return The distance.
	 */
	double distanceTo(const patNode* theNode) const;

	/**
	 * Calculate the perpendicular foot on a arc according the @see distanceToArc().
	 * @return value between 0 and 1
	 */
	double calPerpendicularFootOnArc(map<char*, double> distanceToArc) const;


	pair<bool, double> isGpsPointInZone(const patGpsPoint* prevGpsPoint,
			const patGpsPoint* aGpsPoint) const;

	map<patGpsPoint*, double> detGpsPointsInZone(const patGpsPoint* prevGpsPoint,
			const vector<patGpsPoint>* gpsSequence) const;

	pair<double, double> calSpeedInZone(const patGpsPoint* prevGpsPoint,
			const vector<patGpsPoint>* gpsSequence) const;

	/**
	 * Set gps parameters
	 */
	void setGpsParams(struct gps_params * p, const patGpsPoint* prevGpsPoint,
			const vector<patGpsPoint>* gpsSequence) const;

	patGpsDDR* getDDR();

protected:
	patULong userId;
	patULong timeStamp;
	patReal speed;
	patString type;
	patReal networkAccuracy;
	patReal minPointDDR;
	patReal horizonAccuracy;
	patReal coordsHorizontalAccuracy;
	patReal speedAccuracy;
	patReal heading;
	patReal headingAccuracy;
	patReal verticalAccuracy;
	patReal distanceThreshold;
	patReal distanceThreshold_Loc;
	patReal minNormalSpeed;
	patReal maxNormalSpeedHeading;
	patGeoCoordinates geoCoord;

	patGpsDDR m_ddr;
};

#endif
