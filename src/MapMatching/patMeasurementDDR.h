/*
 * patMeasurementDDR.h
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#ifndef PATMEASUREMENTDDR_H_
#define PATMEASUREMENTDDR_H_
#include "patMeasurement.h"
#include "patArc.h"
#include <set>
#include "patRoadBase.h"
#include "dataStruct.h"
#include <map>
#include "kml/dom.h"
#include "patMultiModalPath.h"
#include "patTransportMode.h"
#include "patNetworkEnvironment.h"
using kmldom::FolderPtr;

class patMeasurementDDR {
public:
	patMeasurementDDR();
	void setMeasurement();
	virtual bool  detArcDDR(const patArc* arc, TransportMode mode)=0;

	struct link_ddr_range  detArcDDRRange(const patArc* arc,
			TransportMode mode) const;
	bool  empty() const;
	int  size() const;

	bool  empty(TransportMode mode) const;

	int  size(TransportMode mode) const;
	set<const patArc*>  detInherentDDR(
			const patMeasurementDDR* prev_ddr, TransportMode mode);
	map<TransportMode, set<const patArc*> >  detInherentDDR(
			const patMeasurementDDR* prev_ddr);

	const map<const patArc*, double>*  getDDRArcs(TransportMode mode) const;

	const map<TransportMode, map<const patArc*, double> >*  getDDRArcs() const;
	set<const patArc*>  getDDRArcSet() const;

	set<const patArc*>  getDDRArcSet(
			TransportMode mode) const;
	bool  isArcInDomain(const patArc* arc,
			TransportMode mode) const;
	bool  isRoadInDomain(const patRoadBase* road,
			TransportMode mode) const;
	struct link_ddr_range  getArcDDRRange(const patArc* arc,
			TransportMode mode, patError*& err) const;

	double getArcDDRValue(const patArc* arc, TransportMode mode) const;
	FolderPtr  getKML(int i) const;

	double computePathDDRRaw(const patMultiModalPath& path) const;

	void finalize(const patNetworkEnvironment* network_environment);
	virtual double measureDDR(const patArc* arc, TransportMode mode)=0;
protected:
	map<TransportMode, map<const patArc*, double> > m_ddr_arcs;
	map<TransportMode, map<const patArc*, struct link_ddr_range> > m_arc_ddr_ranges;

};

#endif /* PATMEASUREMENTDDR_H_ */
