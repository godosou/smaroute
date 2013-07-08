/*
 * patReadPathsFromKML.cc
 *
 *  Created on: May 26, 2012
 *      Author: jchen
 */

#include "patReadPathsFromKML.h"

#include "kml/dom.h"
#include "kml/base/file.h"
#include "patException.h"
#include <xmlwrapp/xmlwrapp.h>
#include <boost/xpressive/xpressive.hpp>

#include <map>
#include "patChoiceSet.h"
using namespace std;
using kmldom::FeaturePtr;
using kmldom::KmlPtr;
using kmldom::ElementPtr;
using kmldom::FolderPtr;

using namespace boost::xpressive;
patReadPathsFromKML::patReadPathsFromKML() {
	// TODO Auto-generated constructor stub

}

patReadPathsFromKML::~patReadPathsFromKML() {
	// TODO Auto-generated destructor stub
}

map<patMultiModalPath, double> patReadPathsFromKML::read(
		const patNetworkElements* m_network, string file_name) {
	map<patMultiModalPath, double> paths;
	xml::tree_parser parser(file_name.c_str());

	if (!parser) {
		WARNING("Error while parsing " << file_name);
		throw RuntimeException("no file is given");

	}
	//DEBUG_MESSAGE("file loaded");
	xml::document &doc = parser.get_document();

	xml::node root_node = doc.get_root_node();
	xml::node::node_type the_type = root_node.get_type();
	xml::node::iterator root_iter(root_node.begin());
	xml::node::iterator root_end(root_node.end());

	for (; root_iter != root_end; ++root_iter) {
		if (string(root_iter->get_name()) != string("Document")) {
			continue;
		}
		xml::node::iterator doc_node = root_iter;
		//DEBUG_MESSAGE(doc_node->get_name());
		xml::node::iterator paths_folder;
		for (xml::node::iterator od_iter = doc_node->begin();
				od_iter != doc_node->end(); ++od_iter) {
			if (od_iter->is_text()) {
				continue;
			}
//			bool is_path=true;
			if (string(od_iter->get_name()) == string("Folder")) {
				bool is_od_folder = false;
				for (xml::node::iterator name_iter_od = od_iter->begin();
						name_iter_od != od_iter->end(); ++name_iter_od) {
					if (string(name_iter_od->get_name()) == string("name")) {
						string folder_name(name_iter_od->get_content());
						//					cout << folder_name << endl;
						if (folder_name.find(string("DDR")) == string::npos
								&& folder_name.find(string("GPS"))
										== string::npos) {
							is_od_folder = true;
							//						cout << "is od folder" << endl;
							break;
						}
					}
				}

				if (!is_od_folder) {
					continue;
				}
				patOd od;
				patChoiceSet od_choice_set;
				for (xml::node::iterator path_iter = od_iter->begin();
						path_iter != od_iter->end(); ++path_iter) {
					if (path_iter->is_text()) {
						continue;
					}

					if (string(path_iter->get_name()) == string("name")) {
						string od_name = path_iter->get_content();
						sregex rex = sregex::compile("(\\d+)-(\\d+)");
						smatch what;

						if (regex_search(od_name, what, rex)) {

							int origin_id = atoi(what[1].str().c_str());
							int dest_id = atoi(what[2].str().c_str());
							const patNode* origin_node = m_network->getNode(
									origin_id);
							const patNode* dest_node = m_network->getNode(
									dest_id);

							DEBUG_MESSAGE(origin_id<<"-"<<dest_id);

							if (origin_node == NULL) {
								throw RuntimeException("Origin NODE POINTER");
							}
							if (dest_node == NULL) {

								throw RuntimeException("DEST NODE POINTER");
							}
//							DEBUG_MESSAGE(origin_id<<"-"<<dest_id);
							od = patOd(origin_node, dest_node);
							od_choice_set.setOD(origin_node, dest_node);

						} else {
//							is_path=false;
							break;
//							throw RuntimeException(
//									"patReadPathsFromKML: no od given");
						}

					} else if (string(path_iter->get_name())
							== string("Folder")) {

						patMultiModalPath new_path;
						int count;
						double path_proba;
						for (xml::node::iterator road_iter = path_iter->begin();
								road_iter != path_iter->end(); ++road_iter) {

							if (road_iter->is_text()) {
								continue;
							} else if (string(road_iter->get_name())
									== string("description")) {
								string path_desc = road_iter->get_content();
								sregex rex = sregex::compile("count: (\\d+)");
								smatch what;
//
//								if (regex_search(path_desc, what, rex)) {
//
//									count = atoi(what[1].str().c_str());
////									DEBUG_MESSAGE(count);
//								} else {
//									throw RuntimeException(
//											"not path count given");
//								}
								sregex rex_log_weight =
										sregex::compile(
												"proba: ([\\-\\+]?\\d+\\.\\d+e[\\-\\+]?\\d+)");
								smatch what_log_weight;

								if (regex_search(path_desc, what_log_weight,
										rex_log_weight)) {

									path_proba = atof(
											what_log_weight[1].str().c_str());
//									DEBUG_MESSAGE(count);
								} else {
									sregex rex_log_weight_nomral =
											sregex::compile(
													"proba: ([\\-\\+]?\\d+\\.\\d+)");
									smatch what_log_weight;

									if (regex_search(path_desc, what_log_weight,
											rex_log_weight_nomral)) {

										path_proba =
												atof(
														what_log_weight[1].str().c_str());
										//									DEBUG_MESSAGE(count);
									}

								}
//								cout <<path_proba<<endl;
							} else if (string(road_iter->get_name())
									== string("Folder")) {
								for (xml::node::iterator arc_iter =
										road_iter->begin();
										arc_iter != road_iter->end();
										++arc_iter) {

									if (arc_iter->is_text()) {
										continue;
									} else if (string(arc_iter->get_name())
											== string("Folder")) {
										for (xml::node::iterator placemark_iter =
												arc_iter->begin();
												placemark_iter
														!= arc_iter->end();
												++placemark_iter) {
											if (placemark_iter->is_text()) {
												continue;
											} else if (string(
													placemark_iter->get_name())
													== string("Placemark")) {

												for (xml::node::iterator desc_iter =
														placemark_iter->begin();
														desc_iter
																!= placemark_iter->end();
														++desc_iter) {

													if (desc_iter->is_text()) {
														continue;
													} else if (string(
															desc_iter->get_name())
															== string(
																	"description")) {
														string arc_desc =
																desc_iter->get_content();
														sregex rex =
																sregex::compile(
																		"(\\d+)->(\\d+)");
														smatch what;

														if (regex_search(
																arc_desc, what,
																rex)) {

															unsigned long up_node_id(
																	atoi(
																			what[1].str().c_str()));
															unsigned long down_node_id(
																	atoi(
																			what[2].str().c_str()));
															//DEBUG_MESSAGE(link_id<<":"<<up_node_id<<"-"<<down_node_id);

															const patNode* up_node =
																	m_network->getNode(
																			up_node_id);
															const patNode* down_node =
																	m_network->getNode(
																			down_node_id);
															if (up_node == NULL) {
																WARNING(
																		"node not found: " << up_node_id);
																throw RuntimeException(
																		"node not found");
															}
															if (down_node
																	== NULL) {
																WARNING(
																		"node not found: " << down_node_id);
																throw RuntimeException(
																		"node not found");
															}
															const patArc* new_arc =
																	m_network->findArcByNodes(
																			up_node,
																			down_node);
															if (new_arc == NULL) {
																WARNING(
																		"arc not found: " << up_node_id << "-" << down_node_id);
																throw RuntimeException(
																		"arc not found");
															} else {

																if (new_path.addRoadTravelToBack(
																		new_arc)
																		== false) {

																	throw RuntimeException(
																			"not valid path");
																}
															}
														} else {
															throw RuntimeException(
																	"arc description not found");
														}

													}
												}
											}
										}

									}
								}
							}
						}
						new_path.computeLength();
						paths[new_path] = path_proba;
					}
				}
			}
		}

	}
	return paths;
}
