/*
 * MyPathProposal.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "MHPathProposal.h"
#include "patTimeFunctions.h"
MHPathProposal::MHPathProposal(const patNode* origin,
		const patNode* destination, const patRouter* router,
		unordered_map<const patNode*, double>& proposalProbabilities,
		const double detour_cost_scale, const patRandomNumber* rnd) :
		m_origin(origin), m_destination(destination), m_proposalProbabilities(
				proposalProbabilities), m_detour_cost_scale(detour_cost_scale), m_rnd(
				rnd), m_router(router) ,m_splice_probability(0.8){
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

MHPoints MHPathProposal::drawPoints(int n, const patRandomNumber* rnd) {
	/*
	 * (1) check
	 */
	if (n < 3) {
		throw RuntimeException("less than three alternatives!");
	}
	/*
	 * (2) draw three disjoint numbers
	 */
	int u1 = rnd->nextInt(n);
	int u2 = rnd->nextInt(n - 1);
	if (u2 >= u1) {
		u2++;
	}
	int u3 = rnd->nextInt(n - 2);
	if (u3 >= u1 || u3 >= u2) {
		u3++;
	}
	if (u3 >= u1 && u3 >= u2) {
		u3++;
	}
	//	DEBUG_MESSAGE(u1 << "," << u2 << "," << u3);
	/*
	 * (3) return sorted numbers
	 */
	vector<int> us;
	us.push_back(u1);
	us.push_back(u2);
	us.push_back(u3);
	sort(us.begin(), us.end());
	MHPoints new_points(us[0], us[1], us[2]);
	return new_points;
}

double MHPathProposal::transitionLogProb(MHPath& from_route, MHPath& to_route,
		bool splice) {
	if (splice == true) {
		/*
		 * different routes -- a feasible SPLICE has occurred
		 */
		//		DEBUG_MESSAGE(" a splice occured");
		return log(m_splice_probability)
				+ log(m_proposalProbabilities.find(to_route.getNodeB())->second);
	} else {
		/*
		 * identical routes (this means that indices and nodes are equiv.)
		 */
		if (from_route.getA() == to_route.getA()
				&& from_route.getC() == to_route.getC()) {
			//	DEBUG_MESSAGE("identical AC");
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
				/* identical routes, identical points a and c, different
				 * point b -- a feasible SPLICE must have occurred because
				 * shuffle-b-only is not allowed
				 */
				return log(m_splice_probability)
						+ log(
								m_proposalProbabilities.find(
										to_route.getNodeB())->second);
			}

		} else {
			/*
			 * identical routes, different points a or c -- a shuffle must
			 * have occurred
			 */
			//DEBUG_MESSAGE("different AC");
			unsigned long forbiddenSize = from_route.getC() - from_route.getA()
					- 2;

			if (from_route.isSpliceable()) {
				return log(1.0 - m_splice_probability)
						- log(from_route.pointCombinationSize() - forbiddenSize);
			} else {
				return -log(from_route.pointCombinationSize() - forbiddenSize);
			}
		}
	}
}
double MHPathProposal::transitionLogProb(MHPath& from_route, MHPath& to_route) {
	if (from_route != to_route) {
		return transitionLogProb(from_route, to_route, true);
	} else {
		return transitionLogProb(from_route, to_route, false);
	}
}
MHPath MHPathProposal::newInitialState() {
	patMultiModalPath new_path = m_router->bestRouteFwd(m_origin,
			m_destination);
//	cout<<"shortest path:"<<new_path.getLength()<<endl;
	//	DEBUG_MESSAGE("nbr of nodes" << new_path.nbrOfNodes());
	MHPoints points = drawPoints(new_path.nbrOfNodes(), m_rnd);
	MHPath mh_path(new_path, points, m_router);
//	 mh_path.update(m_proposalProbabilities);
//	 mh_path.drawPoints(m_rnd);
	return mh_path;
}

const patNode* MHPathProposal::drawInsertNode() {
	double threshold = m_rnd->nextDouble();
	double sum = 0.0;
	for (unordered_map<const patNode*, double>::const_iterator iter =
			m_proposalProbabilities.begin();
			iter != m_proposalProbabilities.end(); ++iter) {
		sum += iter->second;
		if (sum >= threshold) {
			return iter->first;
		}
	}
	throw RuntimeException("no node is selected");
	return NULL;
}
//int tran_count = 0;
//int splice_count = 0;
//int shuffle_count = 0;
//
//double cal_insert_time = 0.0;
//double draw_insert_time = 0.0;
//double cal_transition_time = 0.0;
//double insert_detour_time = 0.0;
//double detect_loop_time = 0.0;
//double isspliceable_time = 0.0;
//double set_points_time = 0.0;
//double cal_transition_proba_time = 0.0;
//double cal_transition_proba_splice_time = 0.0;
//double cal_transition_proba_shuffle_time = 0.0;
MHTransition<MHPath> MHPathProposal::newTransition(MHPath& from_route) {
	MHPath to_route;
	bool splice_flag = false;

	//	double start_time = getMillisecond();
	if (m_rnd->nextDouble() < m_splice_probability && from_route.isSpliceable()) {
		//		splice_count++;
		//		double isspliceable_t = getMillisecond();
		//		isspliceable_time += (isspliceable_t - start_time);
		/*
		 * SPLICE
		 */
//		cout<<"try splice"<<endl;
		//		DEBUG_MESSAGE("splice");
		//		const unordered_map<const patNode*, double> insertProbs =
		//				from_route.getInsertProbs(m_detour_cost_scale);
		//		double cal_insert_t = getMillisecond();
		//		cal_insert_time += (cal_insert_t - isspliceable_t);
		const patNode* insertNode = drawInsertNode();
		//		double draw_insert_t = getMillisecond();
		//		draw_insert_time += (draw_insert_t - cal_insert_t);
		if (insertNode == NULL
				|| from_route.containsNodeFront(insertNode, from_route.getA())
				|| from_route.containsNodeBack(insertNode,
						from_route.size() - from_route.getC())) {
//			cout<<"ivalide node"<<endl;
			to_route = from_route;
		} else {
			MHPath proposal_route = from_route;
			if (proposal_route.insertDetour(insertNode,
					m_proposalProbabilities)) {
				//			double insert_detour_t = getMillisecond();
				//			insert_detour_time += (insert_detour_t - draw_insert_t);

				if (proposal_route.containLoop()!=NULL) {
//					cout<< "contain loop"<<endl;
					//				DEBUG_MESSAGE("CONTAIN LOOP");
					//				double detect_loop_t = getMillisecond();
					//				detect_loop_time += (detect_loop_t - insert_detour_t);
					to_route = from_route;
				} else {
					//				double detect_loop_t = getMillisecond();
					//				detect_loop_time += (detect_loop_t - insert_detour_t);
//                        DEBUG_MESSAGE("a splice");
//					cout << "splice" << endl;
					to_route = proposal_route;
					splice_flag = true;
				}
			} else {
//				cout<<"invalide detour"<<endl;
				to_route = from_route;
			}
		}
	} else {
		//		shuffle_count++;
		//		double isspliceable_t = getMillisecond();
		//		isspliceable_time += (isspliceable_t - start_time);
		/*
		 * SHUFFLE
		 */
//		cout<<"shuffle"<<endl;
		to_route = from_route;
		MHPoints new_points;
		do {
			new_points = drawPoints(to_route.nbrOfNodes(), m_rnd);
		} while (from_route.getA() == new_points.getA()
				&& from_route.getC() == new_points.getC()
				&& !from_route.getB() == new_points.getB());
		to_route.setPoints(new_points);
		//		double set_points_t = getMillisecond();
		//		set_points_time += (set_points_t - isspliceable_t);

	}
	//	double finish_operation_t =  getMillisecond();
	double fwdLogProb = transitionLogProb(from_route, to_route, splice_flag);
	double bwdLogProb = transitionLogProb(to_route, from_route, splice_flag);
	//
	//	double cal_transition_proba_t =  getMillisecond();
	//	if (splice_flag){
	//		cal_transition_proba_splice_time+=(cal_transition_proba_t-finish_operation_t);
	//	}
	//	else{
	//		cal_transition_proba_shuffle_time+=(cal_transition_proba_t-finish_operation_t);
	//	}
	//	if ((shuffle_count + splice_count) % 50000 == 0) {
	//		DEBUG_MESSAGE(
	//				"shuffle " << shuffle_count << " splice: " << splice_count);
	//		DEBUG_MESSAGE("\t isspliceable_time" << isspliceable_time);
	//		DEBUG_MESSAGE("shuffle ");
	//		DEBUG_MESSAGE(
	//				"\t set points \t " << set_points_time << "\t sp updated");
	//		DEBUG_MESSAGE(
	//				"\t calculate proba \t " << cal_transition_proba_shuffle_time << "\t calculate transition proba");
	//		DEBUG_MESSAGE("splice ");
	//		DEBUG_MESSAGE(
	//				"\t insert proba \t " << cal_insert_time << "\t calculate insert proba");
	//		DEBUG_MESSAGE(
	//				"\t draw node \t " << draw_insert_time << "\t draw insert node");
	//		DEBUG_MESSAGE(
	//				"\t insert detour \t " << insert_detour_time << "\t insert detour with node");
	//		DEBUG_MESSAGE(
	//				"\t detect loop \t " << detect_loop_time << "\t detect loop");
	//		DEBUG_MESSAGE(
	//				"\t calculate proba \t " << cal_transition_proba_splice_time << "\t calculate transition proba");
	//	}
	//	DEBUG_MESSAGE("operation done");
	//DEBUG_MESSAGE("probas" << fwdLogProb << "," << bwdLogProb);
	MHTransition<MHPath> path(from_route, to_route, fwdLogProb, bwdLogProb);
	return path;
}

MHPathProposal::~MHPathProposal() {

}
