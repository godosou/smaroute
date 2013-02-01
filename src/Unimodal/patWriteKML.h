
#ifndef patWriteKML_h
#define patWriteKML_h
#include "patType.h"
class patPathJ;
class patOdJ;
class patWriteKML{
public:
patWriteKML();
patString writePath(patPathJ* thePath,patString desc);
patString writeODSample(patOdJ* od);
protected:
}; 
#endif
