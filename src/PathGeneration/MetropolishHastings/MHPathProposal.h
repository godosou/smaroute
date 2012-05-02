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
using namespace std;
class MHPathProposal: public MHProposal<MHPath> {
public:
	MHPathProposal(const patNode* origin, const patNode* destination,
			const patRouter* router, const double detour_cost_scale,
			const patRandomNumber* rnd);
	static MHPoints drawPoints( int n,
			const patRandomNumber*  rnd) ;
	double transitionLogProb( MHPath& from_route,  MHPath& to_route);
	MHPath newInitialState() ;
	const patNode* drawInsertNode(const map<const patNode*, double>& probas);
	MHTransition<MHPath> newTransition( MHPath& from_route);
	virtual ~MHPathProposal();
private:

	const patNode* m_origin;
	const patNode* m_destination;
	const patRandomNumber* m_rnd;

	const double m_detour_cost_scale;

	const patRouter* m_router;
};

#endif /* MYPATHPROPOSAL_H_ */
