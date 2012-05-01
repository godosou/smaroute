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

patKMLPathWriter::patKMLPathWriter(string file_name) :
	patPathWriter::patPathWriter(file_name){
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
	p->set_description(desc.str());
	m_document->add_feature(p);
}
void patKMLPathWriter::close() {
	ofstream kml_file(m_file_name.c_str());

	KmlFactory* factory = KmlFactory::GetFactory();
	KmlPtr kml = factory->CreateKml();
	kml->set_feature(m_document);
	kml_file << kmldom::SerializePretty(kml);
	kml_file.close();

}
