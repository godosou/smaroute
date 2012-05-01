/*
 * patTrafficModelMM.h
 *
 *  Created on: Aug 11, 2011
 *      Author: jchen
 */

#ifndef PATTRAFFICMODELMM_H_
#define PATTRAFFICMODELMM_H_

class patTrafficModelMM :public patTrafficModel{
public:
	void initiate();
	patReal integral(patReal ell_curr, patReal t_c);
};

#endif /* PATTRAFFICMODELMM_H_ */
