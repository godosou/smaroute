/*
 * patMeasurement.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patMeasurement.h"
#include "patMeasurementDDR.h"

#include "kml/dom.h"
using kmldom::KmlFactory;
using kmldom::PlacemarkPtr;
patMeasurement::patMeasurement() {

}

patMeasurement::~patMeasurement() {
	delete m_ddr;
	m_ddr = NULL;
}

MeasurementType patMeasurement::getMeasurementType() const {
	return m_measurement_type;
}

double patMeasurement::getTimeStamp() const {
	return m_timestamp;
}


patMeasurementDDR* patMeasurement::getDDR() {
	return m_ddr;
}

PlacemarkPtr patMeasurement::getKML(int point_id) const {

	KmlFactory* factory = KmlFactory::GetFactory();
	PlacemarkPtr placemark = factory->CreatePlacemark();
	stringstream ss;
	ss << point_id;
	placemark->set_name(ss.str());
	stringstream desc;
	desc << getMeasurementType()<< " "<<"time:"<<getTimeStamp()<<", "<<getSimpleValue();
	placemark->set_description(desc.str());
	return placemark;
}

