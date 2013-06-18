/*
 * patPathWritter.h
 *
 *  Created on: Apr 27, 2012
 *      Author: jchen
 */

#ifndef PATPATHWRITER_H_
#define PATPATHWRITER_H_
#include "patMultiModalPath.h"
#include <map>
using namespace std;
class patPathWriter {
public:
	patPathWriter(string file_name) :
			m_file_name(file_name) {
	}
	;
	virtual void writePath(const patMultiModalPath& path,
			const map<string, string>& attr)=0;
	virtual void close()=0;
	string getFileName() const{
		return m_file_name;
	}

protected:
	string m_file_name;
};

#endif /* PATPATHWRITER_H_ */
