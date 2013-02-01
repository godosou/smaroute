/*
 * patCompressedArc.cc
 *
 *  Created on: Jul 20, 2012
 *      Author: jchen
 */

#include "patCompressedArc.h"
#include "patDisplay.h"

#include <boost/lexical_cast.hpp>
double patCompressedArc::computeGeneralizedCost() {
	m_generalized_cost = 0.0;

	for (vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
			arc_iter != m_arcs.end(); ++arc_iter) {
		m_generalized_cost += (*arc_iter)->getGeneralizedCost();
	}
	return m_generalized_cost;
}

vector<PlacemarkPtr> patCompressedArc::getArcKML(string mode) const {

	vector<PlacemarkPtr> rtn;
	for (vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
			arc_iter != m_arcs.end(); ++arc_iter) {

		vector<PlacemarkPtr> arc_pts = (*arc_iter)->getArcKML(mode);
		rtn.insert(rtn.end(), arc_pts.begin(), arc_pts.end());
	}
	return rtn;
}
patCompressedArc::patCompressedArc(const vector<const patRoadBase*>& roads) {

	for (vector<const patRoadBase*>::const_iterator road_iter = roads.begin();
			road_iter != roads.end(); ++road_iter) {

		vector<const patArc*> arc_list = (*road_iter)->getArcList();

		for (vector<const patArc*>::const_iterator p_arc_iter = arc_list.begin();
				p_arc_iter != arc_list.end(); ++p_arc_iter) {

			if ((*p_arc_iter) == NULL) {
				throw RuntimeException("NULL arc");
			}
			vector<const patArc*> original_arc_list =
					(*p_arc_iter)->getOriginalArcList();
			for (vector<const patArc*>::const_iterator arc_iter =
					arc_list.begin(); arc_iter != arc_list.end(); ++arc_iter) {
				if (!m_arcs.empty()
						&& m_arcs.back()->getDownNode()
								!= (*arc_iter)->getUpNode()) {
					throw RuntimeException(" arcs not consistent");
				} else {
					m_arcs.push_back(*arc_iter);
//				m_length += (*arc_iter)->getLength();
//
//				m_arc_string+=(*arc_iter)->getArcString();
//				m_generalized_cost += (*arc_iter)->getGeneralizedCost();
//				DEBUG_MESSAGE((*arc_iter)->getGeneralizedCost()<<m_generalized_cost);
				}
			}
		}
	}
	if (m_arcs.empty()) {
		throw RuntimeException("Invalide compressed arc");
	}
	summarize();
}
void patCompressedArc::summarize() {

	m_length = 0.0;
	m_generalized_cost = 0.0;
	m_arc_string = "";
	for (vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
			arc_iter != m_arcs.end(); ++arc_iter) {
		m_generalized_cost += (*arc_iter)->getGeneralizedCost();
		m_arc_string += (*arc_iter)->getArcString();
		m_length += (*arc_iter)->getLength();
	}
	if (m_generalized_cost == 0.0) {
		cout << "patCompressedArc: zero generalized cost"<<m_arc_string << endl;

	}
//	cout<<m_generalized_cost<<endl;
//	cout<<"compressed arc:"<<m_arc_string;
	m_up_node = m_arcs.front()->getUpNode();
	m_down_node = m_arcs.back()->getDownNode();
	m_user_id = m_arcs.front()->getUserId();

}
patCompressedArc::~patCompressedArc() {

}

double patCompressedArc::getAttribute(
		ARC_ATTRIBUTES_TYPES attribute_name) const {
	double rtn_value = 0.0;

	for (vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
			arc_iter != m_arcs.end(); ++arc_iter) {
		rtn_value += (*arc_iter)->getAttribute(attribute_name);
	}
	return rtn_value;
}

vector<const patArc*> patCompressedArc::getOriginalArcList() const {
	return m_arcs;
}
