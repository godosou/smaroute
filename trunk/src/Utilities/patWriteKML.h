
#ifndef patWriteKML_h
#define patWriteKML_h
#include "patType.h"
class patPathJ;
class patOd;
class patWriteKML{
public:
patWriteKML();
patString writePath(patPathJ* thePath,patString desc);
patString writeODSample(patOd* od);
protected:
}; 
#endif
