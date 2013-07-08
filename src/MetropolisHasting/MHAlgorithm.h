/*
 * MHAlgorithm.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef MHALGORITHM_H_
#define MHALGORITHM_H_
#include <vector>
#include "patDisplay.h"
#include "MHProposal.h"
#include "MHWeight.h"
#include "patException.h"
#include "MHStateProcessor.h"
#include <patRandomNumber.h>
#include "patDisplay.h"
#include <iostream>
using namespace std;
template<class S>
class MHAlgorithm {
public:

	// -------------------- CONSTRUCTION --------------------

	MHAlgorithm(MHProposal<S>* proposal, const MHWeight<S>* weight,
			const patRandomNumber* rnd) :
			m_msgInterval(1), m_lastCompTime_ms(0.0), m_initialState(NULL), m_proposal(
					proposal), m_weight(weight), m_rnd(rnd) {
		if (proposal == NULL) {
			throw IllegalArgumentException("proposal is null");
		}
		if (weight == NULL) {
			throw IllegalArgumentException("weight is null");
		}
		if (rnd == NULL) {
			throw IllegalArgumentException("rnd is null");
		}
	}

	// -------------------- SETTERS AND GETTERS --------------------

	void setInitialState(const S initialState) {
		m_initialState = initialState;
	}

	S getInitialState() {
		return m_initialState;
	}

	void setMsgInterval(const int msgInterval) {
		if (msgInterval < 1) {
			WARNING("message interval < 1");
			throw IllegalArgumentException("message interval < 1");
		}
		m_msgInterval = msgInterval;
	}

	int getMsgInterval() {
		return m_msgInterval;
	}

	void addStateProcessor(MHStateProcessor<S>* stateProcessor) {
		if (stateProcessor == NULL) {
			WARNING("state processor is null");
			throw IllegalArgumentException("state processor is null");
		}
		m_state_processors.push_back(stateProcessor);
	}

	long getLastCompTime_ms() {
		return m_lastCompTime_ms;
	}

	// -------------------- IMPLEMENTATION --------------------

	void run(const int iterations) {

		m_lastCompTime_ms = 0;

		/*
		 * initialize (iteration 0)
		 */
//		DEBUG_MESSAGE("initiate");
		for (int p_iter = 0; p_iter < m_state_processors.size(); ++p_iter) {
			m_state_processors[p_iter]->start();
		}
		S currentState;
		if (m_initialState != NULL) {
			currentState = *m_initialState;
		} else {
	//		DEBUG_MESSAGE("new initiate");
			currentState = m_proposal->newInitialState();
//			DEBUG_MESSAGE("state initiated");
		}
		double currentLogWeight = m_weight->logWeight(currentState);

//		DEBUG_MESSAGE(
//				"start processors with current log weight"<<currentLogWeight);
		for (typename vector<MHStateProcessor<S>*>::iterator p_iter =
				m_state_processors.begin(); p_iter != m_state_processors.end();
				++p_iter) {
			(*p_iter)->processState(currentState,m_weight->logWeightWithoutCorrection(currentState));
		}

//		DEBUG_MESSAGE("start iterators");
		/*
		 * iterate (iterations 1, 2, ...)
		 */
		int accept_transition = 0;
		for (int i = 1; i <= iterations; i++) {
//			DEBUG_MESSAGE(i);
			if (i % m_msgInterval == 0) {
				cout<<"MH iteration "<< i<<" transtions:"<<accept_transition<<endl;
				//DEBUG_MESSAGE("  state  = " << currentState);
//				DEBUG_MESSAGE("\tlog  weight = " << currentLogWeight);
			}

			MHTransition<S> proposalTransition = m_proposal->newTransition(
					currentState);
			S proposalState = proposalTransition.getNewState();

			double proposalLogWeight = m_weight->logWeight(proposalState);

			double logAlpha = (proposalLogWeight - currentLogWeight)
					+ (proposalTransition.getBwdLogProb()
							- proposalTransition.getFwdLogProb());
			double r_number = m_rnd->nextDouble();
//			double r_number = 0.0;
//			DEBUG_MESSAGE(m_weight->logWeightWithoutCorrection(currentState)<<","<<m_weight->logWeightWithoutCorrection(proposalState));
//            DEBUG_MESSAGE(proposalLogWeight <<","<<currentLogWeight<<","<<proposalTransition.getBwdLogProb()<<","<< proposalTransition.getFwdLogProb());
//			DEBUG_MESSAGE(log(r_number)<<","<<logAlpha);
			if (log(r_number) < logAlpha) {
				accept_transition++;
				currentState = proposalState;
				currentLogWeight = proposalLogWeight;
			}

			for (typename vector<MHStateProcessor<S>*>::iterator p_iter =
					m_state_processors.begin();
					p_iter != m_state_processors.end(); ++p_iter) {
				(*p_iter)->processState(currentState,m_weight->logWeightWithoutCorrection(currentState));
			}

			/*
			 for(int p_iter=0; p_iter< m_state_processors.size();++p_iter){
			 m_state_processors[p_iter]->processState(currentState);
			 }
			 */
		}

		for (typename vector<MHStateProcessor<S>*>::iterator p_iter =
				m_state_processors.begin(); p_iter != m_state_processors.end();
				++p_iter) {
			(*p_iter)->end();
		}

		/*
		 for(int p_iter=0; p_iter< m_state_processors.size();++p_iter){
		 m_state_processors[p_iter]->end();
		 }
		 */
	}
protected:
	MHProposal<S>* m_proposal;

	const MHWeight<S>* m_weight;

	const patRandomNumber* m_rnd;
	S* m_initialState;

	vector<MHStateProcessor<S>*> m_state_processors;

	int m_msgInterval;

	long m_lastCompTime_ms;

};

#endif /* MHALGORITHM_H_ */
