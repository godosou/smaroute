/*
 * patRandomNumber.cc
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#include "patRandomNumber.h"
#include "patException.h"
#include <stdlib.h>
patRandomNumber::patRandomNumber(unsigned long seed) :
		m_seed(seed) {
	srand(m_seed);
}

patRandomNumber::~patRandomNumber() {
}

double patRandomNumber::nextDouble() const {
	return ((double) rand() / (RAND_MAX));
}

int patRandomNumber::nextInt(int n) const {

	if (n == 0) {
		return 0;
	} else if (n > 0) {
		return rand() % n;
	}
	else{
		WARNING("negative n"<<n);
		throw RuntimeException("negative n");
		return -1;
	}
}

int patRandomNumber::sampleWithProba(map<int, double> probas) const {
	double threshold = nextDouble();
	try {
		normalizeProbas(probas);
	}

	catch (RuntimeException& e) {
		DEBUG_MESSAGE(e.what());
		return -1;
	}
	double sum = 0.0;
	for (map<int, double>::iterator iter = probas.begin(); iter != probas.end();
			++iter) {
		sum += iter->second;
		if (sum > threshold) {
			return iter->first;
		}
	}
	throw RuntimeException("Nothing generated");
	return -1;

}

bool patRandomNumber::normalizeProbas(map<int, double>& probas) const {
	double sum = 0.0;

	for (map<int, double>::iterator iter = probas.begin(); iter != probas.end();
			++iter) {
		sum += iter->second;
	}

	if (sum == 0.0) {
		//WARNING("Invalid proba, all zero");
		throw RuntimeException("sum proba zero");
		return false;
	}

//	DEBUG_MESSAGE(sum);
	for (map<int, double>::iterator iter = probas.begin(); iter != probas.end();
			++iter) {
		probas[iter->first] = probas[iter->first] / sum;
	}
	return true;

}
