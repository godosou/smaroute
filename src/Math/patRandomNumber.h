/*
 * patRandomNumber.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef PATRANDOMNUMBER_H_
#define PATRANDOMNUMBER_H_
#include <boost/random.hpp>
#include <map>
using namespace std;
class patRandomNumber {
public:
	patRandomNumber(unsigned long);
	double nextDouble() const;
	int nextInt(int n) const;//including n
	int sampleWithProba(map<int, double> probas) const;
	bool normalizeProbas(map<int, double>& probas) const;
	virtual ~patRandomNumber();
protected:
	 unsigned long m_seed;
};

#endif /* PATRANDOMNUMBER_H_ */
