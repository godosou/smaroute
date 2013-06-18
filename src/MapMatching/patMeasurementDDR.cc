/*
 * patMeasurementDDR.cc
 *
 *  Created on: Nov 25, 2011
 *      Author: jchen
 */

#include "patMeasurementDDR.h"

#include "patDisplay.h"
//biogeme
//same folder
//Utilities
#include "patNBParameters.h"
#include "patNode.h"
#include "patArc.h"
#include "patErrNullPointer.h"
#include "patErrMiscError.h"

#include <sstream>

#include "kml/dom.h"
using kmldom::KmlFactory;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;
patMeasurementDDR::patMeasurementDDR() :
		m_ddr_arcs(), m_arc_ddr_ranges() {
}

bool patMeasurementDDR::detArcDDR(const patArc* arc, TransportMode mode) {

	double ddr_value = measureDDR(arc, mode);
	cout<<ddr_value<<endl;
	if (ddr_value > 0) {
		m_ddr_arcs[mode];
		m_ddr_arcs[mode].insert(pair<const patArc*, double>(arc, ddr_value));
		m_arc_ddr_ranges[mode];
		m_arc_ddr_ranges[mode].insert(
				pair<const patArc*, struct link_ddr_range>(arc,
						detArcDDRRange(arc, mode)));
		return true;
	} else {
		return false;
	}
}

struct link_ddr_range patMeasurementDDR::detArcDDRRange(const patArc* arc,
		TransportMode mode) const {
	struct link_ddr_range ldr;
	ldr.lower_bound = 0.0;
	ldr.upper_bound = 1.0;
	return ldr;

}

bool patMeasurementDDR::empty() const {
	if (size() == 0) {
		return true;
	} else {
		return false;
	}
}
int patMeasurementDDR::size() const {
	int size = 0;
	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			m_ddr_arcs.begin(); mode_iter != m_ddr_arcs.end(); ++mode_iter) {
		size += mode_iter->second.size();
	}
	return size;
}

bool patMeasurementDDR::empty(TransportMode mode) const {
	map<TransportMode, map<const patArc*, double> >::const_iterator find_mode =
			m_ddr_arcs.find(mode);
	if (find_mode == m_ddr_arcs.end()) {
		return true;
	} else {
		return find_mode->second.empty();
	}
}
int patMeasurementDDR::size(TransportMode mode) const {
	map<TransportMode, map<const patArc*, double> >::const_iterator find_mode =
			m_ddr_arcs.find(mode);
	if (find_mode == m_ddr_arcs.end()) {
		return true;
	} else {
		return find_mode->second.size();
	}
}
set<const patArc*> patMeasurementDDR::detInherentDDR(
		const patMeasurementDDR* prev_ddr, TransportMode mode) {
	set<const patArc*> inherentDDR;
	const map<const patArc*, double>* arc_list = prev_ddr->getDDRArcs(mode);
	if (arc_list == NULL) {
		return inherentDDR;
	}
	cout<<"detect inherent ddr"<<getTransportModeString(mode)<<","<<arc_list->size()<<endl;

	for (map<const patArc*, double>::const_iterator arc_iter =
			arc_list->begin(); arc_iter != arc_list->end(); ++arc_iter) {
		if (detArcDDR(arc_iter->first, mode)) {
			inherentDDR.insert(arc_iter->first);
		}
	}
	return inherentDDR;
}
map<TransportMode, set<const patArc*> > patMeasurementDDR::detInherentDDR(
		const patMeasurementDDR* prev_ddr) {
	//check the element of previous domain is also domain of the current gps point.
	map<TransportMode, set<const patArc*> > inherentDDR;
	//DEBUG_MESSAGE("previous point. "<< prevGpsPoint->getTimeStamp() ) ;

	const map<TransportMode, map<const patArc*, double> >* prev_ddr_arcs =
			prev_ddr->getDDRArcs();
	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			prev_ddr_arcs->begin(); mode_iter != prev_ddr_arcs->end();
			++mode_iter) {
		inherentDDR[mode_iter->first] = detInherentDDR(prev_ddr,
				mode_iter->first);
	}
	return inherentDDR;
}

const map<const patArc*, double>* patMeasurementDDR::getDDRArcs(
		TransportMode mode) const {
	map<TransportMode, map<const patArc*, double> >::const_iterator find_mode =
			m_ddr_arcs.find(mode);
	if (find_mode != m_ddr_arcs.end()) {
		return &find_mode->second;
	}
	return NULL;
}

const map<TransportMode, map<const patArc*, double> >* patMeasurementDDR::getDDRArcs() const {
	return &m_ddr_arcs;
}

set<const patArc*> patMeasurementDDR::getDDRArcSet() const {
	set<const patArc*> arc_set;
	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			m_ddr_arcs.begin(); mode_iter != m_ddr_arcs.end(); ++mode_iter) {
		for (map<const patArc*, double>::const_iterator arc_iter =
				mode_iter->second.begin(); arc_iter != mode_iter->second.end();
				++arc_iter) {

			arc_set.insert(arc_iter->first);
		}
	}
	return arc_set;
}

set<const patArc*> patMeasurementDDR::getDDRArcSet(TransportMode mode) const {
	set<const patArc*> arc_set;
	const map<const patArc*, double>* ddr_arcs = getDDRArcs(mode);
	if (ddr_arcs == NULL) {
		return arc_set;
	}
	for (map<const patArc*, double>::const_iterator arc_iter =
			ddr_arcs->begin(); arc_iter != ddr_arcs->end(); ++arc_iter) {

		arc_set.insert(arc_iter->first);
	}
	return arc_set;
}

bool patMeasurementDDR::isArcInDomain(const patArc* arc,
		TransportMode mode) const {
	map<TransportMode, map<const patArc*, double> >::const_iterator find_mode =
			m_ddr_arcs.find(mode);
	if (find_mode == m_ddr_arcs.end()) {
		return false;
	} else {
		map<const patArc*, double>::const_iterator find_arc =
				find_mode->second.find(arc);
		if (find_arc == find_mode->second.end()) {
			return false;
		} else {
			return true;
		}
	}
}

bool patMeasurementDDR::isRoadInDomain(const patRoadBase* road,
		TransportMode mode) const {
	vector<const patArc*> arc_list = road->getArcList();
	for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
//		DEBUG_MESSAGE(*arc_iter);
		if (isArcInDomain(*arc_iter, mode)) {
			return true;
		}
	}
	return false;
}
struct link_ddr_range patMeasurementDDR::getArcDDRRange(const patArc* arc,
		TransportMode mode, patError*& err) const {

	map<TransportMode, map<const patArc*, struct link_ddr_range> >::const_iterator find_mode =
			m_arc_ddr_ranges.find(mode);
	if (find_mode != m_arc_ddr_ranges.end()) {

		map<const patArc*, struct link_ddr_range>::const_iterator arc_ddr_found =
				find_mode->second.find(arc);
		if (arc_ddr_found != find_mode->second.end()) {
			return arc_ddr_found->second;
		}
	} else {
		err = new patErrMiscError("Arc doesn't in ddr");
//		WARNING(err->describe());
		struct link_ddr_range d;
		return d;

	}
}

FolderPtr patMeasurementDDR::getKML(int i) const {
	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr ddr_folder = factory->CreateFolder();
	stringstream ss;
	ss << i;
	ddr_folder->set_name(ss.str());

	for (map<TransportMode, map<const patArc*, double> >::const_iterator mode_iter =
			m_ddr_arcs.begin(); mode_iter != m_ddr_arcs.end(); ++mode_iter) {

		FolderPtr mode_folder = factory->CreateFolder();
		mode_folder->set_name(getTransportMode(mode_iter->first));
		mode_folder->set_styleurl("#" + getTransportMode(mode_iter->first));

		for (map<const patArc*, double>::const_iterator arc_iter =
				mode_iter->second.begin(); arc_iter != mode_iter->second.end();
				++arc_iter) {

			stringstream v;
			v << arc_iter->second;

			vector<PlacemarkPtr> arc_pts = (arc_iter->first)->getArcKML(
					getTransportMode(mode_iter->first));

			for (vector<PlacemarkPtr>::const_iterator pt_iter = arc_pts.begin();
					pt_iter != arc_pts.end(); ++pt_iter) {

				PlacemarkPtr arc = *pt_iter;
				arc->set_name(v.str());
				mode_folder->add_feature(arc);
			}

		}
		ddr_folder->add_feature(mode_folder);
	}
	return ddr_folder;
}

double patMeasurementDDR::getArcDDRValue(const patArc* arc,
		TransportMode mode) const {
	map<TransportMode, map<const patArc*, double> >::const_iterator find_mode =
			m_ddr_arcs.find(mode);
	if (find_mode != m_ddr_arcs.end()) {

		map<const patArc*, double>::const_iterator arc_ddr_found =
				find_mode->second.find(arc);
		if (arc_ddr_found != find_mode->second.end()) {
			return arc_ddr_found->second;
		}
	} else {

		return 0.0;
	}

}

double patMeasurementDDR::computePathDDRRaw(
		const patMultiModalPath& path) const {
	double rtn_value = 0.0;

	if (empty()) {
		DEBUG_MESSAGE("empty domain");
		return rtn_value;
	}
	list<RoadTravel> roads = path.getRoadTravelList();
	for (list<RoadTravel>::const_iterator road_iter = roads.begin();
			road_iter != roads.end(); ++road_iter) {
		vector<const patArc*> arc_list = (*road_iter).road->getArcList();
		TransportMode mode = (*road_iter).mode;
		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
//			DEBUG_MESSAGE((*arc_iter)->getLength()<< " "<<getArcDDRValue(*arc_iter, mode));
			rtn_value += (*arc_iter)->getLength()
					* getArcDDRValue(*arc_iter, mode);
		}

	}

	return rtn_value;
}
void patMeasurementDDR::finalize(
		const patNetworkEnvironment* network_environment) {

	map<const patArc*, pair<double, struct link_ddr_range> > arc_ddr_set_temp;
	for (map<TransportMode, map<const patArc*, struct link_ddr_range> >::const_iterator mode_iter =
			m_arc_ddr_ranges.begin(); mode_iter != m_arc_ddr_ranges.end();
			++mode_iter) {
		for (map<const patArc*, struct link_ddr_range>::const_iterator arc_iter =
				mode_iter->second.begin(); arc_iter != mode_iter->second.end();
				++arc_iter) {
			arc_ddr_set_temp[arc_iter->first] = pair<double,
					struct link_ddr_range>(
					m_ddr_arcs[mode_iter->first][arc_iter->first],
					arc_iter->second);
		}

	}
	for (map<string, patNetworkBase*>::const_iterator network_iter =
			network_environment->getAllNetworks()->begin();
			network_iter != network_environment->getAllNetworks()->end();
			++network_iter) {
		m_ddr_arcs[network_iter->second->getTransportMode()];
		m_arc_ddr_ranges[network_iter->second->getTransportMode()];
		for (map<const patArc*, pair<double, struct link_ddr_range> >::const_iterator arc_iter =
				arc_ddr_set_temp.begin(); arc_iter != arc_ddr_set_temp.end();
				++arc_iter) {
			if (!network_iter->second->getRoadsContainArc(arc_iter->first).empty()) {
				m_ddr_arcs[network_iter->second->getTransportMode()][arc_iter->first] =
						arc_iter->second.first;
				m_arc_ddr_ranges[network_iter->second->getTransportMode()][arc_iter->first] =
						arc_iter->second.second;
			}
		}

	}

}
