/*
 * patRoadBase.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#include "patRoadBase.h"
#include "kml/dom.h"
using kmldom::FolderPtr;
using kmldom::KmlFactory;
using kmldom::PlacemarkPtr;
#include "patArc.h"
patRoadBase::patRoadBase() {
	m_length = 0.0;
}

void patRoadBase::setLength(bool the_length) {

	m_length = the_length;
}
patRoadBase::patRoadBase(const patRoadBase& another) :
		m_length(another.m_length) {

}
patRoadBase::~patRoadBase() {

}

 double patRoadBase::getLength() const{
	return m_length;
}
FolderPtr patRoadBase::getKML(string mode) const {
	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr road_folder = factory->CreateFolder();
	vector<const patArc*> arc_list = getArcList();
	for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		road_folder->add_feature((*arc_iter)->getArcKML(mode));
	}
	return road_folder;

}
