/*
 * paNetworkCompresser.cc
 *
 *  Created on: Jul 20, 2012
 *      Author: jchen
 */

#include "patNetworkCompressor.h"
#include <tr1/unordered_map>
using namespace std;
using namespace std::tr1;
patNetworkCompressor::~patNetworkCompressor() {
	// TODO Auto-generated destructor stub
}

patNetworkCompressor::patNetworkCompressor(patNetworkBase* original_network,
		unordered_set<const patNode*>& not_compresed) :
		m_original_network(original_network), m_not_compresed(not_compresed) {

}

void patNetworkCompressor::walk(const patNode* origin, const patNode* up_node,
		vector<const patRoadBase*>& temp_arcs) {
	const unordered_map<const patNode*, set<const patRoadBase*> >* outgoing_incidents =
			m_original_network->getOutgoingIncidents();

	unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator find_outgoing =
			outgoing_incidents->find(up_node);
	if (find_outgoing != outgoing_incidents->end()) {

		for (set<const patRoadBase*>::const_iterator arc_iter =
				find_outgoing->second.begin();
				arc_iter != find_outgoing->second.end(); ++arc_iter) {
			const patNode* down_node = (*arc_iter)->getDownNode();
			const patNode* previous_node = NULL;
			if (!temp_arcs.empty()) {
				previous_node = temp_arcs.back()->getUpNode();
			}
			if (previous_node == down_node) {
				continue;
			}
			temp_arcs.push_back(*arc_iter);
			if (m_normal_nodes.find(down_node) != m_normal_nodes.end()) {
				unsigned nbr_arcs = temp_arcs.size();
				if (nbr_arcs == 1) {

					pair<map<int, patCompressedArc>::iterator, bool> insert_result =
							m_compressed_streets.insert(
									pair<int, patCompressedArc>(
											m_compressed_streets.size(),
											patCompressedArc(temp_arcs)));
					if (insert_result.second == false) {
						throw RuntimeException("wrong insert street");
					}

					m_new_oi[origin];
					m_new_oi[origin].insert(&(insert_result.first->second));
				} else if (nbr_arcs > 1) {
					//break the arc only reserve one intermediate arc
					const patRoadBase* back_arc = temp_arcs.back();
					vector<const patRoadBase*> seg_2;
					seg_2.push_back(back_arc);
					temp_arcs.pop_back();


					pair<map<int, patCompressedArc>::iterator, bool> insert_result =
							m_compressed_streets.insert(
									pair<int, patCompressedArc>(
											m_compressed_streets.size(),
											patCompressedArc(temp_arcs)));
					if (insert_result.second == false) {
						throw RuntimeException("wrong insert street");
					}

					m_new_oi[origin];
					m_new_oi[origin].insert(&(insert_result.first->second));



					pair<map<int, patCompressedArc>::iterator, bool> insert_result_seg_2 =
							m_compressed_streets.insert(
									pair<int, patCompressedArc>(
											m_compressed_streets.size(),
											patCompressedArc(seg_2)));
					if (insert_result_seg_2.second == false) {
						throw RuntimeException("wrong insert street");
					}

					m_new_oi[back_arc->getUpNode()];
					m_new_oi[back_arc->getUpNode()].insert(&(insert_result_seg_2.first->second));




					temp_arcs.push_back(back_arc);
				}

			} else if (m_deadend_nodes.find(down_node)
					!= m_deadend_nodes.end()) {

			}

			else if (m_intermediate_nodes.find(down_node)
					!= m_intermediate_nodes.end()) {
				walk(origin, down_node, temp_arcs);

			}
			temp_arcs.pop_back();
		}
	}

}
void patNetworkCompressor::compress(
		const map<ARC_ATTRIBUTES_TYPES, double>& link_coef) {
	const set<const patNode*> nodes = m_original_network->getNodes();
	cout << "original network nodes: " << nodes.size();
	for (set<const patNode*>::const_iterator node_iter = nodes.begin();
			node_iter != nodes.end(); ++node_iter) {
		const patNode* new_node = *node_iter;
		if (m_not_compresed.find(new_node) != m_not_compresed.end()) {
			m_normal_nodes.insert(new_node);

		} else {
			NODE_STATUS node_status = m_original_network->checkNodeStatus(
					new_node);
			switch (node_status) {
			case NORMAL:
				m_normal_nodes.insert(new_node);
				break;
			case INTERMEDIATE:
				m_intermediate_nodes.insert(new_node);
				break;
			case DEADEND:
				m_deadend_nodes.insert(new_node);
				break;
			default:
				throw RuntimeException("Wrong node status");
				break;
			}
		}
	}
	cout << "Nodes: normal " << m_normal_nodes.size() << ", intermediate: "
			<< m_intermediate_nodes.size() << ", dead end "
			<< m_deadend_nodes.size() << endl;
	for (unordered_set<const patNode*>::const_iterator node_iter =
			m_normal_nodes.begin(); node_iter != m_normal_nodes.end();
			++node_iter) {
		vector<const patRoadBase*> temp_arcs;
		walk(*node_iter, *node_iter, temp_arcs);
	}
	for (map<int, patCompressedArc>::iterator street_iter =
			m_compressed_streets.begin();
			street_iter != m_compressed_streets.end(); ++street_iter) {
		street_iter->second.computeGeneralizedCost();
	}
//	for (unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator n_iter =
//			m_new_oi.begin(); n_iter != m_new_oi.end(); ++n_iter) {
//		for (set<const patRoadBase*>::const_iterator road_iter =
//				n_iter->second.begin(); road_iter != n_iter->second.end();
//				++road_iter) {
//			DEBUG_MESSAGE(
//					(*road_iter)->getUpNode()->getUserId()<<"-"<<(*road_iter)->getDownNode()->getUserId());
//		}
//	}
	cout << "New outgoing incidents with nodes: " << m_new_oi.size() << endl;
	m_original_network->setOutgoingIncidents(m_new_oi);
	m_original_network->finalizeNetwork();
}
