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
class listDirectory {
public:
	listDirectory();
	static list<string> getListOfFiles(string directory_name, patError* err);
	virtual ~listDirectory();
};

#endif /* LISTDIRECTORY_H_ */
