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
#include "MHLinkAndPathCost.h"
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
	virtual ~MHPathGenerator();
	void setNetwork( patNetworkBase* network_base);
	void run(const patNode* origin, const patNode* destination);
	void setPathWriter(patPathWriter* path_writer);
protected:

	// -------------------- MEMBERS --------------------

	// CONFIGURATION
private:
	int DEFAULT_MSGINTERVAL;

	int DEFAULT_BURNINITERATIONS;

	int DEFAULT_SAMPLEINTERVAL;

	int m_msgInterval;

	unsigned long m_randomSeed;

	int m_totalIterations;

	int m_sampleInterval;

	double m_relativeCostScale;

	double m_cutOffProbability;

	// RUNTIME

	patRandomNumber m_rnd;

	 patNetworkBase* m_network;

	MHLinkAndPathCost m_linkAndPathCost ;
	patPathWriter* m_path_writer;

};

#endif /* MHPATHGENERATOR_H_ */
