/*
 * MyPathProposal.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "MHPathProposal.h"

MHPathProposal::MHPathProposal(const patNode* origin,
		const patNode* destination, const patRouter* router,
		const double detour_cost_scale, const patRandomNumber* rnd) :
		m_origin(origin), m_destination(destination), m_detour_cost_scale(
				detour_cost_scale), m_rnd(rnd), m_router(router) {
	if (origin == NULL) {
		throw IllegalArgumentException("origin is NULL");
	}
	if (destination == NULL) {
		throw IllegalArgumentException("destination is NULL");
	}

	if (router == NULL) {
		throw IllegalArgumentException("router is NULL");
	}

	if (rnd == NULL) {
		throw IllegalArgumentException("rnd is NULL");
	}
}

 MHPoints MHPathProposal::drawPoints( int n,
		const patRandomNumber*  rnd) {
	/*
	 * (1) check
	 */
	DEBUG_MESSAGE(n);
	if (n < 3) {
		throw RuntimeException("less than three alternatives!");
	}
	/*
	 * (2) draw three disjoint numbers
	 */
	int u1 = rnd->nextInt(n-1);
	int u2 = rnd->nextInt(n-2);
	if (u2 >= u1) {
		u2++;
	}
	int u3 = rnd->nextInt(n-3);
	if (u3 >= u1 || u3>=u2) {
		u3++;
	}
	if (u3 >= u1 && u3>=u2) {
		u3++;
	}
	DEBUG_MESSAGE(u1<<","<<u2<<","<<u3);
	/*
	 * (3) return sorted numbers
	 */
	vector<int> us;
	us.push_back(u1);
	us.push_back(u2);
	us.push_back(u3);
	sort(us.begin(),us.end());
	MHPoints new_points(us[0],us[1],us[2]);
	DEBUG_MESSAGE("points"<<new_points);
	return new_points;
}

double MHPathProposal::transitionLogProb( MHPath& from_route,  MHPath& to_route) {
	if (from_route != to_route) {
		/*
		 * different routes -- a feasible SPLICE has occurred
		 */
		DEBUG_MESSAGE(" a splice occured");
		return log(0.5)
				+ log(
						from_route.getInsertProbs(m_detour_cost_scale)[to_route.getNodeB()]);
	} else {
		/*
		 * identical routes (this means that indices and nodes are equiv.)
		 */
		if (from_route.getA() == to_route.getA()
				&& from_route.getC() == to_route.getC()) {
			/*
			 * identical routes, identical points a and c
			 */
			if (from_route.getB() == to_route.getB()) {
				/*
				 * identical routes, identical points a, b, and c
				 */
				return 0.0;
			} else {
				/*
				 * identical routes, identical points a and c, different
				 * point b
				 */
				if (from_route.isSpliceable() && to_route.isSpliceable()) {
					/*
					 * only point b is different, splice was possible
					 */
					return log(0.5)
							+ log(
									from_route.getInsertProbs(
											m_detour_cost_scale)[
											to_route.getNodeB()]
											+ 1.0
													/ from_route.pointCombinationSize());
				} else {
					/*
					 * only point b is different, splice was not possible
					 */
					return -log(from_route.pointCombinationSize());
				}
			}
		} else {
			/*
			 * identical routes, different points a or c -- a shuffle must
			 * have occurred
			 */
			if (from_route.isSpliceable()) {
				return log(0.5) - log(from_route.pointCombinationSize());
			} else {
				return -log(from_route.pointCombinationSize());
			}
		}
	}
}
MHPath MHPathProposal::newInitialState() {
	patMultiModalPath new_path=  m_router->bestRoute(m_origin,
			m_destination);
		DEBUG_MESSAGE("nbr of nodes"<<new_path.nbrOfNodes());
	MHPoints points = drawPoints(new_path.nbrOfNodes(), m_rnd);
	return MHPath(new_path, points, m_router);
}

const patNode* MHPathProposal::drawInsertNode(const map<const patNode*, double>& probas) {
	double threshold = m_rnd->nextDouble();
	double sum = 0.0;
	for (map<const patNode*, double>::const_iterator iter = probas.begin();
			iter != probas.end(); ++iter) {
		sum += iter->second;
		if (sum >= threshold) {
			return iter->first;
		}
	}
	return NULL;
}
MHTransition<MHPath> MHPathProposal::newTransition( MHPath& from_route) {
	 MHPath to_route;
	if (from_route.isSpliceable() && m_rnd->nextDouble() < 0.5) {
		/*
		 * SPLICE
		 */
		DEBUG_MESSAGE("splice");
		const map<const patNode*, double> insertProbs =
				from_route.getInsertProbs(m_detour_cost_scale);
		const patNode* insertNode = drawInsertNode(insertProbs);
		if (insertNode == NULL) {
			to_route = from_route;
		} else {
			MHPath proposal_route  = from_route ;
			proposal_route.insertDetour(insertNode);
			if (proposal_route.containLoop()) {
				to_route = from_route;
			} else {
				to_route = proposal_route;
			}
		}
	} else {
		/*
		 * SHUFFLE
		 */
		DEBUG_MESSAGE("shuffle");
		to_route = from_route;
		to_route.setPoints(drawPoints(to_route.nbrOfNodes(), m_rnd));
	}
	DEBUG_MESSAGE("operation done");
	const double fwdLogProb = transitionLogProb(from_route, to_route);
	const double bwdLogProb = transitionLogProb(to_route, from_route);
	DEBUG_MESSAGE("probas"<<fwdLogProb<<","<<bwdLogProb);
	MHTransition<MHPath> path(from_route, to_route, fwdLogProb, bwdLogProb);
	return path;
}

MHPathProposal::~MHPathProposal(){

}
