/*
 * patBTMeasurement.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATBTMEASUREMENT_H_
#define PATBTMEASUREMENT_H_
#include "patMeasurement.h"
#include "patMeasurementDDR.h"
#include "dataStruct.h"
#include "patArc.h"
#include "patTransportMode.h"
#include "kml/dom.h"
using kmldom::PlacemarkPtr;
class patBTMeasurement : public patMeasurement{
public:
	patBTMeasurement(double bt_time, int bt_amount);
	int getNearbyBTAmount() const;
		friend ostream& operator<<(ostream& str, const patBTMeasurement& x);

bool isGPS() const;
	void setMeasurementType();
	virtual ~patBTMeasurement();
	 MeasurementModelParam getMeasurementModelParam(const patArc* arc, TransportMode mode) const;
	 double getSimpleValue() const;
protected:
	double m_nearby_bt_amount;
	 PlacemarkPtr getKML(int point_id) const{
		 return patMeasurement::getKML(point_id);
	}
};

#endif /* PATBTMEASUREMENT_H_ */
