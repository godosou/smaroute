/*
 * MHLinkAndPathCost.cc
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#include "MHLinkAndPathCost.h"
#include "patNBParameters.h"

map<string, double> MHLinkAndPathCost::m_link_coefficients;
map<string, double> MHLinkAndPathCost::m_path_coefficients;

double MHLinkAndPathCost::m_linkCostScale;

double MHLinkAndPathCost::m_nodeLoopScale;

MHLinkAndPathCost::MHLinkAndPathCost() {

}

MHLinkAndPathCost::~MHLinkAndPathCost() {
	// TODO Auto-generated destructor stub
}

void MHLinkAndPathCost::configure() {

	string file_name = patNBParameters::the()->paramFolder
			+ "rcs/link_cost.txt";
	ifstream file_stream_handler;
	file_stream_handler.open(file_name.c_str(), ios::in);
	if (!file_stream_handler) {
		WARNING("Error while parsing " << file_name);
		throw IllegalArgumentException("wrong link_cost file");
		return;
	}DEBUG_MESSAGE("read file" << file_name);
	string line;
	getline(file_stream_handler, line);
	istringstream linestream_1(line);
	string item_1;

	getline(linestream_1, item_1, ',');
	string k(item_1.c_str());

	getline(linestream_1, item_1, '\n');
	double v = atof(item_1.c_str());

	DEBUG_MESSAGE(k << "," << v);
	if (k != "scale") {
		WARNING("wrong scale parameter " << k);
		throw IllegalArgumentException("wrong scale parameter ");
	} else {
		MHLinkAndPathCost::m_linkCostScale = v;
	}
	while (getline(file_stream_handler, line)) {
		istringstream linestream(line);
		string item;

		getline(linestream, item, ',');
		k = item.c_str();

		getline(linestream, item, '\n');
		v = atof(item.c_str());

		DEBUG_MESSAGE(k << "," << v)
		MHLinkAndPathCost::m_link_coefficients[k] = v;
	}
	file_stream_handler.close();

	string file_name_p = patNBParameters::the()->paramFolder
			+ "rcs/path_cost.txt";
	ifstream file_stream_handler_p;
	file_stream_handler_p.open(file_name.c_str(), ios::in);
	if (!file_stream_handler_p) {
		WARNING("Error while parsing " << file_name_p);
		throw IllegalArgumentException("wrong path_cost file");
		return;
	}

	while (getline(file_stream_handler_p, line)) {
		istringstream linestream(line);
		string item;

		getline(linestream, item, ',');
		k = item.c_str();

		getline(linestream, item, '\n');
		v = atof(item.c_str());

		DEBUG_MESSAGE(k << "," << v)
		MHLinkAndPathCost::m_path_coefficients[k] = v;
	}
	file_stream_handler.close();

}
void MHLinkAndPathCost::setLinkCostScale(const double linkCostScale) {
	MHLinkAndPathCost::m_linkCostScale = linkCostScale;
}

double MHLinkAndPathCost::getLinkCostScale() const {
	return MHLinkAndPathCost::m_linkCostScale;
}

void MHLinkAndPathCost::setNodeLoopScale(const double nodeLoopScale) {
	MHLinkAndPathCost::m_nodeLoopScale = nodeLoopScale;
}

double MHLinkAndPathCost::getNodeLoopScale() const {
	return MHLinkAndPathCost::m_nodeLoopScale;
}
double MHLinkAndPathCost::getCost(const patArc* arc) const {
	double cost = 0.0;
	for (map<string, double>::const_iterator a_iter =
			MHLinkAndPathCost::m_link_coefficients.begin();
			a_iter != MHLinkAndPathCost::m_link_coefficients.end(); ++a_iter) {
		//DEBUG_MESSAGE(a_iter->second<<"*"<<arc->getAttribute(a_iter->first));
		cost += a_iter->second * (float) (arc->getAttribute(a_iter->first));
	}
	return cost;
}

double MHLinkAndPathCost::logWeightWithoutCorrection(const MHPath& path) const {

	double pathCost = 0.0;
	vector<const patArc*> arcs = path.getArcList();
	for (vector<const patArc*>::const_iterator arc_iter = arcs.begin();
			arc_iter != arcs.end(); ++arc_iter) {
		pathCost += getCost(*arc_iter);
	}
//TODO
//	final double nodeLoopCnt;
//	if (this.nodeLoopScale != 0.0) {
//		// we know that we are dealing with an inverted network here
//		final Set<String> originalNodeIds = new HashSet<String>();
//		originalNodeIds.add(path.getNodes().get(0).getAttr(
//						NODE_ORIGINALFROMNODE_ID));
//		for (Node invertedNode : path.getNodes()) {
//			originalNodeIds.add(invertedNode
//					.getAttr(NODE_ORIGINALTONODE_ID));
//		}
//		nodeLoopCnt = (path.getLinks().size() + 2) - originalNodeIds.size();
//	} else {
//		nodeLoopCnt = 0.0;
//	}

	return -MHLinkAndPathCost::m_linkCostScale * pathCost; // - this.nodeLoopScale * nodeLoopCnt);
}

double MHLinkAndPathCost::logWeight(const MHPath& path) const {
	return (logWeightWithoutCorrection(path) - log(path.pointCombinationSize()));
}

// -------------------- INITIALIZATION --------------------
//
//	void configure(final Config config) {
//
//	}

// -------------------- SETTERS AND GETTERS --------------------

// -------------------- IMPLEMENTATION OF MHWeight --------------------

