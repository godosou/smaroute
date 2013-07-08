/*
 * listDirectory.h
 *
 *  Created on: Nov 14, 2011
 *      Author: jchen
 */

#ifndef LISTDIRECTORY_H_
#define LISTDIRECTORY_H_
#include "patError.h"
#include <list>
class patListDirectory {
public:
	patListDirectory();
	void  getFilesInDeep(const string & dirName,
			const string condition, vector<string>& file_list);

	static list<string> getListOfFiles(string directory_name, patError* err);
	virtual ~patListDirectory();
};

#endif /* LISTDIRECTORY_H_ */
