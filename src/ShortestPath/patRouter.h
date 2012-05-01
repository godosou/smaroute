/*
 * patRouter.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATROUTER_H_
#define PATROUTER_H_
#include "patNetworkEnvironment.h"
#include "patLinkCost.h"
#include "patNetworkBase.h"
#include <deque>
#include <map>
#include "patNode.h"
#include "patLinkCost.h"
#include "patShortestPathTreeGeneral.h"
#include "patMultiModalPath.h"
using namespace std;

class patRouter {
public:
	patRouter();
	virtual ~patRouter();

	patRouter(const patNetworkBase* network, const patLinkCost* link_cost);

	patShortestPathTreeGeneral  treeCost(const patNode* root,
			set<const patNode*> targets, const Direction direction) const;

	patShortestPathTreeGeneral  fwdCost(const patNode* origin,
			set<const patNode*> destinations) const;

	patShortestPathTreeGeneral  bwdCost(set<const patNode*> origins,
			const patNode* destination) const;
	patShortestPathTreeGeneral  fwdCost(const patNode* origin,
			const patNode* destination) const;
	patShortestPathTreeGeneral  fwdCost(const patNode* origin) const;
	patShortestPathTreeGeneral  bwdCost(const patNode* destination) const;
	patShortestPathTreeGeneral  bwdCost(const patNode* origin,
			const patNode* destination) const;
	map<const patNode*, patMultiModalPath>  bestRoutes(
			set<const patNode*> origins, const patNode* destination,
			const patShortestPathTreeGeneral* bwdCostTree) const;
	map<const patNode*, patMultiModalPath>  bestRoutes(
			const patNode* origin, set<const patNode*> destinations,
			const patShortestPathTreeGeneral* fwdCostTree) const;
	patMultiModalPath  bestRouteBwd(const patNode*origin,
			const patNode* destination, const patShortestPathTreeGeneral* bwdCost) const;
	patMultiModalPath  bestRouteFwd(const patNode* origin,
			const patNode* destination, const patShortestPathTreeGeneral* fwdCost) const;
	map<const patNode*, patMultiModalPath>  bestRoutes(
			const patNode* origin, set<const patNode*> destinations) const;
	map<const patNode*, patMultiModalPath> bestRoutes(set<const patNode*> origins,
			const patNode* destination) const;
	patMultiModalPath bestRoute(const patNode* origin, const patNode* destination) const;
	patShortestPathTreeGeneral costWithoutExcludedNodes(const patNode* root,
			set<const patNode*> allNodes, set<const patNode*> excludedNodes,
			const patShortestPathTreeGeneral* treeCost, Direction direction) const;
	patShortestPathTreeGeneral costWithoutExcludedNodes2(const patNode* root,
			set<const patNode*> excludedNodes,const  patShortestPathTreeGeneral* treeCost,
			Direction direction) const;
	patShortestPathTreeGeneral fwdCostWithoutExcludedNodes(const patNode* origin,
			set<const patNode*> allNodes, set<const patNode*> excludedNodes,
			const patShortestPathTreeGeneral* fwdCost)const;
	patShortestPathTreeGeneral  bwdCostWithoutExcludedNodes(const patNode* destination,
			set<const patNode*> allNodes,
			set<const patNode*> excludedNodes,
			const patShortestPathTreeGeneral* bwdCost)const;
	const patNetworkBase* getNetwork() const;
private:

protected:

	const patLinkCost* m_link_cost;
	const patNetworkBase* m_network;
};

#endif /* PATROUTER_H_ */
