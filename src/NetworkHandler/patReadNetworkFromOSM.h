/*
 * patReadNetworkFromOSM.h
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#ifndef PATREADNETWORKFROMOSM_H_
#define PATREADNETWORKFROMOSM_H_
#include "patNetworkElements.h"
#include "patGeoBoundingBox.h"
class patReadNetworkFromOSM {
public:
	patReadNetworkFromOSM();
	static void read(string file_name, patNetworkElements& network_element, patGeoBoundingBox& bb);
	virtual ~patReadNetworkFromOSM();
};

#endif /* PATREADNETWORKFROMOSM_H_ */
