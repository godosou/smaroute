/*
 * patPostGreSQLConnector.h
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#ifndef PATPOSTGRESQLCONNECTOR_H_
#define PATPOSTGRESQLCONNECTOR_H_

#include "patType.h"
#include <pqxx/pqxx>
#include "patError.h"
using namespace pqxx;

class patPostGreSQLConnector {
public:
	patPostGreSQLConnector();

	static result makeSelectQuery(patString query_string);

	virtual ~patPostGreSQLConnector();

};
 /* namespace pqxx */
#endif /* PATPOSTGRESQLCONNECTOR_H_ */

