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
#include "patTrafficModel.h"
#include "patMeasurementModel.h"
#include "patMeasurement.h"
#include "kml/dom.h"
#include "patTransportMode.h"
using kmldom::PlacemarkPtr;
class patGpsPoint: public patMeasurement {
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

	patGpsPoint(unsigned long theUserId, double theTimeStamp, double lat,
			double lon, double theSpeed, double thehorizonAccuracy,
			double theHeading, double theVerticalAccuracy,
			double theSpeedAccuracy, double theHeadingAccuracy,
			map<string, double>* theAlgoParams);

	/**
	 * Get the speed in km/h.
	 */
	double getSpeed() const;

	bool isGPS() const;
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
	void setHeading(double v);
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
	double calHeading(const patGpsPoint* prevGpsPoint,
			const patGpsPoint* nextGpsPoint) const;

	/**
	 * Calculate the headings according to the coordinates of previous and next GPS point.
	 */
	double calHeading(const patGeoCoordinates* startCoord,
			const patGeoCoordinates* nextCoord) const;

	/**
	 * Calculate the speed according to another GPS point.
	 * @param another_point: the pointer the another patGpsPoint object;
	 */
	double calSpeed(const patGpsPoint* another_point);


	/**
	 * Calculate the speed according to previous and next GPS points.
	 * Take the nearest one in terms of time;
	 */
	double calSpeed(const patGpsPoint* prevGpsPoint,
			const patGpsPoint* nextGpsPoint);
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
	map<string, double> distanceTo(const patGeoCoordinates& upGeoCoord,
			const patGeoCoordinates& downGeoCoord) const;
	/**
	 * Distance to a link.
	 * @param theArc: The pointer to the arc
	 * @see  distanceTo(patGeoCoordinates* upGeoCoord,patGeoCoordinates* downGeoCoord);
	 */
	map<string, double> distanceTo(const patArc* theArc) const;

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
	double calPerpendicularFootOnArc(map<string, double> distanceToArc) const;

	pair<bool, double> isGpsPointInZone(const patGpsPoint* prevGpsPoint,
			const patGpsPoint* aGpsPoint);

	map<patGpsPoint*, double> detGpsPointsInZone(
			const patGpsPoint* prevGpsPoint, vector<patGpsPoint>* gpsSequence);

	pair<double, double> calSpeedInZone(const patGpsPoint* prevGpsPoint,
			vector<patGpsPoint>* gpsSequence);

	/**
	 * Set gps parameters
	 */
	void setGpsParams(struct gps_params * p,
			const patGpsPoint* prevGpsPoint) const;

	void setMeasurementType();

	PlacemarkPtr getKML(int point_id) const;

	MeasurementModelParam getMeasurementModelParam(const patArc* arc,
			TransportMode mode) const;
	double getSimpleValue() const;
protected:
	unsigned long m_userId;
	double m_speed;
	string m_type;
	double m_networkAccuracy;
	double m_minPointDDR;
	double m_horizonAccuracy;
	double m_coordsHorizontalAccuracy;
	double m_speedAccuracy;
	double m_heading;
	double m_headingAccuracy;
	double m_verticalAccuracy;
	double m_distanceThreshold;
	double m_distanceThreshold_Loc;
	double m_minNormalSpeed;
	double m_maxNormalSpeedHeading;
	patGeoCoordinates m_geoCoord;

};

#endif
