/*
 * patReadPathsFromKML.h
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

#ifndef PATREADPATHSFROMKML_H_
#define PATREADPATHSFROMKML_H_

#include <vector>
#include "patMultiModalPath.h"
#include "patNetworkElements.h"
using namespace std;
class patReadPathsFromKML {
public:
	patReadPathsFromKML();
	static map<patMultiModalPath,double> read(const patNetworkElements* m_network,string file_path);
	virtual ~patReadPathsFromKML();
};

#endif /* PATREADPATHSFROMKML_H_ */
