/*
 * patVerifyingSamplingResult.h
 *
 *  Created on: Jun 7, 2012
 *      Author: jchen
 */

#ifndef PATVERIFYINGSAMPLINGRESULT_H_
#define PATVERIFYINGSAMPLINGRESULT_H_
#include "patMultiModalPath.h"
#include "patRandomNumber.h"
#include <map>
using namespace std;
class patNetworkElements;
class patChoiceSet;
class patPathGenerator;
class patVerifyingSamplingResult {
public:
	patVerifyingSamplingResult(string folder_name, const patNetworkElements* network,const patRandomNumber& rnd);
	void getpaths();
	list<string> getFiles() const;
	double verifyProbability(const patChoiceSet& universal_set, const patPathGenerator* path_generator) const;
	virtual ~patVerifyingSamplingResult();
protected:
	string m_folder_name;
	map<const patMultiModalPath, pair<int, double> > m_paths;
	int m_total_nbr_of_paths;
	double m_total_probas;
	const patNetworkElements* m_network;
	const patRandomNumber& m_rnd;
}
;

#endif /* PATVERIFYINGSAMPLINGRESULT_H_ */
