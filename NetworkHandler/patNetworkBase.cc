/*
 * patNetworkBase.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#include "patNetworkBase.h"
#include <shapefil.h>
#include "patDisplay.h"
#include "patCreateKmlStyles.h"
#include<iostream>
#include<sstream>
#include<fstream>
#include "patNetworkBase.h"
#include "patTransportMode.h"
#include "kml/dom.h"
using kmldom::DocumentPtr;
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;

using kmldom::StylePtr;
using kmldom::LineStylePtr;
patNetworkBase::patNetworkBase() :
		m_minimum_label(patMaxReal) {

}

unsigned long patNetworkBase::getNodeSize() const {
	return m_outgoing_incidents.size();
}
patNetworkBase::~patNetworkBase() {
}

const map<const patNode*, set<const patRoadBase*> >* patNetworkBase::getOutgoingIncidents() {
	return &m_outgoing_incidents;
}

bool patNetworkBase::hasDownStream(const patNode * const a_node) const {

	map<const patNode*, set<const patRoadBase*> >::const_iterator find_node =
			m_outgoing_incidents.find(a_node);
	if (!find_node->second.empty()) {
		return true;
	} else {
		return false;
	}
}
TransportMode patNetworkBase::getTransportMode() const {
	return m_transport_mode;
}
double patNetworkBase::computeMinimumLabel() {

	for (map<const patNode*, set<const patRoadBase*> >::const_iterator n_iter =
			m_outgoing_incidents.begin(); n_iter != m_outgoing_incidents.end();
			++n_iter) {
		for (set<const patRoadBase*>::const_iterator road_iter =
				n_iter->second.begin(); road_iter != n_iter->second.end();
				++road_iter) {
			m_minimum_label =
					(m_minimum_label < (*road_iter)->getLength()) ?
							m_minimum_label : (*road_iter)->getLength();
		}
	}
	return m_minimum_label;
}
double patNetworkBase::getMinimumLabel() {

	return m_minimum_label;
}

void patNetworkBase::finalizeNetwork() {
	computeMinimumLabel();
}

void patNetworkBase::exportKML(const string file_path) const {
	string kml_file_path = file_path + ".kml";
	ofstream kml_file(kml_file_path.c_str());
	patCreateKmlStyles doc;
	DocumentPtr document = doc.createStylesForKml();

	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr stop_folder = factory->CreateFolder();
	FolderPtr links_folder = factory->CreateFolder();

	stop_folder->set_name(string("Stop"));
	links_folder->set_name(string("Links"));
	for (map<const patNode*, set<const patRoadBase*> >::const_iterator incid_iter =
			m_outgoing_incidents.begin();
			incid_iter != m_outgoing_incidents.end(); ++incid_iter) {
		PlacemarkPtr node = incid_iter->first->getKML();
		node->set_styleurl("#stop");
		stop_folder->add_feature(node);

		for (set<const patRoadBase*>::const_iterator road_iter =
				incid_iter->second.begin();
				road_iter != incid_iter->second.end(); ++road_iter) {
			FolderPtr road = (*road_iter)->getKML(
					getTransportModeString(m_transport_mode));
			links_folder->add_feature(road);
		}
	}

	document->add_feature(stop_folder);
	document->add_feature(links_folder);
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(document);

	kml_file << kmldom::SerializePretty(kml);
	kml_file.close();
	DEBUG_MESSAGE(kml_file_path << " written");
}
bool patNetworkBase::exportShpFiles(const string file_path) const {

	string shape_file_path = file_path + ".shp";
	string stop_file_path = file_path + "_stops.shp";
	SHPHandle shp_file_handler = SHPCreate(shape_file_path.c_str(), SHPT_ARC);
	SHPHandle stop_file_handler = SHPCreate(stop_file_path.c_str(), SHPT_POINT);

	DEBUG_MESSAGE("writing path to" << shape_file_path);
	for (map<const patNode*, set<const patRoadBase*> >::const_iterator incid_iter =
			m_outgoing_incidents.begin();
			incid_iter != m_outgoing_incidents.end(); ++incid_iter) {

		double node_padfX[1] = {
				incid_iter->first->getGeoCoord().longitudeInDegrees };
		double node_padfY[1] = {
				incid_iter->first->getGeoCoord().latitudeInDegrees };
		SHPObject* stop_shp_object = SHPCreateSimpleObject(SHPT_POINT, 2,
				node_padfX, node_padfY, NULL);
		int stop_number = SHPWriteObject(stop_file_handler, -1,
				stop_shp_object);
		SHPDestroyObject(stop_shp_object);

		for (set<const patRoadBase*>::const_iterator road_iter =
				incid_iter->second.begin();
				road_iter != incid_iter->second.end(); ++road_iter) {
			list<const patArc*> arc_list = (*road_iter)->getArcList();
			for (list<const patArc*>::iterator arc_iter = arc_list.begin();
					arc_iter != arc_list.end(); ++arc_iter) {
				if (*arc_iter == NULL) {
					continue;
				}
				double up_node_x =
						(*arc_iter)->getUpNode()->getGeoCoord().longitudeInDegrees;
				double up_node_y =
						(*arc_iter)->getUpNode()->getGeoCoord().latitudeInDegrees;
				double down_node_x =
						(*arc_iter)->getDownNode()->getGeoCoord().longitudeInDegrees;
				double down_node_y =
						(*arc_iter)->getDownNode()->getGeoCoord().latitudeInDegrees;

				double padfX[2] = { up_node_x, down_node_x };
				double padfY[2] = { up_node_y, down_node_y };

				SHPObject* path_shp_object = SHPCreateSimpleObject(SHPT_ARC, 2,
						padfX, padfY, NULL);
				int object_number = SHPWriteObject(shp_file_handler, -1,
						path_shp_object);
				SHPDestroyObject(path_shp_object);
			}
		}
	}

	SHPClose(shp_file_handler);
	SHPClose(stop_file_handler);
	DEBUG_MESSAGE("shape files written");
	return true;
}

set<const patArc*> patNetworkBase::getAllArcs() const {
	set<const patArc*> all_arcs;
	for (map<const patNode*, set<const patRoadBase*> >::const_iterator node_iter =
			m_outgoing_incidents.begin();
			node_iter != m_outgoing_incidents.end(); ++node_iter) {
		for (set<const patRoadBase*>::const_iterator road_iter =
				node_iter->second.begin(); road_iter != node_iter->second.end();
				++road_iter) {
			list<const patArc*> arc_list = (*road_iter)->getArcList();
			all_arcs.insert(arc_list.begin(), arc_list.end());
		}
	}
	return all_arcs;
}

