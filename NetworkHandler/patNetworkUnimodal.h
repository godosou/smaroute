/*
 * patNetworkV2.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATNETWORKUNIMODAL_H_
#define PATNETWORKUNIMODAL_H_
#include <set>
#include <map>
#include "patArc.h"
#include "patWay.h"
#include "patType.h"
#include "patNetworkElements.h"
#include "patNetworkBase.h"
using namespace std;

class patNetworkUnimodal:public patNetworkBase {
public:
	patNetworkUnimodal();
	patBoolean addWay(const patWay*  the_way,patBoolean reverse);
	virtual ~patNetworkUnimodal();
protected:
};

#endif /* PATNETWORKV2_H_ */
