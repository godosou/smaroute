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
#include <tr1/unordered_map>
using namespace std::tr1;
using namespace std;
class patPostGISDataType {
public:
	patPostGISDataType();
	static pair<double,double> PointToLonLat(string lat_lon_string);
	static list<unsigned long> IntArrayToULongList(string str);
	static unordered_map<string,string> hstoreToMap(string str);
	virtual ~patPostGISDataType();
};

#endif /* PATPOSTGISDATATYPE_H_ */
