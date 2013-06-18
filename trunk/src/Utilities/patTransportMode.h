/*
 * patTransportMode.h
 *
 *  Created on: Aug 5, 2011
 *      Author: jchen
 */

#ifndef PATTRANSPORTMODE_H_
#define PATTRANSPORTMODE_H_

#include "patType.h"
enum TransportMode {WALK,BIKE,BUS,METRO,TRAIN,CAR, NONE};

bool isPublicTransport(TransportMode m);
patString getTransportMode( TransportMode m);
patString getTransportModeString( TransportMode m);
#endif /* PATTRANSPORTMODE_H_ */
