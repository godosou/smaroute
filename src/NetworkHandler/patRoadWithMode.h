/*
 * patRoadWithMode.h
 *
 *  Created on: Feb 29, 2012
 *      Author: jchen
 */

#ifndef PATROADWITHMODE_H_
#define PATROADWITHMODE_H_

#include "patRoadBase.h"
#include "patTransportMode.h"
class patRoadWithMode: public patRoadBase {
public:
	patRoadWithMode();
	virtual ~patRoadWithMode();
protected:
	patRoadBase* m_road;
	TranportMode* m_mode;
	string m_ref;
};

#endif /* PATROADWITHMODE_H_ */
