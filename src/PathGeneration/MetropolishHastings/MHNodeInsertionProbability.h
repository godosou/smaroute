/*
 * MHNodeInsertionProbability.h
 *
 *  Created on: Jul 30, 2012
 *      Author: jchen
 */

#ifndef MHNODEINSERTIONPROBABILITY_H_
#define MHNODEINSERTIONPROBABILITY_H_

#include <tr1/unordered_map>
class patMultiModalPath;
class patRouter;
class patNetworkBase;
class MHWeightFunction;
class patNode;
class MHNodeInsertionProbability {
public:
	MHNodeInsertionProbability();
	MHNodeInsertionProbability(const patRouter* router,
			const patNetworkBase* network, const MHWeightFunction* MHWeight);
	void setParams(const patRouter* router, const patNetworkBase* network,
			const MHWeightFunction* MHWeight);
	void setPath(const patMultiModalPath* path);
	virtual std::tr1::unordered_map<const patNode*, double> calculate(
			const patNode* origin, const patNode* destination) const=0;
	virtual ~MHNodeInsertionProbability();
protected:
	const patMultiModalPath* m_path;
	const patRouter* m_router;
	const patNetworkBase* m_network;
	const MHWeightFunction* m_MHWeight;
};

#endif /* MHNODEINSERTIONPROBABILITY_H_ */
