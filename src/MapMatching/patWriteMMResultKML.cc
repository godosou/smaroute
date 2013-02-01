/*
 * patWriteMMResultKML.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: jchen
 */

#include "patWriteMMResultKML.h"
#include "patMeasurementDDR.h"
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;

patWriteMMResultKML::patWriteMMResultKML(string file_name) :
		patKMLPathWriter::patKMLPathWriter(file_name) {

}
void patWriteMMResultKML::writeMeasurements(
		const std::vector<patMeasurement*>* current_measurments) {

	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr gps_folder = factory->CreateFolder();
	FolderPtr ddr_folder = factory->CreateFolder();

	ddr_folder->set_name(string("DDR"));
	gps_folder->set_name(string("GPS"));
	for (unsigned i = 0; i < current_measurments->size(); ++i) {

		gps_folder->add_feature(current_measurments->at(i)->getKML(i + 1));
		ddr_folder->add_feature(
				current_measurments->at(i)->getDDR()->getKML(i + 1));
	}
	m_document->add_feature(gps_folder);
	m_document->add_feature(ddr_folder);

}

patWriteMMResultKML::~patWriteMMResultKML() {
}

