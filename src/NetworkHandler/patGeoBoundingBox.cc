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

patGeoBoundingBox::patGeoBoundingBox(double tleft_top_lon, double tleft_top_lat,
		double tright_buttom_lon, double tright_buttom_lat) :
		left_top_lat(tleft_top_lat), left_top_lon(tleft_top_lon), right_buttom_lat(
				tright_buttom_lat), right_buttom_lon(tright_buttom_lon) {

}
patString patGeoBoundingBox::toString() const {
	string bb_box;
	stringstream bb_box_stream(bb_box);
	bb_box_stream << left_top_lon << " " << left_top_lat << ","
			<< right_buttom_lon << " " << right_buttom_lat;
	return bb_box_stream.str();
}

string patGeoBoundingBox::toPostGISString() const {
	string bb_box;
	stringstream bb_box_stream(bb_box);
	bb_box_stream << "st_makebox2d(" << "st_makepoint(" << getMinLon() << ","
			<< getMinLat() << "),st_makepoint(" << getMaxLon() << ","
			<< getMaxLat() << ")" << ")";
	return bb_box_stream.str();
}
double patGeoBoundingBox::getMinLat() const {
	return right_buttom_lat;
}
double patGeoBoundingBox::getMaxLat() const {
	return left_top_lat;
}
double patGeoBoundingBox::getMinLon() const {
	return left_top_lon;
}
double patGeoBoundingBox::getMaxLon() const {
	return right_buttom_lon;
}
patGeoBoundingBox::~patGeoBoundingBox() {
	//

} /* namespace pqxx */

bool patGeoBoundingBox::isInBox(double lat, double lon) const {
	if (lat > right_buttom_lat && lat < left_top_lat && lon > left_top_lon
			&& lon < right_buttom_lon) {
		return true;
	} else {
		return false;
	}
}

void patGeoBoundingBox::extend(const patGeoBoundingBox& bb) {
	left_top_lat =
			bb.getMaxLat() > left_top_lat ? bb.getMaxLat() : left_top_lat;
	left_top_lon =
			bb.getMinLon() < left_top_lon ? bb.getMinLon() : left_top_lon;

	right_buttom_lat =
			bb.getMinLat() < right_buttom_lat ?
					bb.getMinLat() : right_buttom_lat;
	right_buttom_lon =
			bb.getMaxLon() > right_buttom_lon ?
					bb.getMaxLon() : right_buttom_lon;

}

ostream& operator<<(ostream& str, const patGeoBoundingBox& x) {
	str << "BoudingBox:";
	str << "lat [" << x.getMinLat() << "-" << x.getMaxLat() << "]," << "lon ["
			<< x.getMinLon() << "-" << x.getMaxLon() << "],";
	return str;
}

bool patGeoBoundingBox::valid() const {
	if (isfinite(left_top_lat) && isfinite(left_top_lon)
			&& isfinite(right_buttom_lat) && isfinite(right_buttom_lon)) {
		return true;
	} else {
		return false;
	}
}
