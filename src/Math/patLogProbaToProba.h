/*
 * patLogProbaToProba.h
 *
 *  Created on: Jun 19, 2012
 *      Author: jchen
 */

#ifndef PATLOGPROBATOPROBA_H_
#define PATLOGPROBATOPROBA_H_
#include <vector>
using namespace std;
class patLogProbaToProba {
public:
	patLogProbaToProba();
	vector<double> operator()(vector<double>& log_probas);
	virtual ~patLogProbaToProba();
};

#endif /* PATLOGPROBATOPROBA_H_ */
