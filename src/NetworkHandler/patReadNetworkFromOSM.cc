/*
 * patReadNetworkFromOSM.cc
 *
 *  Created on: May 3, 2012
 *      Author: jchen
 */

#include "patReadNetworkFromOSM.h"

#include <xmlwrapp/xmlwrapp.h>
#include <sstream>
#include "patException.h"
#include "patError.h"
patReadNetworkFromOSM::patReadNetworkFromOSM() {
	// TODO Auto-generated constructor stub

}

patReadNetworkFromOSM::~patReadNetworkFromOSM() {
	// TODO Auto-generated destructor stub
}

 void patReadNetworkFromOSM::read(string file_name,
		patNetworkElements& network_element, patGeoBoundingBox& bb,	patError*& err
) {
	 DEBUG_MESSAGE("Read "<<file_name);
	xml::init xmlinit;

	xml::tree_parser parser(file_name.c_str());
	xml::document &doc = parser.get_document();
	if (!parser) {
		stringstream str;
		str << "Error while parsing " << file_name;
		WARNING(str.str());
		throw RuntimeException(str.str().c_str());
	}

	xml::node theRoot = doc.get_root_node();

//	DEBUG_MESSAGE("Node: " << theRoot.get_name());
//	xml::node::node_type theType = theRoot.get_type();
//	DEBUG_MESSAGE("Node type: " << theType);

	xml::node::iterator rootIter(theRoot.begin());
	xml::node::iterator rootEnd(theRoot.end());

	map<string, short> theNodeNames;
	map<xml::node::node_type, short> theNodeTypes;

	for (; rootIter != rootEnd; ++rootIter) {
		string nodeName(rootIter->get_name());
		theNodeNames[nodeName]++;
		xml::node::node_type theType = rootIter->get_type();
		theNodeTypes[theType]++;

		if (nodeName == "bounds") {
//			double minlat, maxlat, minlon, maxlon;

			xml::attributes attr = rootIter->get_attributes();
			xml::attributes::iterator i;
//the map bound not usefule
			//			for (i = attr.begin(); i != attr.end(); ++i) {
//				string attrName(i->get_name());
//				if (attrName == "minlat") {
//					minlat = atof(i->get_value());
//					minlat *= networkScaler;
//				} else if (attrName == "maxlat") {
//					maxlat = atof(i->get_value());
//					maxlat *= networkScaler;
//				} else if (attrName == "minlon") {
//					minlon = atof(i->get_value());
//					maxlat *= networkScaler;
//				} else if (attrName == "maxlon") {
//					maxlon = atof(i->get_value());
//					maxlon *= networkScaler;
//				} else {
//					stringstream str;
//					str << "Unknown attribute of node '" << nodeName
//							<< "' in file " << file_name << ": '" << attrName
//							<< "'";
//					err = new patErrMiscError(str.str());
//					WARNING(err->describe());
//					return false;
//				}
//			}
//			theNetwork.setMapBounds(minlat, maxlat, minlon, maxlon);
		} else if (nodeName == "node") {
			unsigned long id;
			double lat;
			double lon;
			string name;
			xml::attributes attr = rootIter->get_attributes();
			xml::attributes::iterator i;

			bool correct_node = true;
			for (i = attr.begin(); i != attr.end(); ++i) {
				string attrName(i->get_name());
				if (attrName == "id") {
					id = atoi(i->get_value());
					stringstream str;
					str.precision(15);
					str << "Node " << id;
					name = string(str.str());
				} else if (attrName == "lat") {
					lat = atof(i->get_value());
					if (lat > bb.getMaxLat() || lat < bb.getMinLat()) {
						correct_node = false;
						break;
					}
				}

				else if (attrName == "lon") {
					lon = atof(i->get_value());
					if (lon > bb.getMaxLon() || lon < bb.getMinLon()) {
						correct_node = false;
						break;
					}
				}

				else {
					//	  WARNING("Ignore attribute: " << attrName) ;
				}

			}
			if (!correct_node) {
				continue;
			}
			unordered_map < string, string > node_tags;
			xml::node::iterator iter;
			for (iter = rootIter->begin(); iter != rootIter->end(); ++iter) {
				if (string(iter->get_name()) == "tag") {
					xml::attributes attr = iter->get_attributes();
					xml::attributes::iterator i;
					for (i = attr.begin(); i != attr.end(); ++i) {
						string k, v;
						k = string(i->get_value());
						++i;
						v = string(i->get_value());
						node_tags[k] = v;
						if (k == "name") {
							name = v;
						}
					}
				}
			}
			patNode new_node(id, lat, lon);
			new_node.setTags(node_tags);
			new_node.setName(name);
			network_element.addNode(id, new_node, err);
//			if (err != NULL) {
//				stringstream str;
//				str << "Node " << new_node << " not added to the network";
//				WARING(str.str());
//				throw RuntimeException(str.str());
//			}
		}
//		else if (nodeName == "od") {
//			unsigned long orig;
//			unsigned long dest;
//			xml::attributes attr = rootIter->get_attributes();
//			xml::attributes::iterator i;
//			for (i = attr.begin(); i != attr.end(); ++i) {
//				string attrName(i->get_name());
//				if (attrName == "orig") {
//					orig = atoi(i->get_value());
//				}
//				if (attrName == "dest") {
//					dest = atoi(i->get_value());
//				}
//			}
//			theNetwork.addOd(orig, dest);
//		} else if (nodeName == "relation") {
//		} else if (nodeName == "text") {
//
//		}
		else if (nodeName == "way") {
			unsigned long id;
//			unsigned long upNodeId;
//			unsigned long downNodeId;
			string theName;
			xml::attributes attr = rootIter->get_attributes();
			xml::attributes::iterator i;
			for (i = attr.begin(); i != attr.end(); ++i) {
				string attrName(i->get_name());
				if (attrName == "id") {
					id = atoi(i->get_value());
					stringstream str;
					str << "Link " << id;
					theName = string(str.str());
					break;
				}
			}
			struct arc_attributes theAttr;
			list<unsigned long> nodes;
			unordered_map < string, string > tags;
			xml::node::iterator linkIter;
			for (linkIter = rootIter->begin(); linkIter != rootIter->end();
					++linkIter) {
				string tagName(linkIter->get_name());
				if (tagName == "nd") {
					unsigned long nodeid = atoi(
							linkIter->get_attributes().begin()->get_value());
					nodes.push_back(nodeid);
				} else if (tagName == "tag") {
					xml::attributes attr = linkIter->get_attributes();
					xml::attributes::iterator i;
					for (i = attr.begin(); i != attr.end(); ++i) {
						string k, v;
						k = string(i->get_value());
						++i;
						v = string(i->get_value());
						tags[k] = v;
					}
				}
			}
			if (!nodes.empty()) {
				patWay new_way = patWay(id, tags);
				network_element.addWay(&new_way, nodes, err);
			}
		} else {

		}

	}

//	DEBUG_MESSAGE("Node names");
//	for (map<string, short>::iterator i = theNodeNames.begin();
//			i != theNodeNames.end(); ++i) {
//		DEBUG_MESSAGE(i->first << " [" << i->second << "]");
//	}
////	DEBUG_MESSAGE("Node types");
//	for (map<xml::node::node_type, short>::iterator i = theNodeTypes.begin();
//			i != theNodeTypes.end(); ++i) {
//		DEBUG_MESSAGE(i->first << " [" << i->second << "]");
//	}

}
