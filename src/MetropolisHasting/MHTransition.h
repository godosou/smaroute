/*
 * MHTransition.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef MHTRANSITION_H_
#define MHTRANSITION_H_

template<class S>
class MHTransition {
public:


	// -------------------- CONSTRUCTION --------------------

	MHTransition(S oldState, S newState, double fwdLogProb, double bwdLogProb) {
		m_oldState = oldState;
		m_newState = newState;
		m_fwdLogProb = fwdLogProb;
		m_bwdLogProb = bwdLogProb;
	}

	// -------------------- CONTENT ACCESS --------------------

	S getOldState() {
		return m_oldState;
	}

	S getNewState() {
		return m_newState;
	}

	double getFwdLogProb() {
		return m_fwdLogProb;
	}

	double getBwdLogProb() {
		return m_bwdLogProb;
	}
protected:
	double m_fwdLogProb;
	S m_oldState;
	S m_newState;
	double m_bwdLogProb;

};

#endif /* MHTRANSITION_H_ */
