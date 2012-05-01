/*
 * patPostGISDataType.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATPOSTGISDATATYPE_H_
#define PATPOSTGISDATATYPE_H_
#include "patGeoCoordinates.h"
#include "patType.h"
#include <list>
#include <map>
using namespace std;
class patPostGISDataType {
public:
	patPostGISDataType();
	static pair<double,double> PointToLonLat(patString lat_lon_string);
	static list<unsigned long> IntArrayToULongList(patString str);
	static map<patString,patString> hstoreToMap(patString str);
	virtual ~patPostGISDataType();
};

#endif /* PATPOSTGISDATATYPE_H_ */
