#include "patMultiModalPath.h"
#include "patConst.h"
#include "patDisplay.h"
#include "patPower.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patNBParameters.h"
#include <shapefil.h>
#include "patWay.h"
#include "patException.h"
#include <list>

#include <fstream>
#include <sstream>

#include "kml/dom.h"
using kmldom::IconStylePtr;
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PairPtr;
using kmldom::PlacemarkPtr;
using kmldom::StylePtr;
using kmldom::StyleMapPtr;
using kmldom::FolderPtr;
using kmldom::CoordinatesPtr;
using kmldom::PointPtr;
patMultiModalPath::patMultiModalPath(
		list<pair<const patArc*, TransportMode> > the_arc_list, bool& success) :
		m_name(patString("No name")), m_id(-1) {

	m_length = 0.0;
	for (list<pair<const patArc*, TransportMode> >::const_iterator arc_iter =
			the_arc_list.begin(); arc_iter != the_arc_list.end(); ++arc_iter) {
		if (addRoadTravelToBack(arc_iter->first, arc_iter->second, 0.0)
				== false) {
			success = false;
			break;
		}
	}

}

patMultiModalPath::patMultiModalPath(const patMultiModalPath& anotherPath) :
		patArcSequence::patArcSequence(anotherPath),
		m_roads(anotherPath.m_roads), m_unique_modes(
				anotherPath.m_unique_modes), m_od(anotherPath.m_od), m_name(
				anotherPath.m_name), m_id(anotherPath.m_id), m_attributes(
				anotherPath.m_attributes), m_distance_to_stop(
				anotherPath.m_distance_to_stop)

				{

}

patMultiModalPath::patMultiModalPath() :
		m_name(patString("No name")), m_id(-1) {
	m_length = 0.0;
}

patMultiModalPath::patMultiModalPath(list<const patArc*> arc_list) :
		m_name(patString("No name")), m_id(-1) {
	m_length = 0.0;
	for (list<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		if (addRoadTravelToBack(*arc_iter, TransportMode(NONE), 0.0)) {
			throw RuntimeException("wrong path");
			break;
		}
	}
}

patMultiModalPath::patMultiModalPath(list<const patRoadBase*> arc_list) :
		m_name(patString("No name")), m_id(-1) {
	m_length = 0.0;
	for (list<const patRoadBase*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {

		if (addRoadTravelToBack(*arc_iter, TransportMode(NONE), 0.0) == false) {
			throw RuntimeException("wrong path");
			break;
		}
	}
}
ostream& operator<<(ostream& str, const patMultiModalPath& x) {
	str << "<" << x.m_arcs.front()->getUpNode()->getUserId();
	for (vector<const patArc*>::const_iterator arcIter = x.m_arcs.begin();
			arcIter != x.m_arcs.end(); ++arcIter) {

		str << "-" << (*arcIter)->getDownNode()->getUserId();
	}
	str << ">";

	return str;
}

bool operator==(const RoadTravel& a_road, const RoadTravel& b_road) {
	if (a_road.road == b_road.road && a_road.mode == b_road.mode
			&& a_road.stop_time == b_road.stop_time) {
		return true;
	} else {
		return false;
	}
}
bool operator<(const RoadTravel& a_road, const RoadTravel& b_road) {
	if (a_road.road < b_road.road) {
		return true;
	}
	if (a_road.road > b_road.road) {
		return false;
	}
	if (a_road.mode < b_road.mode) {
		return true;
	}
	if (a_road.mode > b_road.mode) {
		return false;
	}
	if (a_road.stop_time < b_road.stop_time) {
		return true;
	}
	if (a_road.stop_time > b_road.stop_time) {
		return false;
	}
	return false;
}
bool operator!=(const RoadTravel& a_road, const RoadTravel& b_road) {
	return !(a_road == b_road);
}
bool operator==(const patMultiModalPath& aPath,
		const patMultiModalPath& bPath) {
	//patBoolean equal =patTRUE;
	//compare path size
	//FIXME differet road but the same arc
	if (aPath.m_arcs.size() != bPath.m_arcs.size()
			or aPath.m_roads.size() != bPath.m_roads.size()) {

		return false;
	}

	//compare from road to road
	list<RoadTravel>::const_iterator aIter = aPath.m_roads.begin();
	list<RoadTravel>::const_iterator bIter = bPath.m_roads.begin();
	while (aIter != aPath.m_roads.end() && bIter != bPath.m_roads.end()) {
		if (*aIter != *bIter) {
			return false;
		}
		++aIter;
		++bIter;
	}
	//DEBUG_MESSAGE("path equal");
	return true;
}

bool operator!=(const patMultiModalPath& aPath,
		const patMultiModalPath& bPath) {
	return !(aPath == bPath);
}

bool operator<(const patMultiModalPath& aPath, const patMultiModalPath& bPath) {

	const double a_length = aPath.getLength();
	const double b_length = bPath.getLength();
//	cout<<a_length<<" "<<b_length;
	if (a_length < b_length) {
		return true;
	}

	if (a_length > b_length) {
		return false;
	}

	const unsigned a_size = aPath.m_arcs.size();
	const unsigned b_size = bPath.m_arcs.size();
	if (a_size < b_size) {
		return true;
	}
	if (a_size > b_size) {
		return false;
	}

	const unsigned a_road_size = aPath.m_roads.size();
	const unsigned b_road_size = bPath.m_roads.size();
	if (a_road_size < b_road_size) {
		return true;
	}
	if (a_road_size > b_road_size) {
		return false;
	}
	list<RoadTravel>::const_iterator aIter = aPath.m_roads.begin();
	list<RoadTravel>::const_iterator bIter = bPath.m_roads.begin();
	while (aIter != aPath.m_roads.end() && bIter != bPath.m_roads.end()) {
		if ((*aIter) < (*bIter)) {
			return true;
		} else if ((*aIter) == (*bIter)) {

			++aIter;
			++bIter;
			continue;
		} else {
			return false;
		}
	}

	return false;
}
void patMultiModalPath::removeRoadTravelToBack() {
	const patRoadBase* end_road = m_roads.back().road;
	m_roads.pop_back();
	pop_back(end_road->size());

}
bool patMultiModalPath::addRoadTravelToBack(const patRoadBase* road,
		TransportMode t_m, double stop_time) {

	if (addArcsToBack(road) == false) {
		return false;
	} else {
		RoadTravel new_road;
		new_road.road = road;
		new_road.mode = t_m;
		new_road.stop_time = stop_time;
		m_roads.push_back(new_road);
		m_unique_modes.insert(t_m);
		return true;
	}
}

bool patMultiModalPath::addRoadTravelToBack(const RoadTravel road) {
	if (addArcsToBack(road.road) == false) {
		return false;
	} else {
		m_roads.push_back(road);
		m_unique_modes.insert(road.mode);
		return true;
	}
}
bool patMultiModalPath::addRoadTravelToFront(const patRoadBase* road,
		TransportMode t_m, double stop_time) {
	if (addArcsToFront(road) == false) {
		return false;
	} else {
		RoadTravel new_road;
		new_road.road = road;
		new_road.mode = t_m;
		new_road.stop_time = stop_time;
		m_roads.push_front(new_road);
		m_unique_modes.insert(t_m);
		return true;
	}
}
/*
 void patMultiModalPath::addArcSequenceToBack(list<patArc*> arcSequence){
 m_arcs.insert(m_arcs.end(),arcSequence.begin(),arcSequence.end());
 }
 */
patOd patMultiModalPath::generateOd() {
	if (m_arcs.empty()) {
		throw RuntimeException("invalid path");
	}
	const patNode* originNode = m_arcs.front()->getUpNode();
	const patNode* destinationNode = m_arcs.back()->getDownNode();
	patOd theOd(originNode, destinationNode);
	return theOd;
}

void patMultiModalPath::assignOd(patOd* theOd) {
	m_od = theOd;
}

double patMultiModalPath::getPathLengthG(double tmpLength) {
	double pathSizeGamma = 0.0;

	if (pathSizeGamma == 0.0) {
		return 1.0;
	} else if (pathSizeGamma == 1.0) {
		return tmpLength;
	} else {
		return patPower(tmpLength, pathSizeGamma);
	}
}

void patMultiModalPath::assignId(const unsigned long theId) {
	m_id = theId;
}

patOd* patMultiModalPath::getOd() const {
	return m_od;
}

int patMultiModalPath::nbrOfArcs() const {
	return m_arcs.size();
}
int patMultiModalPath::nbrOfNodes() const {
	if (m_arcs.empty()) {
		return 0;
	}
	return m_arcs.size() + 1;
}
list<TransportMode> patMultiModalPath::getModeList() const {
	list<TransportMode> mode_list;
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		for (int i = 0; i < (*road_iter).road->size(); ++i) {
			mode_list.push_back((*road_iter).mode);
		}
	}
	return mode_list;
}
list<double> patMultiModalPath::getStopTimeList() const {
	list<double> stop_time_list;
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		for (int i = 0; i < (*road_iter).road->size(); ++i) {
			stop_time_list.push_back((*road_iter).stop_time);
		}
	}
	return stop_time_list;
}
patMultiModalPath patMultiModalPath::getSeg(const patArc* aArc,
		const patArc* bArc) const {
	patMultiModalPath sub_path;
	list<TransportMode> mode_list = getModeList();
	list<double> stop_time_list = getStopTimeList();
	vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
	list<TransportMode>::const_iterator modeIter = mode_list.begin();
	list<double>::const_iterator stopIter = stop_time_list.begin();

	while ((*arcIter) != aArc && arcIter != m_arcs.end()) {
		++arcIter;
		++modeIter;
		++stopIter;
	}

	if (arcIter == m_arcs.end()) {
		//If up arc not found return empty path

		return sub_path;
	} else {
		//Other wise add the up arc
//		DEBUG_MESSAGE(*stopIter)
		sub_path.addRoadTravelToBack(*arcIter, *modeIter, *stopIter);
	}
	if (aArc == bArc) { //if the arc is the same
		return sub_path;
	}

	++arcIter;
	++modeIter;
	++stopIter;
	while ((*arcIter) != bArc && arcIter != m_arcs.end()) {
		//Search until find the down arc

		sub_path.addRoadTravelToBack(*arcIter, *modeIter, *stopIter);
		++arcIter;
		++modeIter;
		++stopIter;
	}

	if (arcIter == m_arcs.end()) {
		return patMultiModalPath();
	}
	sub_path.addRoadTravelToBack(*arcIter, *modeIter, *stopIter); // add the down arc
	return sub_path;
}
unsigned long patMultiModalPath::getId() const {
	return m_id;
}

patString patMultiModalPath::genDescription() const {
	stringstream ss;
	ss << "id: " << m_id << ", ";
	ss << "length: " << m_length << ", ";
	ss << "left turn: " << m_attributes.leftTurn << ", ";
	ss << "right turn: " << m_attributes.rightTurn << ", ";
	ss << "straight turn: " << m_attributes.straightTurn << ", ";
	return ss.str();

}

short patMultiModalPath::getTrafficSignals() const {
	return m_attributes.nbrOfTrafficSignals;
}

bool patMultiModalPath::append(const list<const patArc*>& arc_list) {
	for (list<const patArc*>::const_iterator arc_iter = arc_list.begin();
			arc_iter != arc_list.end(); ++arc_iter) {
		if (addRoadTravelToBack(*arc_iter, TransportMode(NONE), 0.0)) {
			throw RuntimeException("wrong path");
			return false;
		}
	}
}

bool patMultiModalPath::append(const patMultiModalPath& newSeg) {
	if (!newSeg.isValid()) {
		DEBUG_MESSAGE("WRONG seg appended");
		//throw RuntimeException(("WRONG seg appended");
		return false;
	} else {
		list<RoadTravel> roads = newSeg.getRoadTravelList();
//	DEBUG_MESSAGE(roads.size());

		for (list<RoadTravel>::const_iterator road_iter = roads.begin();
				road_iter != roads.end(); ++road_iter) {
			if (addRoadTravelToBack(*road_iter) == false) {
				return false;
			}
		}
	}
	//DEBUG_MESSAGE("success");
	return true;

}

list<pair<const patArc*, TransportMode> > patMultiModalPath::getArcsWithMode() const {
	list<pair<const patArc*, TransportMode> > arcs;
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		vector<const patArc*> arc_list = (*road_iter).road->getArcList();
		TransportMode mode = (*road_iter).mode;
		for (vector<const patArc*>::const_iterator arc_iter = arc_list.begin();
				arc_iter != arc_list.end(); ++arc_iter) {
			arcs.push_back(pair<const patArc*, TransportMode>(*arc_iter, mode));
		}

	}
	return arcs;
}
list<RoadTravel> patMultiModalPath::getRoadTravelList() const {
	return m_roads;
}

short patMultiModalPath::getLeftTurns() const {
	return m_attributes.leftTurn;
}

short patMultiModalPath::getRightTurns() const {
	return m_attributes.rightTurn;
}

short patMultiModalPath::getStraightTurns() const {
	return m_attributes.straightTurn;
}

short patMultiModalPath::getUTurns() const {
	return m_attributes.uTurn;
}

double patMultiModalPath::getPerPrimaryLinkRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "primary_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerTrunkLinkRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "trunk_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerMotorwayLinkRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "motorway_link") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerBridleRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "bridleway") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerResidentialRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "residential") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerUnclassifiedRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "unclassified") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerTertiaryRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "tertiary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerSecondaryRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "secondary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerPrimaryRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "primary") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerTrunkRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "trunk") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}
double patMultiModalPath::getPerMotorwayRd() const {
	double rtn = 0.0;
	for (vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
			arcIter != m_arcs.end(); ++arcIter) {
		if ((*arcIter)->m_attributes.type == "motorway") {
			rtn += (*arcIter)->getLength();
		}

	}
	return rtn / getLength();

}

patMultiModalPath patMultiModalPath::getSubPathWithNodesIndecis(int start,
		int end) {
	patMultiModalPath new_path;
	if (start > nbrOfNodes() || end > nbrOfNodes() || start > end) { //TODO start >=end?
		throw IllegalArgumentException("wrong indecis");
		new_path;
	}
	vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();

	int i = 0;
	while (start > i) {
		++arc_iter;
		++i;
	}
	while (i < end) {
		if (!new_path.addRoadTravelToBack(*arc_iter, TransportMode(NONE),
				0.0)) {
			throw RuntimeException("arcs not consistent");
		}
		++arc_iter;
		++i;
	}
//	DEBUG_MESSAGE("SUB PATH CREATED");
	return new_path;

}
unsigned long patMultiModalPath::getSubPath(patMultiModalPath* newPath,
		patNode* startNode, patNode* endNode) {
	vector<const patArc*>::const_iterator arcIter = m_arcs.begin();
	while (arcIter != m_arcs.end()
			&& startNode->getUserId() != (*arcIter)->getUpNode()->getUserId()) {

		arcIter++;
	}

	if (arcIter == m_arcs.end()) {
		return 1; //can't get the start node;
	}

	while (arcIter != m_arcs.end()) {
		newPath->addRoadTravelToBack(*arcIter, TransportMode(NONE), 0.0);
		if (endNode->userId == (*arcIter)->getDownNode()->getUserId()) {
			break;
		}
		arcIter++;

	}
	if (arcIter == m_arcs.end()) {
		return 2; //can't get the 2end node;
	}

	return 3;
}

bool patMultiModalPath::isUniModal() {
	if (m_unique_modes.size() == 1) {
		return true;
	} else {
		return false;
	}
}

short patMultiModalPath::getNbrOfUniqueModes() {
	return m_unique_modes.size();
}

vector<TransportMode> patMultiModalPath::getUnimodalModes() const {
	vector<TransportMode> segment_modes;

	TransportMode current_mode;
	list<RoadTravel>::const_iterator road_iter = m_roads.begin();
	current_mode = (*road_iter).mode;
	++road_iter;
	segment_modes.push_back(current_mode);
	for (; road_iter != m_roads.end(); ++road_iter) {
		if ((*road_iter).mode != current_mode) {
			segment_modes.push_back(current_mode);
		}
		current_mode = (*road_iter).mode;
	}
	return segment_modes;
}
vector<double> patMultiModalPath::getIntermediateUnimodalLengths() const {
	vector<double> intermediate_length;
	double current_length;
	TransportMode current_mode;
	list<RoadTravel>::const_iterator road_iter = m_roads.begin();

	current_mode = (*road_iter).mode;
	current_length = 0.0;
	++road_iter;
	for (; road_iter != m_roads.end(); ++road_iter) {
		if ((*road_iter).mode != current_mode) {
			intermediate_length.push_back(current_length);
			current_length = (*road_iter).road->getLength();
		} else {
			current_length += (*road_iter).road->getLength();
		}
		current_mode = (*road_iter).mode;
	}
	intermediate_length.push_back(current_length - back()->getLength()); //For the last segment, the last arc is not considered.

	//intermediate_length[0]-=front()->getLength();
	return intermediate_length;
}

bool patMultiModalPath::readShpFile(string file_path,
		patNetworkElements* network, patError*& err) {
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
	list<pair<int, int> > tmp_st;
	DEBUG_MESSAGE(
			"edge" << edge_id_index << "path" << path_id_index << "mode" << mode_index);
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
		vector<const patArc*> the_arc_list = a_way->getArcList();
		DEBUG_MESSAGE("way length: " << the_arc_list.size())
		for (vector<const patArc*>::const_iterator arc_iter =
				the_arc_list.begin(); arc_iter != the_arc_list.end();
				++arc_iter) {

			addRoadTravelToBack(*arc_iter, t_m, 0.0);
		}

	}
	return true;
}
void patMultiModalPath::setUnimodalTransportMode(TransportMode m) {
	m_unique_modes.clear();
	m_unique_modes.insert(m);
	for (list<RoadTravel>::iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		(*road_iter).mode = m;
	}

}

bool patMultiModalPath::exportShpFiles(string file_path,
		patNetworkElements* network, patError*& err) {

//build iterators
	list<TransportMode> mode_list = getModeList();
	vector<const patArc*>::const_iterator arc_iter = m_arcs.begin();
	list<TransportMode>::iterator mode_iter = mode_list.begin();
	/*
	 vector<short>::iterator change_point_iter = m_change_points.begin();
	 */
	short k = 0;

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
		/*
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
		 */
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

RoadTravel patMultiModalPath::back_road_travel() const {
	return m_roads.back();
}
RoadTravel patMultiModalPath::front_road_travel() const {
	return m_roads.front();
}

const patRoadBase* patMultiModalPath::backRoad() const {
	return back_road_travel().road;
}
const patRoadBase* patMultiModalPath::frontRoad() const {
	return front_road_travel().road;
}

vector<int> patMultiModalPath::getChangePoints() const {

	vector<int> m_change_points;
	short loc = 0;
	m_change_points.clear();
	list<RoadTravel>::const_iterator road_iter = m_roads.begin();
	TransportMode prev_mode = (*road_iter).mode;
	++road_iter;
	for (; road_iter != m_roads.end(); ++road_iter) {
		++loc;
		if (prev_mode != (*road_iter).mode) {
			//			DEBUG_MESSAGE(loc<<": "<<prev_mode<<"-"<<*mode_iter);
			m_change_points.push_back(loc);
		}
		prev_mode = (*road_iter).mode;
	}
	return m_change_points;
}

FolderPtr patMultiModalPath::getKML(int iteration_number) const {
	KmlFactory* factory = KmlFactory::GetFactory();
	FolderPtr path_folder = factory->CreateFolder();
	FolderPtr roads_folder = factory->CreateFolder();
	FolderPtr stops_folder = factory->CreateFolder();

	stringstream path_number;
	path_number << iteration_number;
	path_folder->set_name(path_number.str());
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		FolderPtr road_f = (*road_iter).road->getKML(
				getTransportMode((*road_iter).mode));

//		road_f->set_styleurl(style_u.str());
		road_f->set_name(getTransportMode((*road_iter).mode));
		roads_folder->add_feature(road_f);

		if ((*road_iter).stop_time > 0.0) {
			CoordinatesPtr coordinates = factory->CreateCoordinates();
			coordinates->add_latlng(
					(*road_iter).road->getDownNode()->getLatitude(),
					(*road_iter).road->getDownNode()->getLongitude());

			PointPtr point = factory->CreatePoint();
			point->set_coordinates(coordinates); // point takes ownership

			PlacemarkPtr stop_placemark = factory->CreatePlacemark();
			stringstream ss;
			ss << (*road_iter).stop_time;
			stop_placemark->set_name(ss.str());
			stop_placemark->set_geometry(point); // placemark takes ownership
			stops_folder->add_feature(stop_placemark);
			stop_placemark->set_styleurl("#stop");
		}
	}
	path_folder->add_feature(roads_folder);
	path_folder->add_feature(stops_folder);
	return path_folder;
}
double patMultiModalPath::getMinimumTravelTime(
		const patNetworkEnvironment* network_environment,
		set<const patArc*>& begin_arcs, set<const patArc*>& end_arcs) const {
	double minimum_time = 0.0;
	if (m_roads.size() <= 2) {
		return 0.0;
	}
	list<RoadTravel>::const_iterator road_iter = m_roads.begin();
	++road_iter;

	list<RoadTravel>::const_iterator last_road = m_roads.end();
	--last_road;
	for (; road_iter != last_road; ++road_iter) {
		minimum_time += network_environment->getMinimumTravelTime(
				(*road_iter).road, (*road_iter).mode);
	}
	return minimum_time;
}

void patMultiModalPath::proposeStop(
		const patNetworkEnvironment* network_environment, double speed,
		double stop_time) {
	//DEBUG_MESSAGE("TOTAL STOP TIME" << getTotalStopTime());
	double min_speed =
			network_environment->getNetwork(back_road_travel().mode)->getMinSpeed();
	if (speed < min_speed) {
		m_roads.back().stop_time += stop_time;
//		DEBUG_MESSAGE("TOTAL STOP TIME" << getTotalStopTime());
	}
}

double patMultiModalPath::getTotalStopTime() const {
	double stop_time = 0.0;
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		if ((*road_iter).stop_time > 0.0) {
//			DEBUG_MESSAGE((*road_iter).stop_time);
		}
		stop_time = (*road_iter).stop_time;

	}
	return stop_time;
}

bool patMultiModalPath::containsPT() const {
	for (list<RoadTravel>::const_iterator road_iter = m_roads.begin();
			road_iter != m_roads.end(); ++road_iter) {
		if ((*road_iter).mode == TransportMode(BUS)
				|| (*road_iter).mode == TransportMode(METRO)
				|| (*road_iter).mode == TransportMode(TRAIN)) {
			return true;
		}
	}
	return false;
}
bool patMultiModalPath::isReasonableModeChange() const {

	//not necessary to change back to the same mode
	//but restrition only put on non-walk
	if (m_roads.size() <= 2) {
		return true;
	}
	double current_length = 0.0;
	list<RoadTravel>::const_iterator road_iter = m_roads.end();
	--road_iter;
	TransportMode current_mode = (*road_iter).mode;
	int mode_seg = 0;
	const patNode* access_node = (*road_iter).road->getDownNode();
	while (road_iter != m_roads.begin()) {
		--road_iter;
		if ((*road_iter).mode != current_mode) {
			if (mode_seg > 0) {
				if (isPublicTransport(current_mode)) {

					if (!access_node->getName().empty()
							&& access_node->getName()
									== (*road_iter).road->getDownNode()->getName()) {
						DEBUG_MESSAGE(
								"unreasonable change with the same stop" << access_node->getName());
						return false;
					}
				}

				if (current_mode != TransportMode(WALK)) {
					if (current_length
							< patNBParameters::the()->minChangeLengthBackToTheSame) {
						DEBUG_MESSAGE(
								"unreasonable change " << getTransportMode(current_mode) << current_length);
						return false;
					}
				}
			}
			mode_seg++;
			current_length = (*road_iter).road->getLength();
			access_node = (*road_iter).road->getDownNode();
		} else {
			current_length += (*road_iter).road->getLength();
		}
		current_mode = (*road_iter).mode;
	}
	return true;
}

TransportMode patMultiModalPath::frontMode() const {
	return m_roads.front().mode;
}
TransportMode patMultiModalPath::backMode() const {
	return m_roads.back().mode;
}

bool patMultiModalPath::containsRoad(const patArc* arc,
		TransportMode mode) const {

	vector<const patArc*> arc_list = getArcList();
	list<TransportMode> mode_list = getModeList();
	vector<const patArc*>::const_iterator arc_iter = arc_list.end();
	list<TransportMode>::const_iterator mode_iter = mode_list.end();

	while (arc_iter != arc_list.begin()) {
		--arc_iter;
		--mode_iter;
		if (arc == *arc_iter and mode == *mode_iter) {
			return true;

		}
	}
	return false;
}

void patMultiModalPath::detDistanceToStop(
		patNetworkEnvironment* network_environment) {
	list<pair<const patArc*, TransportMode> > arcs = getArcsWithMode();

	list<pair<const patArc*, TransportMode> >::const_iterator arc_iter_back =
			arcs.end();
	--arc_iter_back;
	m_distance_to_stop.push_front(0.0);
	TransportMode down_mode = arc_iter_back->second;
	double distance = arc_iter_back->first->getLength();
	while (arc_iter_back != arcs.begin()) {
		--arc_iter_back;
		double arc_length = arc_iter_back->first->getLength();
		if (down_mode != arc_iter_back->second
				|| arc_iter_back->first->getDownNode()->hasTrafficSignal()) {
			m_distance_to_stop.push_front(0.0);
			distance = arc_length;
		} else {
			m_distance_to_stop.push_front(distance);
			distance += arc_length;
		}
	}

}

void patMultiModalPath::setDistanceToStop(double d) {
	for (short i = 0; i < m_arcs.size() - 1; ++i) {

		m_distance_to_stop.push_back(0.0);
	}
	m_distance_to_stop.push_back(d);
}
const list<double>* patMultiModalPath::getDistanceToStop() const {
	return &m_distance_to_stop;
}

/*
 bool patMultiModalPath::equalsSubPath(patMultiModalPath& b_path, int start, int end){
 list<pair<const patArc*, TransportMode> > b_arcs =b_path.getArcsWithMode();
 list<pair<const patArc*, TransportMode> > a_arcs =getArcsWithMode();

 int b_size =b_arcs.size();
 int a_size = a_arcs.size() - end + start;
 if (b_size!=a_size){
 DEBUG_MESSAGE("size different"<<a_size<<","<<b_size);
 return false;
 }
 list<pair<const patArc*, TransportMode> >::const_iterator b_arc_iter = b_arcs.begin();
 list<pair<const patArc*, TransportMode> >::const_iterator a_arc_iter = a_arcs.begin();
 for(int i=0;i<start;++i){
 ++a_arc_iter;
 }
 for(int i=0;i<end-start-1;++i){
 if(*a_arc_iter!=*b_arc_iter){
 return false;
 }
 ++a_arc_iter;
 ++b_arc_iter;
 }
 return true;
 }
 */
void patMultiModalPath::clear() {
	m_arcs.clear();
	m_length = 0.0;
	m_roads.clear();
	m_unique_modes.clear();
	m_od = NULL;
	m_distance_to_stop.clear();
}
