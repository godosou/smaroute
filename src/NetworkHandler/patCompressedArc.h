/*
 * patCompressedArc.h
 *
 *  Created on: Jul 20, 2012
 *      Author: jchen
 */

#ifndef PATCOMPRESSEDARC_H_
#define PATCOMPRESSEDARC_H_

#include "patArc.h"
#include "patException.h"
#include "kml/dom.h"
using kmldom::PlacemarkPtr;
class patCompressedArc: public patArc {
public:
	patCompressedArc(const vector<const patRoadBase*>& roads);
//	vector<const patArc*> getArcList() const {
//		return m_arcs;
//	};
	vector<PlacemarkPtr> getArcKML(string mode) const;
	vector<const patArc*> getOriginalArcList() const;
	virtual ~patCompressedArc();
	double computeGeneralizedCost();
	void summarize();
	double getAttribute(ARC_ATTRIBUTES_TYPES attribute_name) const;
private:
	vector<const patArc*> m_arcs;
};

#endif /* PATCOMPRESSEDARC_H_ */
