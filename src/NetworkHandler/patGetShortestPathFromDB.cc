/*
 * patGetShortestPathFromDB.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: jchen
 */

#include "patGetShortestPathFromDB.h"
#include "patPostGreSQLConnector.h"
#include "patPostGISDataType.h"
#include <list>
#include <vector>
using namespace std;

patGetShortestPathFromDB::patGetShortestPathFromDB(patNetworkElements* network) :
		m_network(network) {

}
patMultiModalPath patGetShortestPathFromDB::getSPFromSqlString(
		const string& sql_str) {

	result R = patPostGreSQLConnector::makeSelectQuery(sql_str);
	if (R.size() == 0) {
		return patMultiModalPath();
	}
	cout << "SP ways:" << R.size() << endl;

	list<const patArc*> arc_list;
	const patNode* up_node = NULL;
	const patNode* down_node = NULL;
	unsigned way_counter;
	for (result::const_iterator i = R.begin(); i != R.end(); ++i) {

		vector < pair<double, double> > lon_lat =
				patPostGISDataType::MultiLineToString((*i)["geom"].c_str());
//		unsigned long osm_id, source, target;
//		unsigned long id;
//		(*i)["osm_id"].to(osm_id);
//		(*i)["source"].to(source);
//		(*i)["target"].to(target);

		// cout<<"way: "<<++way_counter<<"osm"<<osm_id<<":"<<source<<"-"<<target<<endl;

		for (unsigned n_iter = 0; n_iter < lon_lat.size(); ++n_iter) {
			if (up_node == NULL) {
				patNode new_node(m_network->getNodeSize(),
						lon_lat[n_iter].second, lon_lat[n_iter].first);
				up_node = m_network->addNode(m_network->getNodeSize(),
						new_node);
				if (up_node == NULL) {
					throw RuntimeException(
							"patGetShortestPathFromDB: insert up node wrong");
				}
				// cout<<new_node<<endl;
			} else {
				patNode new_node(m_network->getNodeSize(),
						lon_lat[n_iter].second, lon_lat[n_iter].first);
				down_node = m_network->addNode(m_network->getNodeSize(),
						new_node);
				if (down_node == NULL) {
					throw RuntimeException(
							"patGetShortestPathFromDB: insert down node wrong");
				}
				if (up_node != NULL && down_node != NULL) {
					patArc* new_arc = m_network->addArc(up_node, down_node,
							NULL);
					if (new_arc == NULL) {
						throw RuntimeException(
								"patGetShortestPathFromDB: NULL arc");
					}
//					 cout<<new_node<<endl;
//					 cout<<new_arc<<endl;
					arc_list.push_back(new_arc);
				}

				up_node = down_node;
			}

		}

	}
	cout<<arc_list.size()<<endl;
	return patMultiModalPath(arc_list);
}

patMultiModalPath patGetShortestPathFromDB::getSPFromIDs(const int& start_node,
		const int& end_node) {
	stringstream query_stream;

	string table_name = "edge";
	query_stream << "SELECT  ST_AsText(" << table_name << ".the_geom) as geom, "
			<< table_name << ".osm_id as osm_id, " << table_name
			<< ".source as source, " << table_name
			<< ".target  as target FROM shortest_path_astar('SELECT gid AS id, source::int4,"
			<< "target::int4, length::double precision AS cost, x1, y1, x2, y2 "
			<< "FROM " << table_name << "'," << start_node << "," << end_node
			<< ", true, false)  as sp," << table_name << " where sp.edge_id="
			<< table_name << ".gid;";

	cout << query_stream.str() << endl;
	return getSPFromSqlString(query_stream.str());

}
patMultiModalPath patGetShortestPathFromDB::getSPFromGeoCoords(
		const patCoordinates& start_node, const patCoordinates& end_node,
		const patGeoBoundingBox& bb) {

	stringstream query_stream;
/*
	query_stream << "SELECT  ST_AsText(" << table_name << ".the_geom) as geom, "
			<< table_name << ".osm_id as osm_id, " << table_name
			<< ".source as source, " << table_name
			<< ".target  as target FROM shortest_path_astar('SELECT gid AS id, source::int4,"
			<< "target::int4, length::double precision AS cost, x1, y1, x2, y2 "
			<< "FROM " << table_name << " "
			<< "x1>="<<bb.getMinLon()<<" AND x1 <="<<bb.getMaxLon()
			<< "AND y1>="<<bb.getMinLat()<<" AND y1 <="<<bb.getMaxLat()
			<<"', getNearestNodeEdgeTable("
			<< start_node.getGeomText() << ") , getNearestNodeEdgeTable("
			<< end_node.getGeomText() << "), true, false)  as sp," << table_name
			<< " where sp.edge_id=" << table_name << ".gid;";
*/
	string table_name = "pg_ways";
	double node_precision=0.01;
	double bb_bound=0.1;
	query_stream<<"SELECT gid, AsText(the_geom) AS geom "
			<<"FROM dijkstra_sp_delta('"<<table_name<<"',"
	        <<"getNearestNode("<< start_node.getGeomText()
	        <<","<<node_precision<<",'"<<table_name<<"'),"
	        <<"getNearestNode("<< end_node.getGeomText()
	        <<","<<node_precision<<",'"<<table_name<<"'),"
	        <<bb_bound<<");";
	cout << query_stream.str() << endl;
	return getSPFromSqlString(query_stream.str());
}

patGetShortestPathFromDB::~patGetShortestPathFromDB() {
	// TODO Auto-generated destructor stub
}

