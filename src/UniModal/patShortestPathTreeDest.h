//-*-c++-*------------------------------------------------------------
//
// File name : patShortestPathTreeDest.h
// Author :    \URL[Michel Bierlaire]{http://roso.epfl.ch/mbi}
// Date :      Tue May 16 15:07:07 2006
//
//--------------------------------------------------------------------

#ifndef patShortestPathTreeDest_h
#define patShortestPathTreeDest_h


#include <vector>
#include <list>
#include "patType.h"
#include "patError.h"


class patShortestPathTreeDest {

  friend class patShortestPathAlgoDest ;
  friend class patPathProcessor ;
  friend class patOdJ;//add by Jingmin
  friend class patMapMatching;//add by Jingmin
    friend class patMapMatchingV2;//add by Jingmin

  friend class patPathSampling;//add by Jingmin

public:

  patShortestPathTreeDest(unsigned long theDest, unsigned long nNodes) ;
  patShortestPathTreeDest();
  list<unsigned long> getShortestPathFrom(unsigned long node,patError*& err) ;
protected:
  unsigned long theDest ;
  vector<double> labels ;
  vector<unsigned long> successor ;
};

#endif
