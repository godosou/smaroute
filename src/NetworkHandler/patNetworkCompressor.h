/*
 * paNetworkCompressor.h
 *
 *  Created on: Jul 20, 2012
 *      Author: jchen
 */

#ifndef PATNETWORKCOMPRESSOR_H_
#define PATNETWORKCOMPRESSOR_H_

#include "patNetworkBase.h"
#include "patArcSequence.h"

#include "patCompressedArc.h"
#include "patRoadBase.h"
#include "patNode.h"
#include <tr1/unordered_set>
#include <vector>
class patNetworkCompressor {
public:
	patNetworkCompressor( patNetworkBase* original_network, std::tr1::unordered_set<const patNode*>& not_compresed);
	void compress(const map<ARC_ATTRIBUTES_TYPES, double>& link_coef);
	void walk(const patNode* origin,const patNode* up_node, vector<const patRoadBase*>& temp_arcs);
	virtual ~patNetworkCompressor();
protected:
	map<int, patCompressedArc> m_compressed_streets;
	 patNetworkBase* m_original_network;
	std::tr1::unordered_set<const patNode*>& m_not_compresed;

	unordered_map<const patNode*, set<const patRoadBase*> > m_new_oi;
	std::tr1::unordered_set<const patNode*> m_normal_nodes;
	std::tr1::unordered_set<const patNode*> m_deadend_nodes;
	std::tr1::unordered_set<const patNode*> m_intermediate_nodes;

};

#endif /* PANETWORKCOMPRESSOR_H_ */
