/*
 * patPathExtension.cc
 *
 *  Created on: Nov 9, 2011
 *      Author: jchen
 */

#include "patPathExtension.h"
#include "patDisplay.h"
patPathExtension::patPathExtension(const patNetworkBase* the_network) :
		m_network(the_network) {

}
set<patMultiModalPath> patPathExtension::extendFromNode(
		const set<const patMultiModalPath*>* up_streams, const patNode* connect_node,
		const set<patMultiModalPath>* down_streams, patMeasurementDDR* ddr) {
	/*
	DEBUG_MESSAGE("start extending");
	DEBUG_MESSAGE("upstream   "<<up_streams->size());
	DEBUG_MESSAGE("downstream "<<down_streams->size());
*/
	set<patMultiModalPath> new_path_set;
	for (set<const patMultiModalPath*>::const_iterator up_path = up_streams->begin();
			up_path != up_streams->end(); ++up_path) {
		//Iterate over upstream paths.

		for (set<patMultiModalPath>::const_iterator down_path =
				down_streams->begin(); down_path != down_streams->end();
				++down_path) {
			//iterate over downstream paths.


			//Error if connection node is not consistent
			if ((*up_path)->getDownNode() != connect_node
					|| down_path->getUpNode() != connect_node) {
				WARNING("wrong connection");
				continue;
			}

			//Error invalid down stream if it contains a loop

			 //* FIXME
			if (down_path->containLoop()) {
				DEBUG_MESSAGE("contain loop");
				continue;
			}

			//Forbids unreasonable U-turn.
			if (!down_path->empty()){
				//	&& !ddr->isArcInDomain(down_path->front())) {//FIXME all u turns are forbided
				//If downstream path is not empty, and the first arc is not in the domain, and the last arc is not in domain.
				//There should not be a u turn
				if ((*up_path)->back()->getDownNode()
						== down_path->front()->getUpNode()
						&& (*up_path)->back()->getUpNode()
								== down_path->front()->getDownNode()) {
					//a u turn at the begining
					DEBUG_MESSAGE("a u turn");
					continue;
				}

			}
			patMultiModalPath new_path = *(*up_path);
			if (new_path.append(*down_path)) {
				if (new_path_set.find(new_path) == new_path_set.end()) {
					new_path.computeLength();
					new_path_set.insert(new_path);
				}
			}
		}
	}
	return new_path_set;
}

set<patMultiModalPath> patPathExtension::init(const map<const patArc*, double>* ddr_arcs) {
	set<patMultiModalPath> new_path_set;


	//DEBUG_MESSAGE("link ddr size"<<linkDDR->size());
	if (ddr_arcs->empty()) {
		WARNING("No initial path created!")
		return new_path_set;
	}
	for (map<const patArc*, double>::const_iterator arc_iter = ddr_arcs->begin();
			arc_iter != ddr_arcs->end(); ++arc_iter) {
		set<const patRoadBase*> roads = m_network->getRoadsContainArc(
				arc_iter->first);

		for (set<const patRoadBase*>::iterator road_iter = roads.begin();
				road_iter != roads.end(); ++road_iter) {
			bool success = true;
			patMultiModalPath new_path;
//			DEBUG_MESSAGE(m_network->getTransportMode());
			if (new_path.addRoadTravelToBack(*road_iter,
					m_network->getTransportMode(), 0.0)) {

				new_path_set.insert(new_path);
			}
		}
	}
}
/*
 * TODO: connect arcs in ddr
 set<patMultiModalPath> patPathExtension::init(patGpsDDR* ddr){
 set<patMultiModalPath> new_path_set;

 map<patArc*, double>* ddr_arcs = ddr->getDDRArcs();

 //DEBUG_MESSAGE("link ddr size"<<linkDDR->size());
 if (ddr_arcs->empty()) {
 WARNING("No initial path created!")
 return new_path_set;
 }


 //Create successor relationship among the arcs.
 map < patNode*, set<patNode*> > node_successors;
 for (map<patArc*, bool>::iterator iter1 = ddr_arcs->begin();
 iter1 != ddr_arcs->end();
 ++iter1) {
 if (node_successors.find(iter1->first->getUpNode()) == node_successors.end()) {
 node_successors[iter1->first->getUpNode()] = set<patNode*>();
 }
 node_successors[iter1->first->getUpNode()].insert(iter1->first->getDownNode());
 }

 //Connect nodes according to the successor.
 for (map<patNode*, set<patNode*> >::iterator iter2 = node_successors.begin();
 iter2 != node_successors.end(); ++node_successors) {

 list < patNode* > path_temp;
 connectNodes(iter2->first, &pathTemp, &node_successors, &new_path_set);
 }
 return new_path_set;
 }



 void patPathExtension::connectNodes(patNode* node, list<patNode*>* path_temp,
 map<patNode*, set<patNode*> >* node_successors,
 set<patMultiModalPath>* new_path_set) {

 path_temp->push_back(node);
 newInitPath(path_temp, new_path_set);
 map<patNode*, set<patNode*> >::iterator found = node_successors->find(node);
 if (found != node_successors->end()) {
 for (set<patNode*>::iterator iter1 = found->second.begin();
 iter1 != found->second.end();
 ++iter1) {
 bool flag = true;

 for (list<patNode*>::iterator iter2 = path_temp->begin();
 iter2 != path_temp->end(); ++iter2) {
 if (*iter2 == *iter1) {
 flag = false;
 break;
 }
 }
 if (flag) {
 connectNodes(*iter1, path_temp, node_successors, new_path_set);
 path_temp->pop_back();
 }
 }
 }
 }

 void patPathExtension::newInitPath(list<patNode*>* path_temp,
 set<patMultiModalPath>* new_path_set) {
 if (path_temp->empty() || path_temp->size() <= 1) {
 return;
 } else {

 patMultiModalPath new_path;
 list<patNode*>::iterator nIter = path_temp->begin();
 nIter++;
 patNode* prevNode = path_temp->front();
 for (; nIter != path_temp->end(); ++nIter) {
 patNode* currNode = *nIter;

 //
 patArc* currArc = m_network_environment->getArcFromNodes(prevNode,
 currNode);
 if (currArc == NULL) {
 return;
 }
 new_path.addArcToBack(currArc);

 prevNode = currNode;
 }
 new_path.setUnimodalTransportMode(m_network->getTransportMode());

 new_path_set->insert(new_path);
 }

 }
 */
patPathExtension::~patPathExtension() {
}

