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
using namespace std;
class patWriteBiogemeData {
public:
	patWriteBiogemeData(const vector<patObservation>& observations,const patUtilityFunction* utility_function,
			const patPathGenerator* path_generator,
			const patChoiceSet* choice_set);
	void writeHeader(const string& fileName);
	void writeData(const string& fileName);
	void genHeader();
	virtual ~patWriteBiogemeData();
	void writeSampleFile(const string& fileName);
	void writeSpecFile(const string& fileName);
protected:
	const vector<patObservation>& m_observations;
	vector<string> m_header;
	const patUtilityFunction* m_utility_function;
	const patPathGenerator* m_path_generator;
	const patChoiceSet* m_universal_choiceset;
};

#endif /* PATWRITEBIOGEMEDATA_H_ */
