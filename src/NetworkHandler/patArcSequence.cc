/*
 * patArcSequence.cc
 *
 *  Created on: Nov 15, 2011
 *      Author: jchen
 */

#include "patArcSequence.h"
#include "patDisplay.h"
#include "patException.h"
using kmldom::KmlFactory;
using kmldom::FolderPtr;

patArcSequence::patArcSequence() {

	m_length = 0.0;
}
bool operator==(const patArcSequence& seq_a, const patArcSequence& seq_b) {
	vector<const patArc*> arc_list_a = seq_a.getArcList();
	vector<const patArc*> arc_list_b = seq_b.getArcList();
	if (arc_list_a.size() != arc_list_b.size()) {
		return false;
	}
	vector<const patArc*>::iterator iter_a = arc_list_a.begin();
	vector<const patArc*>::iterator iter_b = arc_list_b.begin();
	while (iter_a != arc_list_a.end()) {
		if (*iter_a != *iter_b) {
			return false;
		}
		++iter_a;
		++iter_b;
	}
	return true;

}
const patNode* patArcSequence::containLoop() const {
	set<const patNode*> nodeSet;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if (nodeSet.find((*arcIter)->getUpNode()) == nodeSet.end()) {
			nodeSet.insert((*arcIter)->getUpNode());
		} else {
			return (*arcIter)->getUpNode();
		}
	}
	if (nodeSet.find(m_arcs.back()->getDownNode()) == nodeSet.end()) {
		return NULL;
	} else {
		return m_arcs.back()->getDownNode();
	}
}

double patArcSequence::getNodeLoopCount() const {
	set<const patNode*> nodeSet;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		nodeSet.insert((*arcIter)->getUpNode());
		nodeSet.insert((*arcIter)->getDownNode());
	}
	return m_arcs.size() + 1 - nodeSet.size();//FIXME
}
double patArcSequence::computeLength() {
	m_length = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		//DEBUG_MESSAGE((*arcIter)->getLength());
		m_length += (*arcIter)->getLength();
	}

	return m_length;
}

double patArcSequence::getLength() const {
	return m_length;
}

/**
 * return the pointer to the up node
 */
const patNode* patArcSequence::getUpNode() const {

	//FIXME call this function can cause NULL pointer
	if (m_arcs.empty()) {
		return NULL;
	}
	return m_arcs.front()->getUpNode();
}

/**
 * return the pointer to the down node
 */
const patNode* patArcSequence::getDownNode() const {
	//FIXME call this function can cause NULL pointer
	if (m_arcs.empty()) {
		return NULL;
	}
	return m_arcs.back()->getDownNode();
}

vector<const patArc*> patArcSequence::getArcList() const {
	return m_arcs;
}
bool patArcSequence::isValid() const {
	if(m_arcs.empty()){
		return true;
	}
	vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
	const patNode* connection_node_up = (*arc_iter)->getDownNode();
	++arc_iter;
	for (; arc_iter != m_arcs.end(); ++arc_iter) {
		//DEBUG_MESSAGE(*connection_node_up);
		if ((*arc_iter)->getUpNode() != connection_node_up) {
			return false;
		}
		connection_node_up = (*arc_iter)->getDownNode();
	}
	return true;
}

bool patArcSequence::isLinkInPath(const patArc* theArc) const {

	bool found = false;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if (*arcIter == theArc) {
			return true;
		}
	}

	return found;
}

bool patArcSequence::isNodeInPath(const patNode* node) const {

	if (m_arcs.empty()) {
		return false;
	}
	vector<const patArc*>::const_iterator arc_iter = m_arcs.end();
	if (node == back()->getDownNode()) {
		return true;
	}

	while (arc_iter != m_arcs.begin()) {
		--arc_iter;
		if (node == (*arc_iter)->getUpNode()) {
//			DEBUG_MESSAGE("OK"<<node);
			return true;
		}
	}
	return false;

}
set<const patArc*> patArcSequence::getDistinctArcs() {
	set<const patArc*> arcSet;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		arcSet.insert(*arcIter);

	}
	return arcSet;
}
int patArcSequence::size() const {
	return m_arcs.size();
}
const patArc* patArcSequence::getArc(unsigned long iArc) const {
	vector<const patArc*>::const_iterator arcIter = m_arcs.begin();

	for (unsigned long i = 0; i <= iArc; ++i) {
		arcIter++;
	}

	return *arcIter;
}

const patArc* patArcSequence::deleteArcFromBack() {

	if (m_arcs.empty() == true) {
		return NULL;
	}
	const patArc* backArc = m_arcs.back();
	m_arcs.pop_back();
	return backArc;
}

const patArc* patArcSequence::front() const {
	if (m_arcs.empty() == true) {
		return NULL;
	}

	return m_arcs.front();

}

const patArc* patArcSequence::back() const {

	if (m_arcs.empty() == true) {
		return NULL;
	}

	return m_arcs.back();
}

const patArc* patArcSequence::back2() const {

	if (m_arcs.size() < 2) {
		return NULL;
	}
	vector<const patArc*>::const_iterator arcIter = m_arcs.end();
	arcIter--;
	arcIter--;
	return *arcIter;
}

bool patArcSequence::addArcToFront(const patArc* arc) {
	if (arc == NULL) {
		return false;
	}

	if (!m_arcs.empty() && getUpNode() != arc->getDownNode()) {
		return false;
	} else {
		m_arcs.insert(m_arcs.begin(), arc);
		m_length += arc->getLength();
		return true;
	}
}
bool patArcSequence::addArcToBack(const patArc* arc) {
	if (arc == NULL) {
		DEBUG_MESSAGE("NULL arc");
		return false;
	}

	if (!m_arcs.empty() && getDownNode() != arc->getUpNode()) {
		WARNING("arcs not consistent: "<<m_arcs.size());
		WARNING(getDownNode()->getUserId());
		WARNING( arc->getUpNode()->getUserId());
		return false;
	} else {
		m_arcs.push_back(arc);
		m_length += arc->getLength();
		return true;
	}
	return true;
}

bool patArcSequence::addArcsToBack(const vector<const patArc*>* arc_list) {
	for (vector<const patArc*>::const_iterator arc_iter = arc_list->begin();
			arc_iter != arc_list->end(); ++arc_iter) {
		if (addArcToBack(*arc_iter)==false) {
			return false;
		}
	}
	return true;
}
bool patArcSequence::addArcsToBack(const patRoadBase* road) {

	if (road->isValid() == false) {
		return false;
	}
	vector<const patArc*> arc_list = road->getArcList();

	for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		if (addArcToBack(*arc_iter) == false) {
			return false;
		}
	}
	return true;
}

bool patArcSequence::addArcsToFront(const patRoadBase* road) {
	if (road->isValid() == false) {
		return false;
	}
	vector<const patArc*> arc_list = road->getArcList();
	vector<const patArc*>::const_iterator arc_iter = arc_list.end();
	while (arc_iter != arc_list.begin()) {
		++arc_iter;
		if (addArcToFront(*arc_iter) == false) {
			return false;
		}
	}
	return true;
}

bool patArcSequence::empty() const {
	return m_arcs.empty();
}

ostream& operator<<(ostream& str, const patArcSequence& x) {
	vector<const patArc*> arc_list = x.getArcList();
	for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		str << **arc_iter << "\n";
	}
}
FolderPtr patArcSequence::getKML() const {
	vector<const patArc*> arc_list = getArcList();
	KmlFactory* factory = KmlFactory::GetFactory();

	FolderPtr kml_folder = factory->CreateFolder();
	for (vector<const patArc*>::iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {

		kml_folder->add_feature((*arc_iter)->getArcKML(""));
	}
	return kml_folder;

}

const patNode* patArcSequence::getNode(int index) const {
	if (index > m_arcs.size()) {
		WARNING("index greater than size"<<index<<">"<<m_arcs.size());
		throw RuntimeException("index greater than size");
		return NULL;
	} else if (index == m_arcs.size()) {
		return m_arcs.back()->getDownNode();
	} else {
		return m_arcs[index]->getUpNode();
	}

}

