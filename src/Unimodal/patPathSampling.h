#ifndef patPathSampling_h
#define patPathSampling_h

#include "patUniform.h" 
#include "patParameters.h"
#include "patRandomNumberGenerator.h"
#include "patNormalWichura.h"
#include "patUnixUniform.h"
#include <set>
#include "patOdJ.h"
#include "patType.h"
#include "patError.h"
class patNetwork;
class patSample;
class patPathSampling{
public:
	patPathSampling( patNetwork* baseNetwork, vector< list < pair<patArc*, patULong> > >* theadjList,patPathJ* aPath);
			
	void run(patULong algoName,
		patError*& err);
	void getShortestPathTreeDest(patError*& err);
	patReal  getShortestPathCostFrom(const patULong aId,patError*& err);
	patReal  computePathProbaForRandomWalk(patPathJ* thePath,patError*& err);
	patPathJ*  newPath(patPathJ aPath);
	void newSampledPath(patPathJ aPath, patReal proba);
	void addPath(patPathJ aPath,patError*& err);
	pair<patArc*,patReal>  getNextArcInRandomWalk(const patNode* fromNode,patUnixUniform* randomNumbersGenerator,patError*& err);
	void  getOnePathWithRandomWalk(patUnixUniform* randomNumbersGenerator,patError*& err1);
	void  getMultiplePathsWithRandomWalk(patULong nbrOfPaths,
									patULong nbrOfTrials,
									patUnixUniform* randomNumbersGenerator,
									patError*& err);
	patBoolean  isAddedPath(patPathJ* aPath);
	void  computeEPS( patError*& err);
	void  computePathSizes( patError*& err);
	void  computePathAttributes(patError*& err);
								
	
	void randomWalk(patError*& err);
	
	set<patPathJ*>* getChoiceSet();
	patReal getPathEPS(patPathJ* aPath,patError*& err);
	patReal getPathPS(patPathJ* aPath,patError*& err);
	patReal getPathRwLogProba(patPathJ* aPath,patError*& err);
	patULong getPathReplicates(patPathJ* aPath,patError*& err);
	
protected:
	patUnixUniform randomNumbersGenerator;
	patNetwork* theNetwork;
	patOdJ* theOd;
	patPathJ* thePath;
  patReal shortestPathCost;
  patShortestPathTreeDest destTree;
  patReal kumaA;
  patReal kumaB;
set<patPathJ*>choiceSet;
map<patPathJ*,patULong> countReplicates;

        vector< list < pair<patArc*, patULong> > >* adjList;
map<patPathJ*,patReal> rwLogProba;
set<patPathJ*> addedPath;
map<patPathJ*,patReal> choiceSetEPS;
map<patPathJ*,patReal> choiceSetPS;
};
#endif
