/*
 * patRouter.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATROUTER_H_
#define PATROUTER_H_
#include "patNetworkEnvironment.h"
#include "patLinkAndPathCost.h"
#include "patNetworkBase.h"
#include <deque>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "patNode.h"
#include "patShortestPathTreeGeneral.h"
#include "patMultiModalPath.h"
using namespace std;
using namespace std::tr1;
class patRouter {
public:
	patRouter();
	virtual ~patRouter();

	patRouter(const patNetworkBase* network,
			const patLinkAndPathCost* link_cost);

	void treeCost(patShortestPathTreeGeneral& tree, const patNode* root,
			set<const patNode*> targets, const Direction direction) const;

	void fwdCost(patShortestPathTreeGeneral& tree, const patNode* origin,
			set<const patNode*> destinations) const;

	void bwdCost(patShortestPathTreeGeneral& tree, set<const patNode*> origins,
			const patNode* destination) const;
	void fwdCost(patShortestPathTreeGeneral& tree, const patNode* origin,
			const patNode* destination) const;
	void fwdCost(patShortestPathTreeGeneral& tree, const patNode* origin) const;
	void bwdCost(patShortestPathTreeGeneral& tree,
			const patNode* destination) const;
	void bwdCost(patShortestPathTreeGeneral& tree, const patNode* origin,
			const patNode* destination) const;
    
	void costWithoutExcludedNodes(patShortestPathTreeGeneral& tree,
			const patNode* root,
			const patNode* destination,
			const set<const patNode*>& excludedNodes,
			const patShortestPathTreeGeneral* treeCost,
			Direction direction) const;
    
	void costWithoutExcludedNodes2(patShortestPathTreeGeneral& tree,
			const patNode* root,
			const patNode* destination,
                                   const set<const patNode*>& excludedNodes,
			const patShortestPathTreeGeneral* treeCost,
			Direction direction) const;
	void fwdCostWithoutExcludedNodes(patShortestPathTreeGeneral& tree,const patNode* origin,
			const patNode* destination, set<const patNode*> excludedNodes,
			const patShortestPathTreeGeneral* fwdCost)const;
	void bwdCostWithoutExcludedNodes(patShortestPathTreeGeneral& tree,
			const patNode* destination, const patNode* origin,
			set<const patNode*> excludedNodes,
			const patShortestPathTreeGeneral* bwdCost) const;

	unordered_map<const patNode*, patMultiModalPath> bestRoutes(
			set<const patNode*> origins, const patNode* destination,
			const patShortestPathTreeGeneral* bwdCostTree) const;
	unordered_map<const patNode*, patMultiModalPath> bestRoutes(
			const patNode* origin, set<const patNode*> destinations,
			const patShortestPathTreeGeneral* fwdCostTree) const;
	patMultiModalPath bestRouteBwd(const patNode*origin,
			const patNode* destination,
			const patShortestPathTreeGeneral* bwdCost) const;
	patMultiModalPath bestRouteFwd(const patNode* origin,
			const patNode* destination,
			const patShortestPathTreeGeneral* fwdCost) const;
	unordered_map<const patNode*, patMultiModalPath> bestRoutes(
			const patNode* origin, set<const patNode*> destinations) const;
	unordered_map<const patNode*, patMultiModalPath> bestRoutes(
			set<const patNode*> origins, const patNode* destination) const;
	patMultiModalPath bestRoute(const patNode* origin,
			const patNode* destination) const;

	const patNetworkBase* getNetwork() const;
private:

protected:

	const patLinkAndPathCost* m_link_cost;
	const patNetworkBase* m_network;
};

#endif /* PATROUTER_H_ */
