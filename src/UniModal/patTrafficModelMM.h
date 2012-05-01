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
	double integral(double ell_curr, double t_c);
};

#endif /* PATTRAFFICMODELMM_H_ */
