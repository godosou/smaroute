#include "patPathJ.h"
#include "patConst.h"
#include "patDisplay.h"
#include "patPower.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patNBParameters.h"
#include <shapefil.h>
#include "patWay.h"

#include <list>

#include <fstream>
#include <sstream>
patPathJ::patPathJ(list<pair<patArc*, TransportMode> > the_arc_list) :
		m_name(patString("No name")), m_id(-1) {

	m_length = -1;
	for (list<pair<patArc*, TransportMode> >::iterator arc_iter =
			the_arc_list.begin(); arc_iter != the_arc_list.end(); ++arc_iter) {
		addArcToBack(arc_iter->first, arc_iter->second);
	}
	detChangePoints();

}
patPathJ::patPathJ() :
		m_name(patString("No name")), m_id(-1) {
	m_length = -1;

}

bool patPathJ::empty() {
	return m_arcs.empty();
}

patPathJ::patPathJ(list<patArc*> theListOfArcs) :
		m_name(patString("No name")), m_id(-1) {
	m_length = -1;
	m_arcs = m_arcs;
}

patPathJ::patPathJ(list<patNode*> listOfNodes, patNetwork* theNetwork,
		patError*& err) :
		m_name(patString("No name")), m_id(-1) {
	m_length = -1;
	patNode* upNode = listOfNodes.front();
	list<patNode*>::iterator nodeIter = listOfNodes.begin();
	nodeIter++;
	for (; nodeIter != listOfNodes.end(); ++nodeIter) {
		patNode* downNode = *nodeIter;
		patArc* arc = theNetwork->getArcFromNodesUserId(upNode->userId,
				downNode->userId);
		if (arc == NULL) {
			err = new patErrNullPointer("patArc");
			WARNING(err->describe());

			return;
		}
		m_arcs.push_back(arc);

		upNode = downNode;

	}
}

ostream& operator<<(ostream& str, const patPathJ& x) {
	str << "<" << x.m_arcs.front()->getUpNode()->getUserId();
	for (list<patArc*>::const_iterator arcIter = x.m_arcs.begin();
			arcIter != x.m_arcs.end(); ++arcIter) {

		str << "-" << (*arcIter)->getDownNode()->getUserId();
	}
	str << ">";

	return str;
}

bool operator==(const patPathJ& aPath, const patPathJ& bPath) {
	//patBoolean equal =patTRUE;
	//compare path size
	if (aPath.m_arcs.size() != bPath.m_arcs.size() or aPath.m_modes.size() !=bPath.m_modes.size()) {
		return false;
	}

	//compare from arc to arc
	list<patArc*>::const_iterator aIter = aPath.m_arcs.begin();
	list<patArc*>::const_iterator bIter = bPath.m_arcs.begin();
	while (aIter != aPath.m_arcs.end() && bIter != bPath.m_arcs.end()) {
		if (*aIter != *bIter) {
			return false;
		}
		++aIter;
		++bIter;
	}

	//compare from arc to arc
	list<TransportMode>::const_iterator aIter_mode = aPath.m_modes.begin();
	list<TransportMode>::const_iterator bIter_mode = bPath.m_modes.begin();
	while (aIter_mode != aPath.m_modes.end() && bIter_mode != bPath.m_modes.end()) {
		if (*aIter_mode != *bIter_mode) {
			return false;
		}
		++aIter_mode;
		++bIter_mode;
	}

	//DEBUG_MESSAGE("path equal");
	return true;
}

bool operator!=(const patPathJ& aPath, const patPathJ& bPath) {
	return !(aPath == bPath);
}

bool operator<(const patPathJ& aPath, const patPathJ& bPath) {

	if (const_cast<patPathJ*>(&aPath)->computeLength()
			< const_cast<patPathJ*>(&bPath)->computeLength()) {
		return true;
	}
	if (const_cast<patPathJ*>(&aPath)->computeLength()
			== const_cast<patPathJ*>(&bPath)->computeLength()) {
		if (aPath.m_arcs.size() < aPath.m_arcs.size()) {
			return true;
		}

		list<patArc*>::const_iterator aIter = aPath.m_arcs.begin();
		list<patArc*>::const_iterator bIter = bPath.m_arcs.begin();
		while (aIter != aPath.m_arcs.end()
				&& bIter != bPath.m_arcs.end()) {
			if ((*aIter)->userId > (*bIter)->userId) {
				return false;
			}
			if ((*aIter)->userId < (*bIter)->userId) {
				return true;
			}
			++aIter;
			++bIter;
		}

		if (aPath.m_modes.size() < aPath.m_modes.size()) {
			return true;
		}

		list<TransportMode>::const_iterator aIter_mode = aPath.m_modes.begin();
		list<TransportMode>::const_iterator bIter_mode = bPath.m_modes.begin();
		while (aIter_mode != aPath.m_modes.end()
				&& bIter_mode != bPath.m_modes.end()) {
			if (*aIter_mode > *bIter_mode) {
				return false;
			}
			if (*aIter_mode < *bIter_mode) {
				return true;
			}
			++aIter_mode;
			++bIter_mode;
		}

	}
	//DEBUG_MESSAGE("less");
	return false;
}

void patPathJ::addArcToBack(patArc* theArc, TransportMode t_m) {

	m_arcs.insert(m_arcs.end(), theArc);
	m_modes.insert(m_modes.end(), t_m);
}

/*
 void patPathJ::addArcSequenceToBack(list<patArc*> arcSequence){
 m_arcs.insert(m_arcs.end(),arcSequence.begin(),arcSequence.end());
 }
 */
patOd patPathJ::generateOd(patNetwork* theNetwork, patError*& err) {
	patNode* originNode = theNetwork->getNodeFromUserId(
			m_arcs.front()->upNodeId);
	if (originNode == NULL) {
		stringstream str;
		str << "Origin node " << m_arcs.front()->upNodeId
				<< " does not exist";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return patOd();
	}
	patNode* destinationNode = theNetwork->getNodeFromUserId(
			m_arcs.back()->downNodeId);
	if (originNode == NULL) {
		stringstream str;
		str << "Destination node " << m_arcs.back()->downNodeId
				<< " does not exist";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return patOd();
	}
	patOd theOd(originNode, destinationNode);
	return theOd;
}

patOd patPathJ::generateOd() {
	patNode* originNode = m_arcs.front()->getUpNode();
	patNode* destinationNode = m_arcs.back()->getDownNode();
	patOd theOd(originNode, destinationNode);
	return theOd;
}

void patPathJ::assignOd(patOd* theOd) {
	m_od = theOd;
}

double patPathJ::getPathLengthG(double tmpLength) {
	double pathSizeGamma = 0.0;

	if (pathSizeGamma == 0.0) {
		return 1.0;
	} else if (pathSizeGamma == 1.0) {
		return tmpLength;
	} else {
		return patPower(tmpLength, pathSizeGamma);
	}
}

void patPathJ::assignId(const unsigned long theId) {
	m_id = theId;
}

patOd* patPathJ::getOd() {
	return m_od;
}

unsigned long patPathJ::nbrOfArcs() {
	return m_arcs.size();
}

list<pair<patArc*, TransportMode> > patPathJ::getSeg(patArc* aArc,
		patArc* bArc) {
	list < pair<patArc*, TransportMode> > rtnList;

	list<patArc*>::iterator arcIter = m_arcs.begin();
	list<TransportMode>::iterator modeIter = m_modes.begin();

	while ((*arcIter) != aArc && arcIter != m_arcs.end()) {
		++arcIter;
		++modeIter;
	}

	if (arcIter == m_arcs.end()) {
		return rtnList;
	}

	rtnList.push_back(pair<patArc*, TransportMode>(aArc, *modeIter));
	if (aArc == bArc) {
		return rtnList;
	}

	++arcIter;

	while ((*arcIter) != bArc && arcIter != m_arcs.end()) {
		rtnList.push_back(pair<patArc*, TransportMode>(*arcIter, *modeIter));
		++arcIter;
		++modeIter;
	}

	if (arcIter == m_arcs.end()) {
		return list<pair<patArc*, TransportMode> >();
	}
	rtnList.push_back(pair<patArc*, TransportMode>(*arcIter, *modeIter));
	return rtnList;
}
/*
 patBoolean patPathJ::isValidPath(vector<patGpsPoint>* gpsSequence){

 patBoolean validPath = patTRUE;
 for(patULong i = 1; i<gpsSequence->size();++i){
 map<patArc*, patReal>* currLinkDDR = gpsSequence->at(i).getLinkDDR();
 map<patArc*, patReal>* prevLinkDDR = gpsSequence->at(i-1).getLinkDDR();
 patBoolean flag1 = patFALSE;
 for(map<patArc*, patReal>::iterator currIter = currLinkDDR->begin();
 currIter != currLinkDDR->end();
 ++currIter){
 if(isLinkInPath(currIter->first) ==  patFALSE){
 continue;
 }
 patBoolean flag2 = patFALSE;
 for(map<patArc*, patReal>::iterator prevIter = prevLinkDDR->begin();
 prevIter != prevLinkDDR->end();
 ++prevIter){
 if(!(getSeg(prevIter->first, currIter->first).empty())){
 flag2 = patTRUE;
 break;
 }
 }
 if (flag2 == patTRUE){
 flag1 = patTRUE;
 break;
 }

 }

 if(flag1 == patFALSE){
 validPath = patFALSE;
 DEBUG_MESSAGE("no connection at point"<<i<<":"<<gpsSequence->at(i).getTimeStamp());
 break;
 }
 }

 return validPath;
 }
 */
void patPathJ::calTurns(patNetwork* theNetwork) {
	m_attributes.leftTurn = 0;
	m_attributes.straightTurn = 0;
	m_attributes.rightTurn = 0;
	m_attributes.uTurn = 0;
	list<patArc*>::iterator arcIter = m_arcs.begin();
	patArc* lastArc = const_cast<patArc*>(*arcIter);
	arcIter++;
	for (; arcIter != m_arcs.end(); ++arcIter) {
		patArc* currArc = *arcIter;
		double headingChange = 180.0
				- (currArc->m_attributes.heading - lastArc->m_attributes.heading);
		headingChange =
				(headingChange > 360.0) ?
						(headingChange - 360.0) : headingChange;
		headingChange =
				(headingChange < 0.0) ? (headingChange + 360.0) : headingChange;
		lastArc = currArc;
		if (headingChange == 0.0
				|| headingChange <= patNBParameters::the()->uTurnAngle
				|| headingChange > patNBParameters::the()->leftTurnAngle) {
			m_attributes.uTurn++;
		} else if (headingChange > patNBParameters::the()->uTurnAngle
				&& headingChange <= patNBParameters::the()->rightTurnAngle) {
			m_attributes.rightTurn++;
		} else if (headingChange > patNBParameters::the()->rightTurnAngle
				&& headingChange <= patNBParameters::the()->straightTurnAngle) {
			m_attributes.straightTurn++;
		} else if (headingChange > patNBParameters::the()->straightTurnAngle
				&& headingChange <= patNBParameters::the()->leftTurnAngle) {
			m_attributes.leftTurn++;
		}
	}
}

unsigned long patPathJ::getId() {
	return m_id;
}

patString patPathJ::genDescription() {
	stringstream ss;
	ss << "id: " << m_id << ", ";
	ss << "length: " << m_length << ", ";
	ss << "left turn: " << m_attributes.leftTurn << ", ";
	ss << "right turn: " << m_attributes.rightTurn << ", ";
	ss << "straight turn: " << m_attributes.straightTurn << ", ";
	return ss.str();

}

bool patPathJ::join(patPathJ bPath) {
	if (back() != bPath.front()) {
		return false;
	}

	list<patArc*>* bList = bPath.getArcList();
	if (bList == NULL || bList->empty()) {
		return true;
	}
	list<patArc*>::iterator bIter = bList->begin();
	bIter++;
	for (; bIter != bList->end(); ++bIter) {
		m_arcs.push_back(*bIter);
	}
	return true;
}

void patPathJ::computeTrafficSignals(patNetwork* theNetwork) {
	m_attributes.nbrOfTrafficSignals = 0;
	if (m_arcs.size() <= 1) {
		return;
	}

	list<patArc*>::iterator lastArc = m_arcs.end();
	lastArc--;
	list<patArc*>::iterator arcIter = m_arcs.begin();
	while (arcIter != lastArc) {

		patNode* intersection = theNetwork->getNodeFromUserId(
				(*arcIter)->downNodeId);

		if (intersection != NULL
				&& intersection->attributes.type == "traffic_signals") {
			m_attributes.nbrOfTrafficSignals++;
		}

		arcIter++;
	}
	return;
}

unsigned long patPathJ::getTrafficSignals() {
	return m_attributes.nbrOfTrafficSignals;
}

bool patPathJ::append(list<patArc*>* newSeg) {
	if (!m_arcs.empty()) {
		if (newSeg == NULL) {
			DEBUG_MESSAGE("null");
			return false;
		} else {
			if (newSeg->empty()) {
				DEBUG_MESSAGE("empty");
				return true;
			}
		}

	}
	for (list<patArc*>::iterator arcIter = newSeg->begin();
			arcIter != newSeg->end(); ++arcIter) {
		//DEBUG_MESSAGE(**arcIter);
		m_arcs.push_back(*arcIter);
	}
	computeLength();
	return true;
}

bool patPathJ::append(patPathJ* newSeg) {
	return append(newSeg->getArcList());
}

double patPathJ::computePointDDRRaw(const map<patArc*, double>* currLinkDDR) {
	double rtnValue = 0.0;
	if (currLinkDDR->empty()) {
		DEBUG_MESSAGE("empty domain");
		return rtnValue;
	}

	for (map<patArc*, double>::const_iterator arcIter = currLinkDDR->begin();
			arcIter != currLinkDDR->end(); ++arcIter) {
		if (isLinkInPath(arcIter->first)) {
			rtnValue += arcIter->second;
		}
	}

	return rtnValue;
}

unsigned long patPathJ::endNodeUserId() {
	if (m_arcs.empty()) {
		return -1;
	} else {
		return m_arcs.back()->downNodeId;
	}
}

bool patPathJ::containLoop() {
	set < unsigned long > nodeSet;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if (nodeSet.find((*arcIter)->upNodeId) == nodeSet.end()) {
			nodeSet.insert((*arcIter)->upNodeId);
		} else {
			return true;
		}
	}
	if (nodeSet.find(m_arcs.back()->downNodeId) == nodeSet.end()) {
		return false;
	} else {
		return true;
	}
}

unsigned long patPathJ::getLeftTurns() {
	return m_attributes.leftTurn;
}

unsigned long patPathJ::getRightTurns() {
	return m_attributes.rightTurn;
}

unsigned long patPathJ::getStraightTurns() {
	return m_attributes.straightTurn;
}

unsigned long patPathJ::getUTurns() {
	return m_attributes.uTurn;
}

double patPathJ::getPerPrimaryLinkRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "primary_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerTrunkLinkRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "trunk_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerMotorwayLinkRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "motorway_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerBridleRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "bridleway") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerResidentialRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "residential") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerUnclassifiedRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "unclassified") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerTertiaryRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "tertiary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerSecondaryRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "secondary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerPrimaryRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "primary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerTrunkRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "trunk") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}
double patPathJ::getPerMotorwayRd() {
	double rtn = 0.0;
	for (list<patArc*>::iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "motorway") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLenth();

}

unsigned long patPathJ::getSubPath(patPathJ* newPath, patNode* startNode,
		patNode* endNode) {
	patPathJ tmpPath;
	list<patArc*>::iterator arcIter = m_arcs.begin();
	while (arcIter != m_arcs.end()
			&& startNode->userId != (*arcIter)->upNodeId) {

		arcIter++;
	}

	if (arcIter == m_arcs.end()) {
		return 1; //can't get the start node;
	}

	while (arcIter != m_arcs.end()) {
		newPath->addArcToBack(*arcIter);
		if (endNode->userId == (*arcIter)->downNodeId) {
			break;
		}
		arcIter++;

	}
	if (arcIter == m_arcs.end()) {
		return 2; //can't get the 2end node;
	}

	return 3;
}

bool patPathJ::isUniModal() {
	return m_change_points.empty();
}

set<TransportMode> patPathJ::getUniqueModes() {
	set<TransportMode> unique_modes;
	for (list<TransportMode>::iterator mode_iter = m_modes.begin();
			mode_iter != m_modes.end(); ++mode_iter) {

		unique_modes.insert(*mode_iter);
	}
	return unique_modes;
}
short patPathJ::getNbrOfUniqueModes() {
	return getUniqueModes().size();
}

void patPathJ::detChangePoints() {
	short loc = 0;
	m_change_points.clear();
	list<TransportMode>::iterator mode_iter = m_modes.begin();
	TransportMode prev_mode = *mode_iter;
	++mode_iter;
	for (; mode_iter != m_modes.end(); ++mode_iter) {
		++loc;
		if (prev_mode != *mode_iter) {
//			DEBUG_MESSAGE(loc<<": "<<prev_mode<<"-"<<*mode_iter);
			m_change_points.push_back(loc);
		}
		prev_mode = *mode_iter;
	}
}

vector<TransportMode> patPathJ::getUnimodalModes() {
	vector<TransportMode> segment_modes;

	TransportMode current_mode;
	list<TransportMode>::iterator mode_iter = m_modes.begin();
	current_mode = *mode_iter;
	++mode_iter;
	segment_modes.push_back(current_mode);
	for (; mode_iter != m_modes.end(); ++mode_iter) {
		if (*mode_iter != current_mode) {
			segment_modes.push_back(current_mode);
		}
		current_mode = *mode_iter;
	}

	return segment_modes;
}
vector<double> patPathJ::getIntermediateUnimodalLengths() {
	vector < double > intermediate_length;
	double current_length;
	TransportMode current_mode;
	list<TransportMode>::iterator mode_iter = m_modes.begin();
	list<patArc*>::iterator arc_iter = m_arcs.begin();

	current_mode = *mode_iter;
	current_length = 0;
	++mode_iter;
	++arc_iter;
	for (; mode_iter != m_modes.end(); ++mode_iter) {
		if (*mode_iter != current_mode) {
			intermediate_length.push_back(current_length);
			current_length = (*arc_iter)->getLength();
		} else {
			current_length += (*arc_iter)->getLength();
		}
		current_mode = *mode_iter;
		++arc_iter;
	}
	intermediate_length.push_back(current_length - back()->getLength());
	return intermediate_length;
}
short patPathJ::getNbrOfChangePoints() {
	return m_change_points.size();
}

bool patPathJ::readShpFile(string file_path, patNetworkElements* network,
		patError*& err) {
	DBFHandle file_handler = DBFOpen(file_path.c_str(), "rb");
	int line_counts = DBFGetRecordCount(file_handler);
	if (line_counts == 0) {
		WARNING("empty path.");
		return false;
	}
	list<int> m_arcsOSMIds;
	list<string> listofArcsModes;

	short int edge_id_index = DBFGetFieldIndex(file_handler, "edge_id");
	short int path_id_index = DBFGetFieldIndex(file_handler, "path_id");
	short int source_id_index = DBFGetFieldIndex(file_handler, "source");
	short int target_id_index = DBFGetFieldIndex(file_handler, "target");
	short int mode_index = DBFGetFieldIndex(file_handler, "mode");
	vector<int> tmp_edges;
	vector<int> tmp_nodes;
	list < pair<int, int> > tmp_st;
	DEBUG_MESSAGE(
			"edge" << edge_id_index << "path" << path_id_index << "mode"
					<< mode_index);
	for (int i = 0; i < line_counts; ++i) {
		TransportMode t_m;
		int osm_id = DBFReadIntegerAttribute(file_handler, i, edge_id_index);
		string mode = DBFReadStringAttribute(file_handler, i, mode_index);
		DEBUG_MESSAGE(osm_id << "," << mode);
		if (mode == "BUS") {
			t_m = BUS;
		} else if (mode == "CAR") {
			t_m = CAR;
		} else if (mode == "WALK") {
			t_m = WALK;
		} else if (mode == "TRAIN") {
			t_m = TRAIN;
		} else if (mode == "BIKE") {
			t_m = BIKE;
		} else {
			WARNING("not recognized transport mode" << mode);
			return false;
		}
		patWay* a_way = network->getProcessedWay(osm_id);
		if (a_way == NULL) {
			WARNING("wrong way id" << osm_id);
			return false;
		}
		const list<patArc*>* the_arc_list = a_way->getArcListPointer();
		DEBUG_MESSAGE("way length: " << the_arc_list->size())
		for (list<patArc*>::const_iterator arc_iter = the_arc_list->begin();
				arc_iter != the_arc_list->end(); ++arc_iter) {
			m_arcs.push_back(*arc_iter);

			m_modes.push_back(t_m);
		}

	}
	return true;
}
void patPathJ::setUnimodalTransportMode(TransportMode m) {
	m_modes.clear();
	m_modes.assign(m_arcs.size(),m);
}

void patPathJ::assignModeForNoInformation() {
	m_modes.clear();
	if (m_modes.empty()) {
		setUnimodalTransportMode(TransportMode(CAR));
	}
}

bool patPathJ::exportShpFiles(string file_path, patNetworkElements* network,
		patError*& err) {
	if (m_arcs.size() != m_modes.size()) {
		stringstream str;
		str << "arcs and modes don't match";
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return false;
	}

	//build iterators
	list<patArc*>::iterator arc_iter = m_arcs.begin();
	list<TransportMode>::iterator mode_iter = m_modes.begin();
	vector<short>::iterator change_point_iter = m_change_points.begin();
	short k = 0;
	short change_point_k = 0;

	//Shape files handler and objects.

	//path shape file
	string shape_file_path = file_path + ".shp";
	SHPHandle shp_file_handler = SHPCreate(shape_file_path.c_str(), SHPT_ARC);

	//change points shape file
	string chg_file_path = file_path + "_chg.dbf";
	SHPHandle chg_file_handler = SHPCreate(chg_file_path.c_str(), SHPT_POINT);

	//path dbf file
	string path_dbf_file_path = file_path + ".dbf";
	DBFHandle path_dbf_handler = DBFCreate(path_dbf_file_path.c_str());

	int mode_dbf_id = DBFAddField(path_dbf_handler, "mode", FTInteger, 2, 0);

	int source_dbf_id = DBFAddField(path_dbf_handler, "source", FTInteger, 10,
			0);
	int target_dbf_id = DBFAddField(path_dbf_handler, "target", FTInteger, 10,
			0);

	//Loop until the end of the path and write each arc and change point.
	double up_node_x =
			(*arc_iter)->getUpNode()->getGeoCoord().longitudeInDegrees;
	double up_node_y = (*arc_iter)->getUpNode()->getGeoCoord().latitudeInDegrees;

	while (arc_iter != m_arcs.end()) {

		//Get vertices.
		double down_node_x =
				(*arc_iter)->getDownNode()->getGeoCoord().longitudeInDegrees;
		double down_node_y =
				(*arc_iter)->getDownNode()->getGeoCoord().latitudeInDegrees;

		double padfX[2] = { up_node_x, down_node_x };
		double padfY[2] = { up_node_y, down_node_y };
		up_node_x = down_node_x;
		up_node_y = down_node_y;

		SHPObject* path_shp_object = SHPCreateSimpleObject(SHPT_ARC, 2, padfX,
				padfY, NULL);
		int object_number = SHPWriteObject(shp_file_handler, -1,
				path_shp_object);
		SHPDestroyObject(path_shp_object);

		DBFWriteIntegerAttribute(path_dbf_handler, object_number, mode_dbf_id,
				*mode_iter);
		DBFWriteIntegerAttribute(path_dbf_handler, object_number, source_dbf_id,
				(*arc_iter)->getUpNode()->getUserId());
		DBFWriteIntegerAttribute(path_dbf_handler, object_number, target_dbf_id,
				(*arc_iter)->getDownNode()->getUserId());

		//Get change point.
		if (change_point_iter != m_change_points.end()
				&& *change_point_iter == k) {
			double chg_points_padfX[1] = {
					(*arc_iter)->getUpNode()->getGeoCoord().longitudeInDegrees };
			double chg_points_padfY[1] = {
					(*arc_iter)->getUpNode()->getGeoCoord().latitudeInDegrees };

			SHPObject* chg_shp_object = SHPCreateSimpleObject(SHPT_POINT, 2,
					chg_points_padfX, chg_points_padfY, NULL);
			SHPWriteObject(chg_file_handler, -1, chg_shp_object);
			SHPDestroyObject(chg_shp_object);

			//change point is the down node.
			++change_point_k;
			++change_point_iter;
		}

		//Increase the iterator.
		++k;
		++arc_iter;
		++mode_iter;
	}
	SHPClose(shp_file_handler);
	SHPClose(chg_file_handler);

	DBFClose(path_dbf_handler);
	DEBUG_MESSAGE("shape files written");
	return true;
}

