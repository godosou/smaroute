/*
 * patEnumeratePaths.cc
 *
 *  Created on: May 25, 2012
 *      Author: jchen
 */

#include "patEnumeratePaths.h"
#include <tr1/unordered_map>
#include <set>
#include "patRoadBase.h"
#include "patMultiModalPath.h"
#include "patException.h"
using namespace std;
using namespace std::tr1;
patEnumeratePaths::patEnumeratePaths() {
	// TODO Auto-generated constructor stub

}
void patEnumeratePaths::setNetwork(const patNetworkBase* network_environment) {
	m_network = network_environment;
}

void patEnumeratePaths::setPathWriter(patPathWriter* path_writer) {
	m_path_writer = path_writer;
}
void patEnumeratePaths::depth_first_enumerate(
		const unordered_map<const patNode*, set<const patRoadBase*> >* outgoing_incidents
		,const patNode* up_node, const patNode* end_node
		, patMultiModalPath& tmp_path,
		set<patMultiModalPath>& found_path) const{
	const unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator find_outgoing =
			outgoing_incidents->find(up_node);
	if (find_outgoing == outgoing_incidents->end()) {
		DEBUG_MESSAGE(up_node->getUserId());
		return;
	} else {
		for (set<const patRoadBase*>::const_iterator road_iter =
				find_outgoing->second.begin();
				road_iter != find_outgoing->second.end(); ++road_iter) {
			if (tmp_path.addRoadTravelToBack(*road_iter)) {
				if (tmp_path.containLoop() != NULL) {
					// contain loop, do nothing
					//tmp_path.removeRoadTravelToBack();
				} else if ((*road_iter)->getDownNode() == end_node) {
					found_path.insert(tmp_path);
				} else {
					depth_first_enumerate(outgoing_incidents,
							(*road_iter)->getDownNode(), end_node, tmp_path,
							found_path);
				}
				tmp_path.removeRoadTravelToBack();
			} else {
				throw RuntimeException("road append error");
			}
		}
	}

}
void patEnumeratePaths::run(const patNode* origin, const patNode* destination) {
	const unordered_map<const patNode*, set<const patRoadBase*> >* outgoing_incidents =
			m_network->getOutgoingIncidents();
//	DEBUG_MESSAGE("ougoing nodes"<<outgoing_incidents->size());
//	for(unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator oi_iter = outgoing_incidents->begin();
//			oi_iter!=outgoing_incidents->end();++oi_iter){
//		DEBUG_MESSAGE(oi_iter->first->getUserId()<<":"<<oi_iter->second.size());
//	}
	set<patMultiModalPath> found_path;
	patMultiModalPath tmp_path;
	depth_first_enumerate(outgoing_incidents,origin,destination,tmp_path,found_path);
	DEBUG_MESSAGE("Enumerates "<<found_path.size()<<" paths");
	map<string, string> attrs;
	attrs["count"]="1";
	attrs["logweight"]="0.0";
	for(set<patMultiModalPath>::const_iterator path_iter = found_path.begin();
			path_iter!=found_path.end();
			++path_iter){
		m_path_writer->writePath(*path_iter,attrs);
	}
	//m_path_writer->close();
}
patEnumeratePaths::~patEnumeratePaths() {
	// TODO Auto-generated destructor stub
}

