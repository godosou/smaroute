/* 
 * File:   patDownloadOSMFile.h
 * Author: jchen
 *
 * Created on April 14, 2011, 8:57 PM
 */

#ifndef PATDOWNLOADOSMFILE_H
#define	PATDOWNLOADOSMFILE_H

#include "patType.h"
class patDownloadOSMFile{
public:
    patDownloadOSMFile();
    patDownloadOSMFile(patReal top, patReal bottom, patReal right, patReal top);
    patDownloadUsingApi();
    
protected:
    patString FileName;

};

#endif	/* PATDOWNLOADOSMFILE_H */

