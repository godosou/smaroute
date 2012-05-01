/*
 * patKMLPathWriter.h
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#ifndef PATKMLPATHWRITER_H_
#define PATKMLPATHWRITER_H_
#include "patPathWriter.h"
#include <map>
using namespace std;
#include "patCreateKmlStyles.h"
#include "kml/dom.h"
using kmldom::DocumentPtr;
class patKMLPathWriter:public patPathWriter {
public:
	patKMLPathWriter(string file_name);
	virtual void writePath(const patMultiModalPath& path, const map<string,string>& attr);
	virtual void close();
	virtual ~patKMLPathWriter();
protected:

	DocumentPtr m_document;
};

#endif /* PATKMLPATHWRITER_H_ */
