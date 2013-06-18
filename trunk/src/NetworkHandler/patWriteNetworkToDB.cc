/*
 * patWriteNetworkToDB.cc
 *
 *  Created on: Aug 19, 2012
 *      Author: jchen
 */

#include "patWriteNetworkToDB.h"
#include <vector>
#include <tr1/unordered_map>
#include <set>
#include "patNode.h"
#include "patRoadBase.h"
#include <pqxx/tablewriter>
#include <pqxx/pqxx>
#include "patNBParameters.h"
using namespace std;
using namespace pqxx;
#include <libconfig.h++>

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace libconfig;
#include "patPostGreSqlDataType.h"
patWriteNetworkToDB::patWriteNetworkToDB() {
	// TODO Auto-generated constructor stub

}

patWriteNetworkToDB::~patWriteNetworkToDB() {
	// TODO Auto-generated destructor stub
}

void patWriteNetworkToDB::write(const patNetworkBase* network,
		const std::string& table_name, const string & conn_string) {

	vector < string > columns;
	columns.push_back("gid");
	columns.push_back("length");
	columns.push_back("name");
	columns.push_back("x1");
	columns.push_back("y1");
	columns.push_back("x2");
	columns.push_back("y2");
	columns.push_back("osm_id");
	columns.push_back("the_geom");
	columns.push_back("source");
	columns.push_back("target");
	connection Conn(conn_string);
	string tname(table_name);

	work Xaction(Conn, tname);

	string null_string("");
//	tablewriter table_writer(Xaction, tname, columns.begin(), columns.end(),
//			null_string);

	string sql_str =
			"INSERT INTO " + tname
					+ " (gid, length, name,x1, y1, x2, y2, osm_id, the_geom, source, target,opposit_dir_id,prev_streets,next_streets,traffic_light) "
							"VALUES($1, $2, $3, $4, $5, $6, $7, $8,ST_GeomFromText($9,4326), $10, $11,$12,$13,$14,$15);";
	Conn.prepare("insert", sql_str);
	DEBUG_MESSAGE( "connected to "<<Conn.dbname()<<", table: "<<tname);

	const unordered_map<const patRoadBase*, const int>* all_roads =
			network->getAllRoads();

	unsigned gid = 0;
	cout << "incident nodes:" << endl;

	unsigned counter = 0;
	unsigned arc_counter = 0;

	for (unordered_map<const patRoadBase*, const int>::const_iterator road_iter =
			all_roads->begin(); road_iter != all_roads->end(); ++road_iter) {

		if (counter % 1000 == 0) {
			cout << counter << "," << arc_counter << endl;
		}
		++counter;
		subtransaction s(Xaction, "sub");
//		try {
			++arc_counter;

			int opposite_road_id;
			const patRoadBase* oppporsite_road = network->findOpposite(
					road_iter->first);
			if (oppporsite_road != NULL) {
				opposite_road_id = network->getRoadId(oppporsite_road);
			}
			const list<int> prev_roads = network->findPrevRoadIds(
					(road_iter->first)->getUpNode());
			const list<int> next_roads = network->findNextRoadIds(
					(road_iter->first)->getDownNode());
			s.prepared("insert")(road_iter->second)(
					(road_iter->first)->getLength())(
					(road_iter->first)->getArcList().front()->getName())(
					(road_iter->first)->getUpNode()->getLongitude())(
					(road_iter->first)->getUpNode()->getLatitude())(
					(road_iter->first)->getDownNode()->getLongitude())(
					(road_iter->first)->getDownNode()->getLatitude())(
					(road_iter->first)->getArcList().front()->getUserId())(
					"MULTILINESTRING((" + (road_iter->first)->getGeomText() + "))")(
					(road_iter->first)->getUpNode()->getUserId())(
					(road_iter->first)->getDownNode()->getUserId())(opposite_road_id)(
					patPostGreSqlDataType::ListIntToArray(prev_roads))(
					patPostGreSqlDataType::ListIntToArray(next_roads))(
					(road_iter->first)->getDownNode()->hasTrafficSignal()).exec();
			s.commit();
//		} catch (pqxx::unique_violation) {
//			cout << "unique violation" << endl;
//		}

	}

	Xaction.commit();
//	table_writer.complete();
	Conn.disconnect();
}
