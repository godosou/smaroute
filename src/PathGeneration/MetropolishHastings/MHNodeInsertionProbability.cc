/*
 * MHNodeInsertionProbability.cc
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#include "MHNodeInsertionProbability.h"
#include "patMultiModalPath.h"
#include "patRouter.h"
#include "patNetworkBase.h"
#include "MHWeightFunction.h"
MHNodeInsertionProbability::~MHNodeInsertionProbability() {
	// TODO Auto-generated destructor stub
}
MHNodeInsertionProbability::MHNodeInsertionProbability() :
		m_path(NULL), m_router(NULL), m_network(NULL), m_MHWeight(NULL) {

}
MHNodeInsertionProbability::MHNodeInsertionProbability(const patRouter* router,
		const patNetworkBase* network, const MHWeightFunction* MHWeight) :
		m_router(router), m_network(network), m_MHWeight(MHWeight), m_path(NULL) {

}
void MHNodeInsertionProbability::setPath(const patMultiModalPath* path) {
	m_path=path;
}
void MHNodeInsertionProbability::setParams(const patRouter* router,
		const patNetworkBase* network, const MHWeightFunction* MHWeight) {

	m_router=router;
	m_network=network;
	m_MHWeight=MHWeight;
}
