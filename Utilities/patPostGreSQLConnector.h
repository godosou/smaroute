/*
 * patPostGreSQLConnector.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATPOSTGRESQLCONNECTOR_H_
#define PATPOSTGRESQLCONNECTOR_H_

#include "patType.h"
#include "patGeoBoundingBox.h"
#include <pqxx/pqxx>
#include "patNetworkElements.h"
#include "patError.h"
using namespace pqxx;

class patPostGreSQLConnector {
public:
	patPostGreSQLConnector();
	 void readNetwork(patGeoBoundingBox bounding_box,patNetworkElements& theNetwork, patError*& err);
	 void readNodes(patGeoBoundingBox bounding_box,patNetworkElements& theNetwork, patError*& err);
	 void readWays(patGeoBoundingBox bounding_box,patNetworkElements& theNetwork, patError*& err);

	static result makeSelectQuery(patString query_string);

	virtual ~patPostGreSQLConnector();

};
 /* namespace pqxx */
#endif /* PATPOSTGRESQLCONNECTOR_H_ */

