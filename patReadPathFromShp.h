/*
 * patReadPathFromShp.h
 *
 *  Created on: Aug 15, 2011
 *      Author: jchen
 */

#ifndef PATREADPATHFROMSHP_H_
#define PATREADPATHFROMSHP_H_
#include "patNetworkElements.h"
#include "patPathJ.h"
#include "patError.h"
#include <shapefil.h>
#include "patWay.h"
#include <vector>
class patReadPathFromShp {
public:
	patReadPathFromShp();
	void readIndex(DBFHandle& file_handler);
	TransportMode readTransportMode(patString mode_string);
	int detFirstEdgeForwardDirection(DBFHandle& file_handler);
	bool read(patPathJ* path, string file_path, patNetworkElements* network, patError*& err);
	void readSequence(DBFHandle& file_handler);
	virtual ~patReadPathFromShp();
protected:
	short int edge_id_index;
	short int path_id_index;
	short int source_id_index;
	short int target_id_index ;
	short int mode_index;
	short int sequence_id_index;
	map<int,int> sequence_list;

};

#endif /* PATREADPATHFROMSHP_H_ */
