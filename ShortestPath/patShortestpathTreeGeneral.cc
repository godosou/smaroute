/*
 * patShortestPathTreeGeneral.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: jchen
 */

#include "patShortestPathTreeGeneral.h"
#include "patDisplay.h"
patShortestPathTreeGeneral::patShortestPathTreeGeneral(){

}
patShortestPathTreeGeneral::~patShortestPathTreeGeneral(){

}
list<const patRoadBase*> patShortestPathTreeGeneral::getShortestPathTo(
		const patNode* dest_node) {

	list<const patRoadBase*> list_of_roads;

	const patNode* current_node = dest_node;
	map<const patNode*, const patRoadBase*>::const_iterator find_pred =
			m_predecessors.find(current_node);

	while (find_pred != m_predecessors.end()) {

		list_of_roads.push_front(find_pred->second);
		current_node = find_pred->second->getUpNode();
		if(m_root.find(current_node)!=m_root.end()){
			break;
		}
		find_pred = m_predecessors.find(current_node);
	}

	return list_of_roads;
}

/**
 * Set the label for a node;
 */

void patShortestPathTreeGeneral::setLabel(const patNode* node, double value) {
	m_labels[node] = value;
}

/**
 * insert a root node
 */
void patShortestPathTreeGeneral::insertRoot(const patNode* node) {
	m_root.insert(node);
}

/**
 * Get the label of a node, default is patMaxReal.
 */
double patShortestPathTreeGeneral::getLabel(const patNode* node) const {

	map<const patNode*, double>::const_iterator find_node_label = m_labels.find(
			node);
	if (find_node_label == m_labels.end()) {
		return patMaxReal;
	} else {
		return find_node_label->second;
	}
}

void patShortestPathTreeGeneral::setPredecessor(const patNode* node,
		const patRoadBase* road) {
	m_predecessors[node] = road;
}
void patShortestPathTreeGeneral::insertSuccessor(const patNode* node,
		const patRoadBase* road) {
	m_successors[node];
	const patNode* down_node = road->getDownNode();
	m_successors[node][down_node] = road;
}
