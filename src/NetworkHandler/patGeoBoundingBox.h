/*
 * patGeoBoundingBox.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATGEOBOUNDINGBOX_H_
#define PATGEOBOUNDINGBOX_H_
#include "patType.h"
#include <iostream>
class patGeoBoundingBox {
	friend ostream& operator<<(ostream& str, const patGeoBoundingBox& x);
public:
	patGeoBoundingBox() {
		left_top_lat = -DBL_MAX; //maxlat
		left_top_lon = DBL_MAX; //minlon
		right_buttom_lat = DBL_MAX; //minlat
		right_buttom_lon = -DBL_MAX; //maxlon

	}
	;

	void extend(const patGeoBoundingBox& bb);
	/*
	 * maxlat, minlon, minlat, maxlon
	 */
	patGeoBoundingBox(double tleft_top_lat, double tlat_top_lon,
			double tright_buttom_lat, double tright_buttom_lon);
	patString toString() const;

	bool isInBox(double lat, double lon) const;

	bool valid() const;
	double getMinLat() const;
	double getMaxLat() const;
	double getMinLon() const;
	double getMaxLon() const;
	virtual ~patGeoBoundingBox();
protected:
	double left_top_lat; //maxlat
	double left_top_lon; //minlon
	double right_buttom_lat; //minlat
	double right_buttom_lon; //maxlon
};

#endif /* PATGEOBOUNDINGBOX_H_ */
