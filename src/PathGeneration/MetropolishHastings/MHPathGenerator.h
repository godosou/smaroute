/*
 * MHPathGenerator.h
 *
 *  Created on: Apr 26, 2012
 *      Author: jchen
 */

#ifndef MHPATHGENERATOR_H_
#define MHPATHGENERATOR_H_
#include "patPathGenerator.h"
#include "MHPath.h"
#include "MHPathProposal.h"
#include "MHWeightFunction.h"
#include "patRandomNumber.h"
#include "patPathWriter.h"
#include "patNode.h"
#include "patNetworkEnvironment.h"
#include <boost/random.hpp>
class MHPathGenerator: public patPathGenerator {
public:
	// DEFAULT PARAMETER VALUES

	// CONFIG ELEMENTS

	MHPathGenerator(unsigned long rng);
	MHPathGenerator(MHPathGenerator const& other);

	MHPathGenerator* clone() const{
		return new MHPathGenerator(*this);
	}

	const patNetworkBase* getNetwork(){
		return m_network;
	}
	virtual ~MHPathGenerator();
	void setNetwork(const patNetworkBase* network_base);
	void run(const patNode* origin, const patNode* destination);
	void setPathWriter( patPathWriter* path_writer);
	void setWritterWrapper(MHStateProcessor<MHPath>* writter_wrapper);
	void setRouterLinkCost(const patLinkAndPathCost* linkCost) ;
	void setMHWeight(const MHWeightFunction* MHWeight);
	const MHWeightFunction* getMHWeight() const{
		return m_MHWeight;
	}
	double calculatePathLogWeight(const patMultiModalPath& path) const;
protected:


	int m_msgInterval;

	unsigned long m_total_samples;
	unsigned long m_randomSeed;

	int m_totalIterations;

	int m_sampleInterval;

	double m_relativeCostScale;

	double m_cutOffProbability;

	// RUNTIME

	patRandomNumber m_rnd;

	 patNetworkBase* m_network;

	const patLinkAndPathCost* m_linkAndPathCost;
	const MHWeightFunction* m_MHWeight;
	patPathWriter* m_path_writer;
	MHStateProcessor<MHPath>* m_writter_wrapper;
};

#endif /* MHPATHGENERATOR_H_ */
