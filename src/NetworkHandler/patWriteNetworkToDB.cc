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
patWriteNetworkToDB::patWriteNetworkToDB() {
	// TODO Auto-generated constructor stub

}

patWriteNetworkToDB::~patWriteNetworkToDB() {
	// TODO Auto-generated destructor stub
}

void patWriteNetworkToDB::write(const patNetworkBase* network,
		const std::string& table_name) {

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

	string conn_string = patNBParameters::the()->databaseHost;
	connection Conn(conn_string);
	string tname(table_name);

	work Xaction(Conn, tname);

	string null_string("");
//	tablewriter table_writer(Xaction, tname, columns.begin(), columns.end(),
//			null_string);

	string sql_str =
			"INSERT INTO " + tname
					+ " (gid, length, name,x1, y1, x2, y2, osm_id, the_geom, source, target) VALUES($1, $2, $3, $4, $5, $6, $7, $8,ST_GeomFromText($9,4326), $10, $11);";
	Conn.prepare("insert", sql_str);
	DEBUG_MESSAGE( "connected to "<<Conn.dbname()<<", table: "<<tname);

	const unordered_map<const patNode*, set<const patRoadBase*> >* incidents =
			network->getOutgoingIncidents();
	unsigned gid = 0;
	cout<<"incident nodes:"<<endl;

	unsigned counter=0;
	unsigned arc_counter = 0;
	for (unordered_map<const patNode*, set<const patRoadBase*> >::const_iterator node_iter =
			incidents->begin(); node_iter != incidents->end(); ++node_iter) {

		if(counter % 1000==0){
			cout<<counter<<","<<arc_counter<<endl;
		}
		++counter;
		for (set<const patRoadBase*>::const_iterator road_iter =
				node_iter->second.begin(); road_iter != node_iter->second.end();
				++road_iter) {
			subtransaction s(Xaction, "sub");
			try{
				++arc_counter;
			s.prepared("insert")(++gid)((*road_iter)->getLength())(
					(*road_iter)->getArcList().front()->getName())(
					(*road_iter)->getUpNode()->getLongitude())(
					(*road_iter)->getUpNode()->getLatitude())(
					(*road_iter)->getDownNode()->getLongitude())(
					(*road_iter)->getDownNode()->getLatitude())(
					(*road_iter)->getArcList().front()->getUserId())(
					"MULTILINESTRING((" + (*road_iter)->getGeomText() + "))")(
					(*road_iter)->getUpNode()->getUserId())(
					(*road_iter)->getDownNode()->getUserId()).exec();
			s.commit();}
			catch(pqxx::unique_violation){
				cout<<"unique violation"<<endl;
			}
//
//			vector < string > values;
//			{
//				stringstream ss;
//				ss << ++gid;
//				values.push_back(ss.str());
//			}
//			{
//				stringstream ss;
//				ss << (*road_iter)->getLength();
//				values.push_back(ss.str());
//			}
//			{
//				//name
//				stringstream ss;
//				ss << (*road_iter)->getArcList().front()->getName();
//				values.push_back(ss.str());
//			}
//			{
//				//x1
//				stringstream ss;
//				ss << (*road_iter)->getUpNode()->getLongitude();
//				values.push_back(ss.str());
//			}
//			{
//				//y1
//				stringstream ss;
//				ss << (*road_iter)->getUpNode()->getLatitude();
//				values.push_back(ss.str());
//			}
//			{
//				//x2
//				stringstream ss;
//				ss << (*road_iter)->getDownNode()->getLongitude();
//				values.push_back(ss.str());
//			}
//			{
//				//y2
//				stringstream ss;
//				ss << (*road_iter)->getDownNode()->getLatitude();
//				values.push_back(ss.str());
//			}
//			{
//				//osmid
//				stringstream ss;
//				ss << (*road_iter)->getArcList().front()->getUserId();
//				values.push_back(ss.str());
//			}
//			{
//				//the_geom
//				stringstream ss;
//				ss << (*road_iter)->getGeomText();
//				values.push_back(ss.str());
//			}
//			{
//				//source
//				stringstream ss;
//				ss << (*road_iter)->getUpNode()->getUserId();
//				values.push_back(ss.str());
//			}
//			{
//				//target
//				stringstream ss;
//				ss << (*road_iter)->getDownNode()->getUserId();
//				values.push_back(ss.str());
//			}
//			cout << columns.size() << "," << values.size() << endl;
//			for (unsigned v_i = 0; v_i < values.size(); ++v_i) {
//				cout << values[v_i] << endl;
//			}
//			stringstream sql_ss;
//			sql_ss << "INSERT INTO ";
//			sql_ss << tname;
//			sql_ss
//					<< " (gid, length, x1, y1, x2, y2, osm_id, the_geom, source, target) ";
//			sql_ss << "VALUES(";
//			sql_ss << ++gid;
//			sql_ss << (*road_iter)->getLength() << ",";
//			sql_ss << (*road_iter)->getUpNode()->getLongitude() << ",";
//			sql_ss << (*road_iter)->getUpNode()->getLatitude() << ",";
//			sql_ss << (*road_iter)->getDownNode()->getLongitude() << ",";
//			sql_ss << (*road_iter)->getDownNode()->getLatitude() << ",";
//			sql_ss << (*road_iter)->getArcList().front()->getUserId() << ",";
//			sql_ss << "ST_GeomFromText('MULTILINESTRING(("
//					<< (*road_iter)->getGeomText() << "))',4326)" << ",";
//			sql_ss << (*road_iter)->getUpNode()->getUserId() << ",";
//			sql_ss << (*road_iter)->getDownNode()->getUserId() << ");";
//
//			cout << sql_ss.str() << endl;
//			table_writer.insert(values.begin(), values.end());
		}
	}
	Xaction.commit();
//	table_writer.complete();
	Conn.disconnect();
}
