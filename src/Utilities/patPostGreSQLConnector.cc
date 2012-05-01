/*
 * patPostGreSQLConnector.cc
 *
 *  Created on: Jul 20, 2011
 *      Author: jchen
 */

#include "patPostGreSQLConnector.h"
#include "patType.h"
#include "patDisplay.h"
#include <sstream>
#include "patError.h"
#include "patNBParameters.h"
using namespace pqxx;

patPostGreSQLConnector::patPostGreSQLConnector() {
	patString host;
	patString port;
	patString db_user;
	patString db_passwd;
	patString db_name;
	//


}



result patPostGreSQLConnector::makeSelectQuery(patString query_string){
	patString conn_string = "host="+patNBParameters::the()->databaseHost+" port=5433 dbname=osm user=jchen password=xXxXxXxXxXx";
	try{
		connection Conn(conn_string);
		DEBUG_MESSAGE("connected to "<<Conn.dbname()<<endl);
		work Xaction(Conn, "OSM_PG_CONNECTOR");
		  std::ostringstream oss;
		result R=Xaction.exec(query_string);
		Xaction.commit();
		return R;

	}
	catch(const exception &e){
		WARNING(e.what());
	}

}
patPostGreSQLConnector::~patPostGreSQLConnector() {

}
/* namespace pqxx */
