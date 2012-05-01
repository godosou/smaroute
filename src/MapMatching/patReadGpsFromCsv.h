/*
 * patReadGpsFromCsv.h
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#ifndef PATREADGPSFROMCSV_H_
#define PATREADGPSFROMCSV_H_
#include <vector>
#include "patError.h"
#include "patGpsPoint.h"

class patReadGpsFromCsv {
public:
	patReadGpsFromCsv();
	static void read(vector<patGpsPoint>& gps_sequence, string file_name, patError*& err);
	 ~patReadGpsFromCsv();
};

#endif /* PATREADGPSFROMCSV_H_ */
