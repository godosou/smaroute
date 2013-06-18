/*
 * patReadChoiceSetFromKML.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patReadChoiceSetFromKML.h"
#include "kml/dom.h"
#include "kml/base/file.h"
#include "patException.h"
#include "patLogProbaToProba.h"
#include "patSampleDiscreteDistribution.h"
#include <xmlwrapp/xmlwrapp.h>
#include <boost/xpressive/xpressive.hpp>
#include "patRandomNumber.h"
#include <map>
#include "patChoiceSet.h"
using namespace std;
using kmldom::FeaturePtr;
using kmldom::KmlPtr;
using kmldom::ElementPtr;
using kmldom::FolderPtr;

using namespace boost::xpressive;
patReadChoiceSetFromKML::patReadChoiceSetFromKML(
		const patNetworkElements* a_network) :
		m_network(a_network) {
}

patReadChoiceSetFromKML::~patReadChoiceSetFromKML() {
	// TODO Auto-generated destructor stub
}

map<patOd, patChoiceSet> patReadChoiceSetFromKML::read(string file_name, const patRandomNumber& rnd) {
	map<patOd, patChoiceSet> ods;
	xml::tree_parser parser(file_name.c_str());

	if (!parser) {
		WARNING("Error while parsing " << file_name);
		throw RuntimeException("no file is given");

	}
	//DEBUG_MESSAGE("file loaded");
	xml::document &doc = parser.get_document();

	xml::node root_node = doc.get_root_node();
//	xml::node::node_type the_type = root_node.get_type();
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

			if (string(od_iter->get_name()) == string("Folder")) {
				patOd od;
				vector<patMultiModalPath> paths;
				vector<double> log_weights;
				patChoiceSet od_choice_set;
				unsigned nbr_paths = 0;
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
							if (origin_node == NULL || dest_node == NULL) {
								throw RuntimeException("NULL NODE POINTER");
							}
//							DEBUG_MESSAGE(origin_id<<"-"<<dest_id);
							od = patOd(origin_node, dest_node);
							od_choice_set.setOD(origin_node, dest_node);

						} else {
							WARNING("not od given");
							throw RuntimeException("not od given");
						}

					} else if (string(path_iter->get_name())
							== string("Folder")) {

						patMultiModalPath new_path;
						int count;
						double log_weight;
						for (xml::node::iterator road_iter = path_iter->begin();
								road_iter != path_iter->end(); ++road_iter) {

							if (road_iter->is_text()) {
								continue;
							} else if (string(road_iter->get_name())
									== string("description")) {
								string path_desc = road_iter->get_content();
								sregex rex = sregex::compile("count: (\\d+)");
								smatch what;

								if (regex_search(path_desc, what, rex)) {

									count = atoi(what[1].str().c_str());
//									DEBUG_MESSAGE(count);
								} else {
									throw RuntimeException(
											"not path count given");
								}
								sregex rex_log_weight = sregex::compile(
										"logweight: ([\\-\\+]?\\d+(\\.\\d+)?)");
								smatch what_log_weight;

								if (regex_search(path_desc, what_log_weight,
										rex_log_weight)) {
//									DEBUG_MESSAGE(what_log_weight[1]);
									log_weight = atof(
											what_log_weight[1].str().c_str());
//									DEBUG_MESSAGE(log_weight);
								} else {
									throw RuntimeException(
											"not path log weight given");
								}
//								DEBUG_MESSAGE(count<<","<<log_weight);
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
															if (up_node
																	== NULL) {
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
															if (new_arc
																	== NULL) {
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
						new_path.generateOd();
//						DEBUG_MESSAGE(new_path.computeLength());
//						DEBUG_MESSAGE(new_path. generateOd());
//						DEBUG_MESSAGE(count<<","<<log_weight);
//						if (count == 0) {
//							count = 1;
//						}
//						for (unsigned count_iter = 0; count_iter < count;
//								++count_iter) {
//							paths.push_back(new_path);
//							log_weights.push_back(log_weight);
//						}
//						nbr_paths += count;
						od_choice_set.addPath(new_path, log_weight, count);
//						++nbr_paths;
//						if (nbr_paths
//								>= patNBParameters::the()->choiceSetInBiogemeData) {
//							continue;
//						}
					}
				}
//				DEBUG_MESSAGE(od);
//				DEBUG_MESSAGE(paths.size());
//				patChoiceSet od_chocie_set;
//				od_chocie_set.setOd(od);
////				DEBUG_MESSAGE(od.getOrigin()->getUserId()<<","<<od.getDestination()->getUserId());
//				if (paths.size() > sampled_count) {
////					DEBUG_MESSAGE("random sample the choice set: "<<sampled_count<<"<"<<paths.size() );
//					vector<double> probas(paths.size(), 1.0);
////					DEBUG_MESSAGE(probas.size()<<","<<log_weights.size());
//					vector<unsigned int> sampled_path_indics =
//							patSampleDiscreteDistribution::sampleWithOutReplaceMent(
//									probas, sampled_count, rnd);
//
////					DEBUG_MESSAGE(sampled_path_indics.size());
//					for (vector<unsigned int>::iterator sample_iter =
//							sampled_path_indics.begin();
//							sample_iter != sampled_path_indics.end();
//							++sample_iter) {
//
//						od_choice_set.addPath(paths[*sample_iter],
//								log_weights[*sample_iter], 1);
//					}
////					DEBUG_MESSAGE("OK");
//				} else {
//					od_chocie_set.setOd(od);
//					for (unsigned int sample_iter = 0;
//							sample_iter < paths.size(); ++sample_iter) {
////						DEBUG_MESSAGE(sample_iter);
//						od_choice_set.addPath(paths[sample_iter],
//								log_weights[sample_iter], 1);
//					}
//				}
				ods[od] = od_choice_set;
			}
		}

	}
	return ods;
}
