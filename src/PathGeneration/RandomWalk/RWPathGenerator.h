/*
 * RWPathGenerator.h
 *
 *  Created on: May 31, 2012
 *      Author: jchen
 */

#ifndef RWPATHGENERATOR_H_
#define RWPATHGENERATOR_H_
#include "patPathGenerator.h"
#include "patLinkAndPathCost.h"
#include "patRandomNumber.h"
#include "patPathWriter.h"
#include "patNode.h"
#include "patMultiModalPath.h"
#include "patChoiceSetWriter.h"
class patRoadBase;
class patNetworkBase;
class patPathWriter;
class patLinkAndPathCost;
class patShortestPathTreeGeneral;
class RWPathGenerator: public patPathGenerator {
public:
	RWPathGenerator(unsigned long rng, double kumaA, double kumaB,
			const patLinkAndPathCost* link_cost);
	void setNetwork(const patNetworkBase* network);
	void setPathWriter(patPathWriter* path_writer);
	void setLinkCost(const patLinkAndPathCost* link_cost);
	RWPathGenerator* clone() const {
		return new RWPathGenerator(*this);
	}
	map<const patRoadBase*,  double> getOutgoingArcProbas(
			const patNode* from_node, const patNode* destination,
			const patShortestPathTreeGeneral* sp_tree) const;
	pair<const patRoadBase*, const double> getNextArcInRandomWalk(
			const patNode* from_node, const patNode* destination,
			const patShortestPathTreeGeneral* sp_tree) const;
	void sampleOnePath(const patNode* origin, const patNode* destination,
			const patShortestPathTreeGeneral* sp_tree,patChoiceSetWriter& csw) ;
	double calculatePathLogWeight(const patMultiModalPath& path) const;
	void run(const patNode* origin, const patNode* destination) ;

	const patNetworkBase* getNetwork() const{
		return m_network;
	}
	virtual ~RWPathGenerator();
protected:

	const patNode* m_origin;
	const patNode* m_destination;
	int m_msgInterval;

	unsigned long m_total_samples;

	patRandomNumber m_rnd;

	const patNetworkBase* m_network;

	const patLinkAndPathCost* m_link_cost;
	patPathWriter* m_path_writer;

//	map<patMultiModalPath, pair<int, double> > m_sampled_set;

	double m_kumaA;
	double m_kumaB;
};

#endif /* RWPATHGENERATOR_H_ */
