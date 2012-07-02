/*
 * MyPathProposal.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef MHPATHPROPOSAL_H_
#define MHPATHPROPOSAL_H_

#include "MHPath.h"
#include "MHProposal.h"
#include "patRandomNumber.h"
#include "patException.h"
#include "patRouter.h"
#include <algorithm>
#include "patNode.h"
#include <tr1/unordered_map>
using namespace std;
using namespace std::tr1;
class MHPathProposal: public MHProposal<MHPath> {
public:
	MHPathProposal(const patNode* origin, const patNode* destination,
			const patRouter* router, unordered_map<const patNode*, double>& proposalProbabilities,
			const double detour_cost_scale,
			const patRandomNumber* rnd);
	static MHPoints drawPoints(int n, const patRandomNumber* rnd);
	double transitionLogProb(MHPath& from_route, MHPath& to_route);
	MHPath newInitialState();
	const patNode* drawInsertNode();
	MHTransition<MHPath> newTransition(MHPath& from_route);
	double transitionLogProb(MHPath& from_route, MHPath& to_route, bool splice);

	virtual ~MHPathProposal();
private:

	const unordered_map<const patNode*, double>& m_proposalProbabilities;
	const patNode* m_origin;
	const patNode* m_destination;
	const patRandomNumber* m_rnd;

	const double m_detour_cost_scale;

	const patRouter* m_router;
};

#endif /* MYPATHPROPOSAL_H_ */
