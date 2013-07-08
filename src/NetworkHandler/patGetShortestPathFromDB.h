/*
 * patGetShortestPathFromDB.h
 *
 *  Created on: Aug 20, 2012
 *      Author: jchen
 */

#ifndef PATGETSHORTESTPATHFROMDB_H_
#define PATGETSHORTESTPATHFROMDB_H_

#include "patNode.h"
#include "patNetworkElements.h"
#include "patMultiModalPath.h"
 #include "patCoordinates.h"
class patGetShortestPathFromDB {
public:
	patGetShortestPathFromDB(patNetworkElements* network);
	patMultiModalPath getSPFromSqlString(const string& sql_str) ;
	patMultiModalPath getSPFromIDs(const int& start_node, const int& end_node);
	patMultiModalPath getSPFromGeoCoords(const patCoordinates& start_node, const patCoordinates& end_node, const patGeoBoundingBox& bb);
	virtual ~patGetShortestPathFromDB();
protected:
	patNetworkElements* m_network;
};

#endif /* PATGETSHORTESTPATHFROMDB_H_ */
