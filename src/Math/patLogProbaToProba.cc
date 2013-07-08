/*
 * patLogProbaToProba.cc
 *
 *  Created on: Jun 19, 2012
 *      Author: jchen
 */

#include "patLogProbaToProba.h"
#include <math.h>
#include <float.h>
patLogProbaToProba::patLogProbaToProba() {
	// TODO Auto-generated constructor stub

}

vector<double> patLogProbaToProba::operator()(vector<double>& log_probas) {
	double min_log_proba=DBL_MAX;

	vector<double> probas;
	for (unsigned i = 0; i < log_probas.size(); ++i) {
		double log_proba = log_probas[i];
		min_log_proba = log_proba < min_log_proba ? log_proba : min_log_proba;
		probas.push_back(log_proba);
	}

	double weight_sum = 0.0;

	for (unsigned i = 0; i < probas.size(); ++i) {
		double log_weight = probas[i];
		double weight=exp(log_weight - min_log_proba);
		weight_sum += weight;
		probas[i]= weight;
	}
	for (unsigned i = 0; i < probas.size(); ++i) {
		probas[i]/= weight_sum;
	}
	return probas;
}
patLogProbaToProba::~patLogProbaToProba() {
	// TODO Auto-generated destructor stub
}

