/*
 * patAccelMeasurement.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATACCELMEASUREMENT_H_
#define PATACCELMEASUREMENT_H_
#include "patMeasurement.h"
#include "dataStruct.h"
#include "patArc.h"
#include "patTransportMode.h"
#include "kml/dom.h"
using kmldom::PlacemarkPtr;
class patAccelMeasurement: public patMeasurement {
public:
	patAccelMeasurement(double accel_time, double accel);

 	friend ostream& operator<<(ostream& str, const patAccelMeasurement& x);
	double getAccel() const;
	void setMeasurementType();
	MeasurementModelParam getMeasurementModelParam(const patArc* arc,
			TransportMode mode) const;
	double getSimpleValue() const;
	bool isGPS() const;
	PlacemarkPtr getKML(int point_id) const{
		 return patMeasurement::getKML(point_id);
	}
protected:
	double m_accel;
};

#endif /* PATACCELMEASUREMENT_H_ */
