/*
 * patPublicTransportSegment.cc
 *
 *  Created on: Jul 22, 2011
 *      Author: jchen
 */

#include "patPublicTransportSegment.h"
#include "patNetworkElements.h"
#include "patDisplay.h"
patPublicTransportSegment::patPublicTransportSegment() {
	m_length = -1.0;
}

patPublicTransportSegment::~patPublicTransportSegment() {
	//
}

bool operator==(const patPublicTransportSegment& seg_a, const patPublicTransportSegment& seg_b){
	vector<const patArc*> arc_list_a = seg_a.getArcList();
	vector<const patArc*> arc_list_b = seg_b.getArcList();
	if (arc_list_a.size()!=arc_list_b.size()){
		return false;
	}
	vector<const patArc*>::iterator iter_a =arc_list_a.begin();
	vector<const patArc*>::iterator iter_b =arc_list_b.begin();
	while(iter_a!=arc_list_a.end()){
		if(*iter_a != *iter_b){
			return false;
		}
		++iter_a;
		++iter_b;
	}
	return true;

}
bool operator<(const patPublicTransportSegment& seg_a, const patPublicTransportSegment& seg_b){
	if(seg_a.getLength()<seg_b.getLength()){
		return true;
	}
	if(seg_a.getLength()>seg_b.getLength()){
			return false;
	}
	if(seg_a.size()<seg_b.size()){
		return true;
	}
	if(seg_a.size()>seg_b.size()){
			return false;
	}

	vector<const patArc*> arc_list_a = seg_a.getArcList();
	vector<const patArc*> arc_list_b = seg_b.getArcList();
	vector<const patArc*>::iterator iter_a =arc_list_a.begin();
	vector<const patArc*>::iterator iter_b =arc_list_b.begin();
	while(iter_a!=arc_list_a.end()){
		if(*iter_a < *iter_b){
			return true;
		}
		else if(*iter_a > *iter_b){
			return false;
		}
		++iter_a;
		++iter_b;
	}
	return false;

}
ostream& operator<<(ostream& str, const patPublicTransportSegment& x) {
	vector<const patArc*> arc_list = x.getArcList();
	for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		str << **arc_iter << "\n";
	}
}

 bool patPublicTransportSegment::addArcToBack(const patArc* arc,
		int direction) {
	if (m_directions.back() != 3 && direction != 3) {
		if (m_directions.back() != direction) {
			return false;
		}
	}
	if (patArcSequence::addArcToBack(arc) == false) {
		return false;
	}
	m_directions.push_back(direction);
	return true;
}
 const patArc* patPublicTransportSegment::deleteArcToBack() {

	if (m_arcs.empty() || m_directions.empty()) {
		return NULL;
	}
	const patArc* backArc = m_arcs.back();
	m_arcs.pop_back();
	m_directions.pop_back();
	return backArc;
}
