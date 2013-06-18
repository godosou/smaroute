/*
 * patMeasurement.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATMEASUREMENT_H_
#define PATMEASUREMENT_H_

#include "dataStruct.h"
#include "patArc.h"
#include "patTransportMode.h"
#include "kml/dom.h"
using kmldom::PlacemarkPtr;

enum MeasurementType {
	GPS, BT, ACCEL
};

class patMeasurementDDR;
class patMeasurement {
public:
	patMeasurement();
	MeasurementType getMeasurementType() const;
	virtual void setMeasurementType() =0;
	virtual bool isGPS() const = 0;
	double getTimeStamp() const;
	patMeasurementDDR* getDDR();
	virtual ~patMeasurement();
	virtual MeasurementModelParam getMeasurementModelParam(const patArc* arc,
			TransportMode mode) const = 0;
	virtual double getSimpleValue() const = 0;

	virtual PlacemarkPtr getKML(int point_id) const = 0;
protected:
	double m_timestamp;
	MeasurementType m_measurement_type;
	patMeasurementDDR* m_ddr;
};

#endif /* PATMEASUREMENT_H_ */
