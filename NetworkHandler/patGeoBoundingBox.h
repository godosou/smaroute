/*
 * patGeoBoundingBox.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATGEOBOUNDINGBOX_H_
#define PATGEOBOUNDINGBOX_H_
#include "patType.h"

class patGeoBoundingBox {
public:
	patGeoBoundingBox(double tleft_top_lat, double tlat_top_lon,
			double tright_buttom_lat,double tright_buttom_lon);
	patString toString();
	virtual ~patGeoBoundingBox();
protected:
	double left_top_lat;
	double left_top_lon;
	double right_buttom_lat;
	double right_buttom_lon;
};

#endif /* PATGEOBOUNDINGBOX_H_ */
