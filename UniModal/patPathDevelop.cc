#include "patPathDevelop.h"
#include "patGpsPoint.h"
#include "patArc.h"
#include "patNBParameters.h"
#include "patPathJ.h"
#include "patDisplay.h"
#include "patNetwork.h"
#include "patPathProbaAlgoV4.h"
#include "patArcTransition.h"
#include "patObservation.h"
#include "patSample.h"
#include <time.h>
#include <numeric>
#include "patDisplay.h"
#include "patDiscreteDistribution.h"

patPathDevelop::patPathDevelop(patNetwork* theNetwork,
		set<patArcTransition>* theArcTranSet,
		vector<patGpsPoint>* theOriginalGpsPoints) :
		baseNetwork(theNetwork), arcTranSet(theArcTranSet), originalGpsPoints(
				theOriginalGpsPoints), ranNumUniform(time(NULL)) {

}

set<patULong> patPathDevelop::genEndNodes() {
	set < patULong > endNodes;
	for (map<patULong, map<patPathJ, patReal> >::iterator iter1 =
			pathSet.begin(); iter1 != pathSet.end(); ++iter1) {
		endNodes.insert(iter1->first);
	}

	return endNodes;
}

void patPathDevelop::lowSpeedPoints(vector<patGpsPoint*>* lowSpeedGpsPoints,
		set<patArc*>* interArcs, patNetwork* theNetwork) {

	for (vector<patGpsPoint*>::iterator gpsIter = lowSpeedGpsPoints->begin();
			gpsIter != lowSpeedGpsPoints->end(); ++gpsIter) {
		patGpsPoint* currGps = *gpsIter;
		currGps->lowSpeedDDR(interArcs, theNetwork);
		if (!currGps->emptyDomain()) {
			gpsSequence.push_back(currGps);
		}

	}

}
void patPathDevelop::appendSeg(patGpsPoint* gpsPoint,
		map<patULong, set<patPathJ> >* segSet,set<patArc*>* inheritDDR) {
	gpsSequence.push_back(gpsPoint);
	oldPathSet = pathSet;
	patReal minPathLength = patMaxReal;
	DEBUG_MESSAGE("ddrs:" << gpsSequence.back()->getLinkDDR()->size());
	map<patPathJ, patReal>* newPathSet = new map<patPathJ, patReal>;

	for (map<patULong, map<patPathJ, patReal> >::iterator iter1 =
			pathSet.begin(); iter1 != pathSet.end(); ++iter1) {
		for (map<patPathJ, patReal>::iterator iter2 = iter1->second.begin();
				iter2 != iter1->second.end(); ++iter2) {
			if (inheritDDR->find(const_cast<patPathJ*>(&(iter2->first))->back())
					!= inheritDDR->end()) {
				patReal length =
						const_cast<patPathJ*>(&(iter2->first))->computePathLength();
				minPathLength =
						(length < minPathLength) ? length : minPathLength;
				newPathSet->insert(
						pair<patPathJ, patReal>(iter2->first, length));
			}
		}

	}

	set<patArc*> activeDomain = gpsSequence.back()->selectDomainByCdf(
			baseNetwork);
	;
	for (map<patULong, map<patPathJ, patReal> >::iterator iter3 =
			pathSet.begin(); iter3 != pathSet.end(); ++iter3) {

		map<patULong, set<patPathJ> >::iterator segFound = segSet->find(
				iter3->first);

		if (segFound != segSet->end() && !segFound->second.empty()) {
			//DEBUG_MESSAGE(iter3->first<<", "<<iter3->second.size()<<", "<<segFound->second.size());
			unionTwoDomains(&minPathLength, newPathSet, &(iter3->second),
					&(segFound->second), &activeDomain);
		}
	}
	/*
	 DEBUG_MESSAGE(newPathSet->size()<<"min length:"<<minPathLength);
	 patReal lengthCeil = minPathLength * patNBParameters::the()->maxPathLengthRatio ;
	 for(map<patPathJ,patReal>::iterator iter = newPathSet->begin();
	 iter!=newPathSet->end();){
	 //DEBUG_MESSAGE("length: "<<iter->second);
	 if(minPathLength >= patNBParameters::the()->minPathInterLength ){
	 if(iter->second > lengthCeil){
	 newPathSet->erase(iter++);
	 continue;
	 }
	 }
	 else if(iter->second >= patNBParameters::the()->maxPathBeginLength ){
	 newPathSet->erase(iter++);
	 continue;
	 }

	 iter->second = patMaxReal;
	 iter++;

	 }
	 */
	DEBUG_MESSAGE("before select path set: " << newPathSet->size());
	//if(gpsSequence.back() != &(originalGpsPoints->back())){
	if (newPathSet->size()
			> patNBParameters::the()->minGeneratedInterMediatePath) {
		selectPaths(newPathSet);
	}
	//}
	finalizeOneStage(newPathSet);
	delete newPathSet;
}

void patPathDevelop::unionTwoDomains(patReal* minLength,
		map<patPathJ, patReal>* newPathSet,
		map<patPathJ, patReal>* prevSet,set<patPathJ>* newSegSet
		,set<patArc*>* activeDomain) {
	for (map<patPathJ, patReal>::iterator iter1 = prevSet->begin();
			iter1 != prevSet->end(); ++iter1) {
		for (set<patPathJ>::iterator iter2 = newSegSet->begin();
				iter2 != newSegSet->end(); ++iter2) {

			//DEBUG_MESSAGE(aPath<<","<<*iter2);
			patPathJ* bPath = const_cast<patPathJ*>(&(*(iter2)));
			if (bPath->containLoop()) {
				continue;
			}
			patPathJ aPath = iter1->first;
			list<patArc*>* aL = bPath->getArcList();
			if (activeDomain->find(aL->back()) == activeDomain->end()) {
				//DEBUG_MESSAGE("not in domain");
				continue;
			}
			if (aL->size() > 1
					&& activeDomain->find(aL->front()) == activeDomain->end()) {
				if (aPath.back()->downNodeId == aL->front()->upNodeId
						&& aPath.back()->upNodeId == aL->front()->downNodeId) {
					//a u turn at the begining
					//DEBUG_MESSAGE("a u turn");
					continue;
				}

			}
			for (list<patArc*>::iterator arcIter = aL->begin();
					arcIter != aL->end(); ++arcIter) {
				aPath.addArcToBack(*arcIter);
			}
			if (newPathSet->find(aPath) == newPathSet->end()) {
				patReal length = aPath.computePathLength();

				(*minLength) = (length < *minLength) ? length : *minLength;
				newPathSet->insert(pair<patPathJ, patReal>(aPath, length));
			}
		}
	}

}

patReal patPathDevelop::sumProba(map<patPathJ, patReal>* newPathSet) {
	patReal r = 0.0;
	for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
			iter1 != newPathSet->end(); ++iter1) {
		r += iter1->second;
	}

	return r;
}
void patPathDevelop::selectPathsByPdf(list<patPathJ*>* preSelected,
		map<patPathJ, patReal>* newPathSet) {

	patReal totalProba = sumProba(newPathSet);
	set<patPathJ*> tmpSet;
	tmpSet.insert(preSelected->begin(), preSelected->end());
	for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
			iter1 != newPathSet->end(); ++iter1) {
		if (iter1->second * patNBParameters::the()->selectPathInversePercent
				>= totalProba) {
			tmpSet.insert(const_cast<patPathJ*>(&(iter1->first)));
			newPathSet->erase(iter1++);
		}
	}
	if (tmpSet.empty()) {
		selectPathsByCdf(preSelected, newPathSet);
	} else {
		for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
				iter1 != newPathSet->end();) {

			if (tmpSet.find(const_cast<patPathJ*>(&(iter1->first)))
					== tmpSet.end()) {
				newPathSet->erase(iter1++);
			} else {
				iter1++;
			}
		}

	}

}
void patPathDevelop::selectPathsByCdf(list<patPathJ*>* preSelected,
		map<patPathJ, patReal>* newPathSet) {

	set<patPathJ*> tmpSet;
	selectPathsByShortest(newPathSet, &tmpSet);

	DEBUG_MESSAGE("select shortest" << tmpSet.size());
	tmpSet.insert(preSelected->begin(), preSelected->end());

	patReal totalProba = sumProba(newPathSet);
	patReal th = patNBParameters::the()->selectPathCdfThreshold * totalProba;
	map < patReal, set<patPathJ*> > ddrPathSet;
	for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
			iter1 != newPathSet->end(); ++iter1) {
		if (iter1->second == 0.0) {
			continue;
		}
		patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));

		if (ddrPathSet.find(iter1->second) == ddrPathSet.end()) {

			ddrPathSet[iter1->second] = set<patPathJ*>();
		}
		ddrPathSet[iter1->second].insert(currPath);

	}

	map<patReal, set<patPathJ*> >::iterator ddrPathIter = ddrPathSet.end();
	patReal cdfProba = 0.0;
	while (ddrPathIter != ddrPathSet.begin()) {
		ddrPathIter--;
		cdfProba += ddrPathIter->first * ddrPathIter->second.size();
//		DEBUG_MESSAGE("proba: "<<ddrPathIter->first<<", number: "<<ddrPathIter->second.size()<<", cdf: "<<cdfProba/totalProba);
		tmpSet.insert(ddrPathIter->second.begin(), ddrPathIter->second.end());
		if (cdfProba >= th) {
			break;
		}
	}

	if (ddrPathIter == ddrPathSet.begin()) {

		DEBUG_MESSAGE("no path is excluded by cdf criteria");
		return;
	} else {
		DEBUG_MESSAGE(
				tmpSet.size() << " out of " << newPathSet->size()
						<< " paths are remained by cdf criteria");

		for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
				iter1 != newPathSet->end();) {
			if (tmpSet.find(const_cast<patPathJ*>(&(iter1->first)))
					== tmpSet.end()) {
				newPathSet->erase(iter1++);
			} else {
				iter1++;
			}
		}

		if (newPathSet->size() > 100) {
			selectPathsByPdf(preSelected, newPathSet);
		}
	}

}

void patPathDevelop::selectPathsByNumber(list<patPathJ*>* preSelected,
		map<patPathJ, patReal>* newPathSet) {
	map < patReal, set<patPathJ*> > ddrPathSet;
	/*
	 map<patReal,set<patPathJ*> > lengthPathSet;

	 map<patReal,set<patPathJ*> > distPathSet;
	 map<patReal,set<patPathJ*> > ddrPathSet_avg;
	 */
	for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
			iter1 != newPathSet->end(); ++iter1) {
		patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));
		/*
		 patReal pl = currPath->computePathLength();
		 if(lengthPathSet.find(pl)==lengthPathSet.end()){
		 lengthPathSet[pl]=set<patPathJ*>();
		 }
		 lengthPathSet[pl].insert(currPath);

		 patPathProbaAlgoV4 pathProba(currPath, &gpsSequence,baseNetwork,arcTranSet,originalGpsPoints);

		 patReal dist = pathProba.run_dist();

		 if(distPathSet.find(dist)==distPathSet.end()){
		 distPathSet[dist]=set<patPathJ*>();
		 }
		 distPathSet[dist].insert(currPath);

		 */
		if (ddrPathSet.find(iter1->second) == ddrPathSet.end()) {

			ddrPathSet[iter1->second] = set<patPathJ*>();
		}
		ddrPathSet[iter1->second].insert(currPath);

		/*
		 patReal ddrAvg = iter1->second/pl;

		 if(ddrPathSet_avg.find(ddrAvg)==ddrPathSet_avg.end()){

		 ddrPathSet_avg[ddrAvg]=set<patPathJ*>();
		 }
		 ddrPathSet_avg[ddrAvg].insert(currPath);
		 */

	}

	map<patReal, set<patPathJ*> >::iterator ddrPathIterBack = ddrPathSet.end();
	/*
	 map<patReal,set<patPathJ*> >::iterator ddrPathIterBack_avg= ddrPathSet_avg.end();

	 map<patReal,set<patPathJ*> >::iterator lengthPathIterFront= lengthPathSet.begin();
	 */
	patULong best = patNBParameters::the()->selectBestPaths;
	patULong worst = patNBParameters::the()->selectWorstPaths;
	patULong shortest = patNBParameters::the()->selectShortestPaths;

	set<patPathJ*> tmpPathSet;
	tmpPathSet.insert(preSelected->begin(), preSelected->end());
	for (patULong i = 0; i < best; ++i) {
		if (ddrPathIterBack != ddrPathSet.begin()) {
			ddrPathIterBack--;
			tmpPathSet.insert(ddrPathIterBack->second.begin(),
					ddrPathIterBack->second.end());
		}

		/*	
		 if(ddrPathIterBack_avg!=ddrPathSet_avg.begin()){
		 ddrPathIterBack_avg--;
		 tmpPathSet.insert(ddrPathIterBack_avg->second.begin(),ddrPathIterBack_avg->second.end());

		 }

		 */
	}
	/*
	 for(patULong i=0;i<shortest;++i){
	 if(lengthPathIterFront!=lengthPathSet.end()){
	 tmpPathSet.insert(lengthPathIterFront->second.begin(),lengthPathIterFront->second.end());
	 lengthPathIterFront++;

	 }
	 }
	 */
	for (map<patPathJ, patReal>::iterator pIter = newPathSet->begin();
			pIter != newPathSet->end();) {

		if (tmpPathSet.find(const_cast<patPathJ*>(&(pIter->first)))
				== tmpPathSet.end()) {
			newPathSet->erase(pIter++);
		} else {
			pIter++;
		}
	}

}

void patPathDevelop::selectPathsByShortest(map<patPathJ, patReal>* newPathSet,
set<patPathJ*>* tmpPathSet) {
	map < patReal, set<patPathJ*> > ddrPathSet;
	map < patReal, set<patPathJ*> > lengthPathSet;

	for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
			iter1 != newPathSet->end(); ++iter1) {
		patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));
		patReal pl = currPath->computePathLength();
		if (lengthPathSet.find(pl) == lengthPathSet.end()) {
			lengthPathSet[pl] = set<patPathJ*>();
		}
		lengthPathSet[pl].insert(currPath);
	}
	map<patReal, set<patPathJ*> >::iterator lengthPathIterFront =
			lengthPathSet.begin();
	patULong shortest = patNBParameters::the()->selectShortestPaths;
	for (patULong i = 0; i < shortest; ++i) {
		if (lengthPathIterFront != lengthPathSet.end()) {
			tmpPathSet->insert(lengthPathIterFront->second.begin(),
					lengthPathIterFront->second.end());
			lengthPathIterFront++;

		}
	}
}

void patPathDevelop::calPathDDR(map<patPathJ, patReal>* newPathSet) {

	for (map<patPathJ, patReal>::iterator iter = newPathSet->begin();
			iter != newPathSet->end(); ++iter) {
		patPathProbaAlgoV4 pathProba(const_cast<patPathJ*>(&(iter->first)),
				&gpsSequence, baseNetwork, arcTranSet, originalGpsPoints);
		//DEBUG_MESSAGE("algo:"<<patNBParameters::the()->algoInSelection);
		patReal ddr = pathProba.run(patNBParameters::the()->algoInSelection);
//		DEBUG_MESSAGE("path DDR"<<ddr);
		if (patNBParameters::the()->algoInSelection == "dist"
				|| patNBParameters::the()->algoInSelection == "mm") {
			ddr = -ddr;
		}
		iter->second = ddr;
	}

}

list<patPathJ*> patPathDevelop::selectPathByImportantDDR(
		map<patPathJ, patReal>* newPathSet) {
	list<patPathJ*> impPath;
	set<patArc*> impDDR = selectImportantDDR();
	DEBUG_MESSAGE("number of important ddrs" << impDDR.size());

	for (set<patArc*>::iterator impIter = impDDR.begin();
			impIter != impDDR.end(); ++impIter) {

		vector<patPathJ*> pathViaImpDDR;
		vector < patReal > pathDDR;
		for (map<patPathJ, patReal>::iterator pathIter = newPathSet->begin();
				pathIter != newPathSet->end(); ++pathIter) {
			if (pathIter->first.isLinkInPath(*impIter)) {
//				DEBUG_MESSAGE(pathIter->second)
				pathViaImpDDR.push_back(
						const_cast<patPathJ*>(&(pathIter->first)));
				pathDDR.push_back(pathIter->second);
			}
		}
		if (pathDDR.size() == 0) {
			continue;
		}
		patDiscreteDistribution discreteDraw(&pathDDR, &ranNumUniform);
		patULong d = discreteDraw();
		impPath.push_back(pathViaImpDDR[d]);
	}
	DEBUG_MESSAGE("select important ddrs' paths" << impPath.size());
	return impPath;
}
/*
 
 select important ddr with simulations
 */
set<patArc*> patPathDevelop::selectImportantDDR() {
	set<patArc*> importantDDR;
	map<patArc*, patReal>* linkDDR = gpsSequence.back()->getLinkDDR();
	if (patNBParameters::the()->selectImportantDDRCdf == 1.0) {
		DEBUG_MESSAGE("the whole ddr is selected as import ddr");
		for (map<patArc*, patReal>::iterator ddrIter = linkDDR->begin();
				ddrIter != linkDDR->end(); ++ddrIter) {
			importantDDR.insert(ddrIter->first);
		}
		return importantDDR;
	}

	patReal sumTotal = gpsSequence.back()->calDDRSum();

	vector<patArc*> ddrArcs;
	vector < patReal > ddrValues;

	for (map<patArc*, patReal>::iterator ddrIter = linkDDR->begin();
			ddrIter != linkDDR->end(); ++ddrIter) {
		ddrArcs.push_back(ddrIter->first);
		ddrValues.push_back(ddrIter->second);
	}

	patDiscreteDistribution discreteDraw(&ddrValues, &ranNumUniform);

	patReal selectedDDRSum = 0.0;

	while (selectedDDRSum / sumTotal
			< patNBParameters::the()->selectImportantDDRCdf
			|| gpsSequence.back()->getLinkDDR()->size()
					< patNBParameters::the()->minDomainSize) {

		patULong selectedArcIndex = discreteDraw();
		pair<set<patArc*>::iterator, bool> insertResult = importantDDR.insert(
				ddrArcs[selectedArcIndex]);
		if (insertResult.second == true) {
			//DEBUG_MESSAGE("selected ddr"<<ddrValues[selectedArcIndex]);
			selectedDDRSum += ddrValues[selectedArcIndex];
		}

	}

	return importantDDR;
}
void patPathDevelop::selectPaths(map<patPathJ, patReal>* newPathSet) {

	if (newPathSet->empty()) {
		return;
	}

	calPathDDR(newPathSet);

	list<patPathJ*> preSelected = selectPathByImportantDDR(newPathSet);
	if (patNBParameters::the()->selectPathCte == "pdf") {
		selectPathsByPdf(&preSelected, newPathSet);

	}
	if (patNBParameters::the()->selectPathCte == "number") {
		selectPathsByNumber(&preSelected, newPathSet);

	}
	if (patNBParameters::the()->selectPathCte == "cdf") {
		selectPathsByCdf(&preSelected, newPathSet);

	}

}

patGpsPoint* patPathDevelop::lastGpsPoint() {
	return gpsSequence.back();
}
void patPathDevelop::finalizeOneStage(map<patPathJ, patReal>* newPathSet) {

	set<patArc*> newLinkDDR;
	if (!newPathSet->empty()) {
		pathSet = map<patULong, map<patPathJ, patReal> >();
		for (map<patPathJ, patReal>::iterator iter1 = newPathSet->begin();
				iter1 != newPathSet->end(); ++iter1) {
			patPathJ* currPath = const_cast<patPathJ*>(&(iter1->first));
			if (currPath != NULL) {
				patULong endNode = currPath->endNodeUserId();
				if (pathSet.find(endNode) == pathSet.end()) {
					pathSet[endNode] = map<patPathJ, patReal>();
				}
				newLinkDDR.insert(currPath->back());
				pathSet[endNode].insert(*iter1);
			}
		}
		DEBUG_MESSAGE("path size" << newPathSet->size());
	}
	if (newPathSet->size() == 0 || newPathSet->empty()) {
		DEBUG_MESSAGE("no path generated, invalid gps");
		gpsSequence.pop_back();
	}
	//gpsSequence.back()->updateLinkDDR(&newLinkDDR);
	DEBUG_MESSAGE("final ddr " << gpsSequence.back()->getLinkDDR()->size());

}

set<patPathJ> patPathDevelop::getPaths() {
	set<patPathJ> rtnPath;
	for (map<patULong, map<patPathJ, patReal> >::iterator iter1 =
			pathSet.begin(); iter1 != pathSet.end(); ++iter1) {
		for (map<patPathJ, patReal>::iterator iter2 = iter1->second.begin();
				iter2 != iter1->second.end(); ++iter2) {
			rtnPath.insert(iter2->first);
		}
	}

	return rtnPath;
}

void patPathDevelop::init(patGpsPoint* firstGpsPoint) {
	map<patPathJ, patReal>* newPathSet = new map<patPathJ, patReal>;
	gpsSequence.push_back(firstGpsPoint);
	map<patArc*, patReal>* linkDDR = firstGpsPoint->getLinkDDR();
	//DEBUG_MESSAGE("link ddr size"<<linkDDR->size());
	if (linkDDR->empty()) {
		return;
	}
	map < patULong, set<patULong> > succ;
	for (map<patArc*, patReal>::iterator iter1 = linkDDR->begin();
			iter1 != linkDDR->end(); ++iter1) {
		if (succ.find(iter1->first->upNodeId) == succ.end()) {
			succ[iter1->first->upNodeId] = set<patULong>();
		}
		succ[iter1->first->upNodeId].insert(iter1->first->downNodeId);
	}

	for (map<patULong, set<patULong> >::iterator iter2 = succ.begin();
			iter2 != succ.end(); ++iter2) {

		list < patULong > pathTemp;
		connectNodes(iter2->first, &pathTemp, &succ, newPathSet);
	}

	finalizeOneStage(newPathSet);
	delete newPathSet;
}

void patPathDevelop::connectNodes(patULong node, list<patULong>* pathTemp,
		map<patULong, set<patULong> >* succ,
		map<patPathJ, patReal>* newPathSet) {

	pathTemp->push_back(node);
	newInitPath(pathTemp, newPathSet);
	map<patULong, set<patULong> >::iterator found = succ->find(node);
	if (found != succ->end()) {
		for (set<patULong>::iterator iter1 = found->second.begin();
				iter1 != found->second.end(); ++iter1) {
			patBoolean flag = patTRUE;
			for (list<patULong>::iterator iter2 = pathTemp->begin();
					iter2 != pathTemp->end(); ++iter2) {
				if (*iter2 == *iter1) {
					flag = patFALSE;
					break;
				}
			}
			if (flag) {
				connectNodes(*iter1, pathTemp, succ, newPathSet);
				pathTemp->pop_back();
			}
		}
	}
}

void patPathDevelop::newInitPath(list<patULong>* pathTemp,
		map<patPathJ, patReal>* newPathSet) {
	if (pathTemp->empty() || pathTemp->size() <= 1) {
		return;
	} else {

		patPathJ newPath;
		list<patULong>::iterator nIter = pathTemp->begin();
		nIter++;
		patULong prevNode = pathTemp->front();
		for (; nIter != pathTemp->end(); ++nIter) {
			patULong currNode = *nIter;
			patArc* currArc = baseNetwork->getArcFromNodesUserId(prevNode,
					currNode);
			if (currArc == NULL) {
				return;
			}
			newPath.addArcToBack(currArc);

			prevNode = currNode;
		}
		patReal ddr = newPath.computePointDDRRaw(
				gpsSequence.back()->getLinkDDR());
		newPathSet->insert(pair<patPathJ, patReal>(newPath, ddr));
	}

}

patReal patPathDevelop::calAverageSpeed() {
	patReal avegSpeed = 0.0;
	patReal count = 0;
	for (patULong i = 0; i < gpsSequence.size(); ++i) {
		patReal as = gpsSequence.at(i)->getSpeedMS();
		if (as >= 1.0) {

			avegSpeed += gpsSequence.at(i)->getSpeedMS();
			count++;
		}
	}

	avegSpeed /= count;
	//DEBUG_MESSAGE("average speed"<<avegSpeed/3.6);
	return avegSpeed;
}

patReal patPathDevelop::calLengthCeil() {
	return patNBParameters::the()->pathLengthCeil * calAverageSpeed()
			* (gpsSequence.back()->getTimeStamp()
					- gpsSequence.front()->getTimeStamp());
}

patReal patPathDevelop::lastStage(patSample* theSample,
		patObservation* generatedObservation) {
	//DEBUG_MESSAGE("all paths"<<pathSet.size());

	for (map<patULong, map<patPathJ, patReal> >::iterator destIter =
			pathSet.begin(); destIter != pathSet.end(); ++destIter) {
		for (map<patPathJ, patReal>::iterator pathIter =
				destIter->second.begin(); pathIter != destIter->second.end();
				++pathIter) {

			patPathJ* thePath = const_cast<patPathJ*>(&(pathIter->first));
			patError* err(NULL);

			patOdJ tod = thePath->generateOd(baseNetwork, err);
			if (err != NULL) {
				continue;
			}
			//DEBUG_MESSAGE(patNBParameters::the()->algoInSelection);
			patOdJ* theOd = theSample->addOd(tod);

			thePath->assignOd(theOd);
			patPathJ* pathPointer = theOd->addPath(*thePath);
			//DEBUG_MESSAGE("calculate proba for "<<pathIter->first);
			patPathProbaAlgoV4 pathProba(pathPointer, &gpsSequence, baseNetwork,
					arcTranSet, originalGpsPoints);

			patReal value1 = pathProba.run(
					patNBParameters::the()->algoInSelection);
			//	patReal value1 = 0.0;
			patReal value2 = 0.0;
			patReal value3 = 0.0;
			//patReal value3 = pathProba.run("raw");
			patReal value4 = pathProba.run("mm");

			generatedObservation->pathDDRsReal[pathPointer] = vector<patReal>();
			generatedObservation->pathDDRsReal[pathPointer].push_back(value1);
			generatedObservation->pathDDRsReal[pathPointer].push_back(value2);
			generatedObservation->pathDDRsReal[pathPointer].push_back(value3);
			generatedObservation->pathDDRsReal[pathPointer].push_back(value4);

			DEBUG_MESSAGE(
					"old value:" << pathIter->second
							<< patNBParameters::the()->algoInSelection << value1
							<< ", simple:" << value2 << ", raw:" << value3);
		}
	}

	DEBUG_MESSAGE("finish all paths");

	DEBUG_MESSAGE("final paths" << generatedObservation->pathDDRsReal.size());
}
