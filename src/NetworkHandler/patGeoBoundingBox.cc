/*
 * patGeoBoundingBox.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#include "patGeoBoundingBox.h"
#include "patType.h"
#include <iostream>
#include <sstream>
#include "patDisplay.h"

using namespace std;

patGeoBoundingBox::patGeoBoundingBox(double tlat_top_lon, double tleft_top_lat,
		double tright_buttom_lon, double tright_buttom_lat) :
		left_top_lat(tleft_top_lat), left_top_lon(tlat_top_lon), right_buttom_lat(
				tright_buttom_lat), right_buttom_lon(tright_buttom_lon) {

}
patString patGeoBoundingBox::toString() {
	string bb_box;
	stringstream bb_box_stream(bb_box);
	bb_box_stream << left_top_lon << " " << left_top_lat << ","
			<< right_buttom_lon << " " << right_buttom_lat;
	return bb_box_stream.str();
}

double patGeoBoundingBox::getMinLat() const{
	return right_buttom_lat;
}
double patGeoBoundingBox::getMaxLat() const{
	return left_top_lat;
}
double patGeoBoundingBox::getMinLon() const{
	return left_top_lon;
}
double patGeoBoundingBox::getMaxLon() const{
	return right_buttom_lon;
}
patGeoBoundingBox::~patGeoBoundingBox() {
	//

} /* namespace pqxx */
