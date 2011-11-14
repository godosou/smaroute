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
	patGeoBoundingBox(patReal tleft_top_lat, patReal tlat_top_lon,
			patReal tright_buttom_lat,patReal tright_buttom_lon);
	patString toString();
	virtual ~patGeoBoundingBox();
protected:
	patReal left_top_lat;
	patReal left_top_lon;
	patReal right_buttom_lat;
	patReal right_buttom_lon;
};

#endif /* PATGEOBOUNDINGBOX_H_ */
