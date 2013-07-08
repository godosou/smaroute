/*
 * patWriteBiogemeData.h
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#ifndef PATWRITEBIOGEMEDATA_H_
#define PATWRITEBIOGEMEDATA_H_
#include "patObservation.h"
#include <set>;
#include <list>
#include "patChoiceSet.h"
#include "patUtilityFunction.h"
#include "patPathGenerator.h"
class patRandomNumber;
using namespace std;
class patWriteBiogemeData {
public:
	patWriteBiogemeData(const vector<patObservation>& observations,const patUtilityFunction* utility_function,
			const patPathGenerator* path_generator,
			const patChoiceSet* choice_set, const patRandomNumber& rnd);
	void writeHeader(const string& fileName);
	void writeData(const string& fileName, const unsigned& choiceset_size);
	void genHeader(const unsigned& choiceset_size);
	virtual ~patWriteBiogemeData();
	void writeSampleFile(const string& fileName, const string& sample_prefix);
	void writeSpecFile(const string& fileName, const unsigned& choiceset_size);
protected:
	const vector<patObservation>& m_observations;
	vector<string> m_header;
	const patUtilityFunction* m_utility_function;
	const patPathGenerator* m_path_generator;
	const patChoiceSet* m_universal_choiceset;
	const patRandomNumber& m_rnd;
};

#endif /* PATWRITEBIOGEMEDATA_H_ */
