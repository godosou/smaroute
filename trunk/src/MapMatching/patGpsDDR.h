/*
 * patGpsDDR.h
 *
 *  Created on: Oct 12, 2011
 *      Author: jchen
 */

#ifndef PATGPSDDR_H_
#define PATGPSDDR_H_

#include <map>
//Utilities
#include "dataStruct.h"

//NetworkHandler
#include "patArc.h"
#include "patMeasurementDDR.h"
#include "kml/dom.h"
class patArc;
using kmldom::FolderPtr;
//Objects
class patGpsPoint;
class patGpsDDR: public patMeasurementDDR {
public:
	patGpsDDR(patGpsPoint* the_gps);
	virtual ~patGpsDDR();
	/**
	 * Detect whether an arc in DDR or not
	 */
	bool detArcDDR(const patArc* arc, TransportMode mode);

	double measureDDR(const patArc* arc, TransportMode mode);
	/**
	 * Detect the DDR range of the arc
	 * @param arc_distance the arc distance
	 * @return link_ddr_range
	 */
	struct link_ddr_range detArcDDRRange(map<string, double> arc_distance) const;

	/**
	 * Calculate arc DDR.
	 * @param distance_to_arc the distance to arc
	 * @param ldr link_ddr_range struct
	 * @return double value.
	 */
	double measureDDR(map<string, double> distance_to_arc
			, link_ddr_range ldr) const;

	/**
	 * Calculate maximum possible distance traveled since the previous GPS point.
	 * @param prevGpsPoint The previous GPS point
	 * @return The distance
	 */
	double getDistanceCeilFromPrevDDR(const patGpsPoint* prevGpsPoint) const;

protected:
	double m_min_point_ddr;
	double m_horizon_accuracy;
	double m_distance_threshold_loc;
	patGpsPoint* m_gps;
};

#endif /* PATGPSDDR_H_ */
