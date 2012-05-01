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
	patPathSampling( patNetwork* baseNetwork, vector< list < pair<patArc*, unsigned long> > >* theadjList,patPathJ* aPath);
			
	void run(unsigned long algoName,
		patError*& err);
	void getShortestPathTreeDest(patError*& err);
	double  getShortestPathCostFrom(const unsigned long aId,patError*& err);
	double  computePathProbaForRandomWalk(patPathJ* thePath,patError*& err);
	patPathJ*  newPath(patPathJ aPath);
	void newSampledPath(patPathJ aPath, double proba);
	void addPath(patPathJ aPath,patError*& err);
	pair<patArc*,double>  getNextArcInRandomWalk(const patNode* fromNode,patUnixUniform* randomNumbersGenerator,patError*& err);
	void  getOnePathWithRandomWalk(patUnixUniform* randomNumbersGenerator,patError*& err1);
	void  getMultiplePathsWithRandomWalk(unsigned long nbrOfPaths,
									unsigned long nbrOfTrials,
									patUnixUniform* randomNumbersGenerator,
									patError*& err);
	bool  isAddedPath(patPathJ* aPath);
	void  computeEPS( patError*& err);
	void  computePathSizes( patError*& err);
	void  computePathAttributes(patError*& err);
								
	
	void randomWalk(patError*& err);
	
	set<patPathJ*>* getChoiceSet();
	double getPathEPS(patPathJ* aPath,patError*& err);
	double getPathPS(patPathJ* aPath,patError*& err);
	double getPathRwLogProba(patPathJ* aPath,patError*& err);
	unsigned long getPathReplicates(patPathJ* aPath,patError*& err);
	
protected:
	patUnixUniform randomNumbersGenerator;
	patNetwork* theNetwork;
	patOdJ* theOd;
	patPathJ* thePath;
  double shortestPathCost;
  patShortestPathTreeDest destTree;
  double kumaA;
  double kumaB;
set<patPathJ*>choiceSet;
map<patPathJ*,unsigned long> countReplicates;

        vector< list < pair<patArc*, unsigned long> > >* adjList;
map<patPathJ*,double> rwLogProba;
set<patPathJ*> addedPath;
map<patPathJ*,double> choiceSetEPS;
map<patPathJ*,double> choiceSetPS;
};
#endif
