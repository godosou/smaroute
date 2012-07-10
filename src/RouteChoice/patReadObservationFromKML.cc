/*
 * File:   patReadPathFromKML.cc
 * Author: jchen
 *
 * Created on May 2, 2011, 11:29 AM
 */
#include <xmlwrapp/xmlwrapp.h>
#include <boost/xpressive/xpressive.hpp>
#include "patReadObservationFromKML.h"
#include "patMultiModalPath.h"
#include "patDisplay.h"
#include "patException.h"
using namespace boost::xpressive;

patReadObservationFromKML::patReadObservationFromKML(
		const patNetworkElements* a_network) :
		m_network(a_network) {

}

void patReadObservationFromKML::parseFile(string file_name,
		patObservation* observation) {
	xml::tree_parser parser(file_name.c_str());

	if (!parser) {
		WARNING("Error while parsing " << file_name);
		throw RuntimeException("no file is given");

	}
	//DEBUG_MESSAGE("file loaded");
    unsigned slash_position=file_name.rfind("/");
    unsigned file_name_length = file_name.size()-slash_position-5;
//    DEBUG_MESSAGE(file_name_length);
	observation->setId(file_name.substr(slash_position+1,file_name_length));
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
		for (xml::node::iterator folder_iter = doc_node->begin();
				folder_iter != doc_node->end(); ++folder_iter) {
			if (folder_iter->is_text()) {
				continue;
			}

			if (string(folder_iter->get_name()) == string("Folder")) {

				//DEBUG_MESSAGE(folder_iter->get_name());
				for (xml::node::iterator name_iter_1 = folder_iter->begin();
						name_iter_1 != folder_iter->end(); ++name_iter_1) {
					if (string(name_iter_1->get_content()) == string("paths")) {
						paths_folder = folder_iter;
						break;
					}
				}
			}
		}
		DEBUG_MESSAGE(paths_folder->get_name());
		xml::node::iterator od_iter = paths_folder->begin();
		for (; od_iter != paths_folder->end(); ++od_iter) {
			if (od_iter->is_text()) {
				continue;
			}
			//DEBUG_MESSAGE(od_iter->get_name());

			if (string(od_iter->get_name()) != string("Folder")) {
				continue;
			}

			for (xml::node::iterator path_iter = od_iter->begin();
					path_iter != od_iter->end(); ++path_iter) {
				if (path_iter->is_text()) {
					continue;
				}
				//DEBUG_MESSAGE(path_iter->get_name());
				if (string(path_iter->get_name()) != string("Folder")) {
					continue;
				}
//				DEBUG_MESSAGE("new path");
				patMultiModalPath new_path;
				bool zero_proba_path=false;
				double proba = 0.0;
				double normalized_proba = 0.0;
				for (xml::node::iterator placemark_iter = path_iter->begin();
						placemark_iter != path_iter->end(); ++placemark_iter) {
					if (placemark_iter->is_text()) {
						continue;
					}
					//DEBUG_MESSAGE(placemark_iter->get_name());
					if (string(placemark_iter->get_name())
							== string("description")) {
						string desc_str(placemark_iter->get_content());
						//DEBUG_MESSAGE(desc_str);
						if (desc_str == "") {
							WARNING("no path proba info for path");
							throw RuntimeException(
									"no path proba info for path");
						} else {
//							DEBUG_MESSAGE(desc_str);
							sregex rex = sregex::compile("ts: (\\d+\\.?\\d*?e?[\\-\\+]?\\d*?)\\((\\d+\\.?\\d*?e?[\\-\\+]?\\d*?)\\)");
							smatch what;
							if (regex_search(desc_str, what, rex)) {
								//DEBUG_MESSAGE(what[1].str());
								proba = atof(what[1].str().c_str());
								normalized_proba = atof(what[2].str().c_str());
//								DEBUG_MESSAGE(proba<<","<<normalized_proba);
								if (proba == 0.0) {
									zero_proba_path=true;
									DEBUG_MESSAGE("ZERO PROBA");
									break;
								}
							}
						}
					}
					if (string(placemark_iter->get_name())
							== string("Placemark")) {
						for (xml::node::iterator descrip_iter =
								placemark_iter->begin();
								descrip_iter != placemark_iter->end();
								++descrip_iter) {
							if (string(descrip_iter->get_name())
									== string("description")) {
								string desc_str(descrip_iter->get_content());
								//DEBUG_MESSAGE(desc_str);
								if (desc_str == "") {
									WARNING(
											"no description specified in the link");
									throw RuntimeException(
											"no description specified in the link");
								} else {
									sregex rex =
											sregex::compile(
													"Arc (\\d+) from node (\\d+) to node (\\d+)");
									smatch what;

									if (regex_match(desc_str, what, rex)) {

//										unsigned long link_id(
//												atoi(what[1].str().c_str()));
										unsigned long up_node_id(
												atoi(what[2].str().c_str()));
										unsigned long down_node_id(
												atoi(what[3].str().c_str()));
//										DEBUG_MESSAGE(up_node_id<<"-"<<down_node_id);

										const patNode* up_node =
												m_network->getNode(up_node_id);
										const patNode* down_node =
												m_network->getNode(
														down_node_id);
										if (up_node == NULL) {
											WARNING(
													"node not found: " << up_node_id );
											throw RuntimeException(
													"node not found");
										}
										if (down_node == NULL) {
											WARNING(
													"node not found: " << down_node_id );
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
													new_arc) == false) {

												throw RuntimeException(
														"not valid path");
											}
										}
									}
									else{
										throw RuntimeException("not valid link");
									}

								}
							}

						}
					}
				}
				if(zero_proba_path){
					continue;
				}

//				DEBUG_MESSAGE("finish a path");
				new_path.computeLength();
//				DEBUG_MESSAGE("new path read" << new_path);
				observation->addPath(new_path, proba);
//				DEBUG_MESSAGE("done");
			}

		}

	}

}

patReadObservationFromKML::~patReadObservationFromKML() {
}
