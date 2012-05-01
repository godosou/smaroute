/*
 * patReadPathFromShp.h
 *
 *  Created on: Aug 15, 2011
 *      Author: jchen
 */

#ifndef PATREADPATHFROMSHP_H_
#define PATREADPATHFROMSHP_H_
#include "patNetworkElements.h"
#include "patMultiModalPath.h"
#include "patError.h"
#include <shapefil.h>
#include "patWay.h"
#include <vector>
class patReadPathFromShp {
public:
	patReadPathFromShp();

	/**
	 * Read index of each column:
	 * edge_id, path_id, source, target, mode, sequence
	 */
	void readIndex(DBFHandle& file_handler);

	/**
	 * Read TransportMode from string.
	 * @param mode_string
	 * @return TransportMode
	 */
	TransportMode readTransportMode(patString mode_string);

	/**
	 * Whether the first edget is forward of not.
	 * @param file_hander the DBF file hander.
	 * @return 1: forward; -1: backward; 0, error.
	 */
	int detFirstEdgeForwardDirection(DBFHandle& file_handler);

	/**
	 * Read the path given the DBF file, the network elements.
	 * @param path The path to be stored.
	 * @param file_path The path file
	 * @param network NetworkElement object.
	 * @param err Error pointer.
	 */
	bool read(patMultiModalPath* path, string file_path, patNetworkElements* network, patError*& err);

	/**
	 * Read sequence of arcs in the path.
	 * The sequence is stored in protected variable: sequence_list.
	 */
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
