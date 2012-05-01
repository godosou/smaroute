/*
 * patCreateKmlStyles.h
 *
 *  Created on: Nov 23, 2011
 *      Author: jchen
 */

#ifndef PATCREATEKMLSTYLES_H_
#define PATCREATEKMLSTYLES_H_
#include "kml/dom.h"
using kmldom::DocumentPtr;

class patCreateKmlStyles {
public:
	patCreateKmlStyles();
	 DocumentPtr createStylesForKml();
	virtual ~patCreateKmlStyles();
};

#endif /* PATCREATEKMLSTYLES_H_ */
