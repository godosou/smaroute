/* 
 * File:   patReadPathFromKML.cc
 * Author: jchen
 * 
 * Created on May 2, 2011, 11:29 AM
 */

#include "patReadPathFromKML.h"
#include <set>
#include <xmlwrapp/xmlwrapp.h>
#include "patDisplay.h"
#include "patErrMiscError.h"
#include "patErrNullPointer.h"
#include "patDisplay.h"
#include "patPathJ.h"
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

patReadObservationFromKML::patReadObservationFromKML(patString a_file_name, patNetwork* a_network) :
file_name(a_file_name),
network(a_network) {

}

bool patReadObservationFromKML::parseFile(patError*& err) {
    xml::tree_parser parser(file_name.c_str());

    if (!parser) {
        stringstream str;
        str << "Error while parsing " << file_name;
        err = new patErrMiscError(str.str());
        WARNING(err->describe());
        return false;
    }
    DEBUG_MESSAGE("file loaded")
    xml::document &doc = parser.get_document();

    xml::node root_node = doc.get_root_node();
    xml::node::node_type the_type = root_node.get_type();
    xml::node::iterator root_iter(root_node.begin());
    xml::node::iterator root_end(root_node.end());

    for (; root_iter != root_end; ++root_iter) {
        if (patString(root_iter->get_name()) != patString("Document")) {
            continue;
        }
        xml::node::iterator doc_node = root_iter;
        //DEBUG_MESSAGE(doc_node->get_name());
        xml::node::iterator paths_folder;
        for (xml::node::iterator folder_iter = doc_node->begin();
                folder_iter != doc_node->end();
                ++folder_iter) {
            if (folder_iter->is_text()) {
                continue;
            }

            if (patString(folder_iter->get_name()) == patString("Folder")) {

                //DEBUG_MESSAGE(folder_iter->get_name());
                for (xml::node::iterator name_iter_1 = folder_iter->begin();
                        name_iter_1 != folder_iter->end();
                        ++name_iter_1) {
                    if (patString(name_iter_1->get_content()) == patString("paths")) {
                        paths_folder = folder_iter;
                        break;
                    }
                }
            }
        }
        //DEBUG_MESSAGE(paths_folder->get_name());
        xml::node::iterator od_iter = paths_folder->begin();
        for (; od_iter != paths_folder->end(); ++od_iter) {
            if (od_iter->is_text()) {
                continue;
            }
            //DEBUG_MESSAGE(od_iter->get_name());

                    if (patString(od_iter->get_name()) != patString("Folder")) {
                        continue;
                    }

            for (xml::node::iterator path_iter = od_iter->begin();
                    path_iter != od_iter->end();
                    ++path_iter) {
                if (path_iter->is_text()) {
                    continue;
                }
                //DEBUG_MESSAGE(path_iter->get_name());
                if (patString(path_iter->get_name()) != patString("Folder")) {
                        continue;
                    }
                patPathJ new_path;
                for (xml::node::iterator placemark_iter = path_iter->begin();
                        placemark_iter != path_iter->end();
                        ++placemark_iter) {
                    if (placemark_iter->is_text()) {
                        continue;
                    }
                    //DEBUG_MESSAGE(placemark_iter->get_name());
                    if (patString(placemark_iter->get_name()) == patString("Placemark")) {
                        for (xml::node::iterator descrip_iter = placemark_iter->begin();
                                descrip_iter != placemark_iter->end();
                                ++descrip_iter) {
                            if (patString(descrip_iter->get_name()) == patString("description")) {
                                patString desc_str(descrip_iter->get_content());
                                //DEBUG_MESSAGE(desc_str);
                                if (desc_str == "") {
                                    stringstream str;
                                    str << " no description specified in the link";
                                    err = new patErrMiscError(str.str());
                                    WARNING(err->describe());
                                    return false;
                                } else {
                                    sregex rex = sregex::compile( "Arc (\\d+) from node (\\d+) to node (\\d+)" );
                                    smatch what;

                                    if (regex_match(desc_str, what, rex)) {
                                        
                                        unsigned long link_id(atoi(what[1].str().c_str()));
                                        unsigned long up_node_id(atoi(what[2].str().c_str()));
                                        unsigned long down_node_id(atoi(what[3].str().c_str()));
                                        //DEBUG_MESSAGE(link_id<<":"<<up_node_id<<"-"<<down_node_id);
                                        patArc* new_arc = network->getArcFromNodesUserId(up_node_id, down_node_id);
                                        if (new_arc == NULL) {
                                            err = new patErrNullPointer("patArc");
                                            WARNING(err->describe());
                                            return false;

                                        }
                                        /*
                                        else if (new_arc->userId != link_id) {
                                            stringstream str;
                                            str << " link id doesn't match"
                                                    << new_arc->userId
                                                    << "-" << link_id;
                                            err = new patErrMiscError(str.str());
                                            WARNING(err->describe());

                                            return patFALSE;
                                        }
                                         * */
                                        else {

                                            new_path.addArcToBack(new_arc);
                                        }
                                    }

                                }
                            }

                        }
                    }
                }
                new_path.generateOd(network, err);
                DEBUG_MESSAGE("new path read" << new_path);
                path_set.insert(new_path);
            }

        }

    }
    return true;

}

patReadObservationFromKML::patReadObservationFromKML(const patReadObservationFromKML & orig) {
}

patReadObservationFromKML::~patReadObservationFromKML() {
}
