/*
 * patKMLPathWriter.cc
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#include "patKMLPathWriter.h"
#include <sstream>
#include <fstream>
using kmldom::KmlFactory;
using kmldom::KmlPtr;
using kmldom::PlacemarkPtr;
using kmldom::FolderPtr;
#include "patDisplay.h"
patKMLPathWriter::patKMLPathWriter(string file_name) :
		patPathWriter::patPathWriter(file_name) {
	// TODO Auto-generated constructor stub
	patCreateKmlStyles doc;
	m_document = doc.createStylesForKml();
}

patKMLPathWriter::~patKMLPathWriter() {
	// TODO Auto-generated destructor stub
}

void patKMLPathWriter::writePath(const patMultiModalPath& path,
		const map<string, string>& attr) {
	FolderPtr p = path.getKML(0);
	stringstream desc;
	for (map<string, string>::const_iterator a_iter = attr.begin();
			a_iter != attr.end(); ++a_iter) {
		desc << a_iter->first << ": " << a_iter->second << "\n";
	}
//	DEBUG_MESSAGE(desc.str());
	p->set_description(desc.str());
	patOd path_od(path.getUpNode(), path.getDownNode());
	map<patOd, FolderPtr>::iterator find_od = m_ods.find(path_od);
	if (find_od == m_ods.end()) {
		FolderPtr od_folder = path_od.getKML();
		pair<map<patOd, FolderPtr>::iterator, bool> insert_old_pair =
				m_ods.insert(pair<patOd, FolderPtr>(path_od,od_folder));
//		DEBUG_MESSAGE(insert_old_pair.second);
		find_od = insert_old_pair.first;
	}
	find_od->second->add_feature(p);
}
void patKMLPathWriter::close() {
//	DEBUG_MESSAGE("Dealt ods"<<m_ods.size()<<", file writen to"<<m_file_name);
	for (map<patOd, FolderPtr>::iterator od_iter = m_ods.begin();
			od_iter != m_ods.end(); ++od_iter) {
		m_document->add_feature(od_iter->second);
	}
//cout<<m_file_name;
	KmlFactory* factory = KmlFactory::GetFactory();
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(m_document);
	ofstream kml_file_op(m_file_name.c_str());
	kml_file_op << kmldom::SerializePretty(kml)<<endl;

	kml_file_op.close();

}
