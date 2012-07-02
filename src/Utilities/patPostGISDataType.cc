/*
 * patPostGISDataType.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#include "patPostGISDataType.h"
#include <boost/xpressive/xpressive.hpp>
#include "patType.h"
#include "patConst.h"
#include "patDisplay.h"
using namespace boost::xpressive;
#include <iostream>

patPostGISDataType::patPostGISDataType() {


}
pair<double , double > patPostGISDataType::PointToLonLat(
		string lat_lon_string) {

	//string example "POINT(6.1666183 46.2721863)"
	//DEBUG_MESSAGE(lat_lon_string);
	sregex rex = sregex::compile("\\w+\\W(\\d+\\W\\d+)\\s(\\d+\\W\\d+)\\W");
	smatch what;
	double  lon = DBL_MAX;
	double  lat = DBL_MAX;
	if (regex_match(lat_lon_string, what, rex)) {
		std::istringstream i1(what[1].str().c_str());
		i1 >> lon;
		std::istringstream i2(what[2].str().c_str());
		i2 >> lat;
	}
	return pair<double , double >(lon, lat);
}
list<unsigned long> patPostGISDataType::IntArrayToULongList(string str) {
	sregex time = sregex::compile("(\\d+)");
	int const subs[] = { 1 };
	list<unsigned long> long_list;

	sregex_token_iterator cur(str.begin(), str.end(), time, subs);
	sregex_token_iterator end;
	for (; cur != end; ++cur) {
		std::istringstream i(*cur);
		unsigned long new_long;
		i >> new_long;
		long_list.push_back(new_long);
	}
	return long_list;
}

unordered_map<string, string> patPostGISDataType::hstoreToMap(string str) {
	//""bicycle"=>"yes", "highway"=>"path", "surface"=>"earth""
	sregex rex = sregex::compile("\"(\\w+)\"=>\"([\\w\\d\\s]*)\"");

	int const subs[] = { 1, 2};
	unordered_map<string, string>  string_map;
	sregex_token_iterator cur(str.begin(), str.end(), rex, subs);
	sregex_token_iterator end;
	string the_key="";

	for (; cur != end; ++cur) {
		if (the_key.empty()){
			the_key=*cur;
		}
		else{
			string the_value;
			string_map[the_key]=*cur;
			the_key="";
		}
	}
	return string_map;
}

patPostGISDataType::~patPostGISDataType() {
	//
}

