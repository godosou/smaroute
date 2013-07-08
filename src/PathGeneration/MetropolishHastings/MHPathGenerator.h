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
#include "MHNodeInsertionProbability.h"
class MHPathGenerator: public patPathGenerator {
public:
	// DEFAULT PARAMETER VALUES

	// CONFIG ELEMENTS

	MHPathGenerator(const patRandomNumber& rnd);
	MHPathGenerator(MHPathGenerator const& other);

	MHPathGenerator* clone() const {
		return new MHPathGenerator(*this);
	}

	void getNodeProbability(patRouter& router,
			unordered_map<const patNode*, double>& proposal_probabilities,
			const patNode* origin, const patNode* destination);
	virtual ~MHPathGenerator();
	void run(const patNode* origin, const patNode* destination);
	void setPathWriter(patPathWriter* path_writer);
	void setNIPCalculator(MHNodeInsertionProbability* nip_calculator);
	void setWritterWrapper(MHStateProcessor<MHPath>* writter_wrapper);
	void setRouterLinkCost(const patLinkAndPathCost* linkCost);
	void setUtilityFunction(const patLinkAndPathCost* utility_function);
	void setMHWeight(const MHWeightFunction* MHWeight);
	const MHWeightFunction* getMHWeight() const {
		return m_MHWeight;
	}

	void calibrate(const patNode* origin, const patNode* destination);
	void setSampleCount(const unsigned long& sample_count);
	double calculatePathLogWeight(const patMultiModalPath& path) const;
protected:

	int m_msgInterval;

	unsigned long m_total_samples;
	unsigned long m_randomSeed;

	int m_totalIterations;

	int m_sampleInterval;

	double m_relativeCostScale;

	double m_cutOffProbability;

	const patRandomNumber& m_rnd;
	MHNodeInsertionProbability* m_nip_calculator;
	patLinkAndPathCost* m_router_cost;
	const patLinkAndPathCost* m_utility_function;
	MHWeightFunction* m_MHWeight;
	patPathWriter* m_path_writer;
	MHStateProcessor<MHPath>* m_writter_wrapper;
};

#endif /* MHPATHGENERATOR_H_ */
