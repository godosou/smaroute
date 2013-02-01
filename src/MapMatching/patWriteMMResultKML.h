/*
 * patWriteMMResultKML.h
 *
 *  Created on: Aug 21, 2012
 *      Author: jchen
 */

#ifndef PATWRITEMMRESULTKML_H_
#define PATWRITEMMRESULTKML_H_

#include "patKMLPathWriter.h"
#include <map>
#include "kml/dom.h"
#include "patMeasurement.h"
#include <vector>
using kmldom::FolderPtr;
class patWriteMMResultKML: public patKMLPathWriter {
public:
	patWriteMMResultKML(string file_name);
	void writeMeasurements(const std::vector<patMeasurement*>* current_measurments);
	virtual ~patWriteMMResultKML();
};

#endif /* PATWRITEMMRESULTKML_H_ */
