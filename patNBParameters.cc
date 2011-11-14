#include "patDisplay.h"
#include "patNBParameters.h"
#include "patErrMiscError.h"
#include "patArcTransition.h"
#include <xmlwrapp/xmlwrapp.h>

patNBParameters::patNBParameters(patString fname)
{
	 
}

patNBParameters::patNBParameters()
{
	 
}
patNBParameters* patNBParameters::ins=NULL;

patNBParameters* patNBParameters::the() {
  if (patNBParameters::ins == NULL) {
    patNBParameters::ins = new patNBParameters ;
    //assert(singleInstance != NULL) ;
  } 
  return patNBParameters::ins ;
}

patBoolean patNBParameters::readFile(patString fileName,patError*& err){

DEBUG_MESSAGE("parsing config file:"<<fileName);
	xml::init xmlinit ;  
  
DEBUG_MESSAGE("OK0");
  xml::tree_parser parser(fileName.c_str(),true); 
DEBUG_MESSAGE("OK1");
  xml::document &doc = parser.get_document(); 
DEBUG_MESSAGE("OK2");
  doc.save_to_file ("t.xml", 0);
DEBUG_MESSAGE("OK3");
  if (!parser) {
    stringstream str ;
    str << "Error while parsing " << fileName ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return patFALSE ;
  }

	DEBUG_MESSAGE("OK4");
  xml::node theRoot = doc.get_root_node() ;
	
  DEBUG_MESSAGE("Node: " << theRoot.get_name()) ;
  xml::node::node_type theType = theRoot.get_type() ;
  DEBUG_MESSAGE("Node type: " << theType) ;
	
	xml::nodes_view modules(theRoot.elements("module"));
	for(xml::nodes_view::iterator mIter = modules.begin();
		mIter!=modules.end();
		++mIter){
		
		xml::attributes attr = mIter->get_attributes() ;
		xml::attributes::iterator attrIter1=attr.find("name");
		if (attrIter1==attr.end()){
			continue;
		}
		else{
			patString moduleName(attrIter1->get_value());		
			DEBUG_MESSAGE("module:"<<moduleName);
			xml::nodes_view params(mIter->elements("param"));
			patULong k=0;
			for(xml::nodes_view::iterator pIter = params.begin();
				pIter!=params.end();
				++pIter){
				k+=1;
				
				xml::attributes pAttr = pIter->get_attributes() ;
				xml::attributes::iterator attrIterN=pAttr.find("name");
				xml::attributes::iterator attrIterV=pAttr.find("value");
				xml::attributes::iterator attrIterT=pAttr.find("type");
				
					
				if(attrIterN==pAttr.end()||attrIterV==pAttr.end()||attrIterT==pAttr.end()){
					continue;
				}
				else{
					patString pName(attrIterN->get_value());
					patString pValue(attrIterV->get_value());						
					patString pType(attrIterT->get_value());
					
					DEBUG_MESSAGE(pName<<", "<<pValue<<", "<<pType);
					setParam(pName,pValue,pType);
				}
	
				
			}
	
			DEBUG_MESSAGE("there are "<<k<<" params");
			
	
		}
		
	}
  return patTRUE ;
}
patBoolean patNBParameters::setParam(patString name,patString value,patString type){
	pair<patString, pair<patString,patString> > el = pair<patString,pair<patString,patString> >(name,pair<patString,patString>(value,type)) ;
	if(params.insert(el).second){
		return patTRUE;
	}
	return patFALSE;
}


patBoolean patNBParameters::getParam(patString name, patReal* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "float"){
		return patFALSE;
	}
	else{
		*value = atof(pIter->second.first.c_str());
		return patTRUE;
	}
}


patBoolean patNBParameters::getParam(patString name, patULong* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "int"){
		return patFALSE;
	}
	else{
		*value = atol(pIter->second.first.c_str());
		return patTRUE;
	}
}


patBoolean patNBParameters::getParam(patString name, patString* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "string"){
		return patFALSE;
	}
	else{
		*value = pIter->second.first;
		return patTRUE;
	}
}

void patNBParameters::showAll(){
	for(map<patString, pair<patString,patString> >::iterator pIter=params.begin();
			pIter!=params.end();
			++pIter){
		DEBUG_MESSAGE(pIter->first<<","<<pIter->second.first<<","<<pIter->second.second);
	
	}
}

void patNBParameters::init(patError*& err){
	map<patString,patReal*> realParams;
	map<patString,patULong*> intParams;
	map<patString,patString*> strParams;
	realParams["maxHorizonAccuracy"]=&maxHorizonAccuracy;
	realParams["maxHeadingAccuracy"]=&maxHeadingAccuracy;
	realParams["maxSpeedAccuracy"]=&maxSpeedAccuracy;
	realParams["minArcDDR"]=&minArcDDR;
	realParams["maxHeadingGPSArc"]=&maxHeadingGPSArc;
	realParams["maxDistanceGPSArc"]=&maxDistanceGPSArc;
	realParams["maxDistanceGPSLoc"]=&maxDistanceGPSLoc;
	realParams["minNormalSpeed"]=&minNormalSpeed;
	realParams["minAverageSpeedRatio"]=&minAverageSpeedRatio;
	realParams["zoneRadius"]=&zoneRadius;
	realParams["zoneTime"]=&zoneTime;
	realParams["kumaA"]=&kumaA;
	realParams["kumaB"]=&kumaB;
	realParams["leftTurnPenalty"]=&leftTurnPenalty;
	realParams["goStraightPenalty"]=&goStraightPenalty;
	realParams["rightTurnPenalty"]=&rightTurnPenalty;
	realParams["minNodeDDR"]=&minNodeDDR;
	realParams["maxDistanceRatioSP"]=&maxDistanceRatioSP;
	realParams["maxNormalSpeedHeading"]=&maxNormalSpeedHeading;
	realParams["simHorizonArcLength"]=&simHorizonArcLength;
	realParams["simTurnUp"]=&simTurnUp;
	realParams["simSpeed"]=&simSpeed;
	realParams["simNetworkUpDownDistance"]=&simNetworkUpDownDistance;
	realParams["simSpeedDeltaRatio"]=&simSpeedDeltaRatio;
	realParams["pathLengthCeil"]=&pathLengthCeil;
	realParams["pathSizeGamma"]=&pathSizeGamma;
	realParams["stopPenalty"]=&stopPenalty;
	realParams["maxPathLengthRatio"]=&maxPathLengthRatio;
	realParams["minPathInterLength"]=&minPathInterLength;
	realParams["maxPathBeginLength"]=&maxPathBeginLength;
	realParams["selectPathInversePercent"]=&selectPathInversePercent;
	realParams["selectPathCdfThreshold"]=&selectPathCdfThreshold;
	realParams["latlngOffsetRange"]=&latlngOffsetRange;
	realParams["simNetworkScale"]=&simNetworkScale;
	realParams["simDelayatIntersection"]=&simDelayatIntersection;
	realParams["pointProbaPower"]=&pointProbaPower;
	realParams["calStrangeSpeedRatio"]=&calStrangeSpeedRatio;
	realParams["selectImportantDDRCdf"]=&selectImportantDDRCdf;
	realParams["minDomainDDRCdf"]=&minDomainDDRCdf;
	realParams["uTurnAngle"]=&uTurnAngle;
	realParams["leftTurnAngle"]=&leftTurnAngle;
	realParams["rightTurnAngle"]=&rightTurnAngle;
	realParams["straightTurnAngle"]=&straightTurnAngle;
	realParams["maxGapDistance"]=&maxGapDistance;
	realParams["maxGapTime"]=&maxGapTime;
	realParams["initialSearchRadius"]=&initialSearchRadius;
	realParams["searchRadiusIncrement"]=&searchRadiusIncrement;
	realParams["arcFreeFlowSpeed"]=&arcFreeFlowSpeed;
	realParams["calStrangeSpeedVarianceA"]=&calStrangeSpeedVarianceA;
	realParams["calStrangeSpeedVarianceB"]=&calStrangeSpeedVarianceB;
	realParams["calStrangeSpeedVarianceB"]=&calStrangeSpeedVarianceB;
	realParams["maxMotorSpeed"]=&maxMotorSpeed;
	realParams["zeroSpeedProba0"]=&zeroSpeedProba0;
	realParams["zeroSpeedProba1"]=&zeroSpeedProba1;
	realParams["zeroSpeedProba2"]=&zeroSpeedProba2;
	realParams["pZeroLambda"]=&pZeroLambda;
	realParams["pZeroSpeedRatio"]=&pZeroSpeedRatio;
	realParams["networkAccuracy"]=&networkAccuracy;
	realParams["tmcw"]=&tmcw;
	realParams["tmclambda"]=&tmclambda;
	realParams["tmcmu"]=&tmcsigma;
	realParams["tmcsigma"]=&tmcsigma;
	realParams["routeJoiningQualityThreshold"]=&routeJoiningQualityThreshold;
	realParams["newGpsSamplingInterval"]=&newGpsSamplingInterval;
	
	
	intParams["stepsPriority"]=&stepsPriority;
	intParams["footwayPriority"]=&footwayPriority;
	intParams["cyclewayPriority"]=&cyclewayPriority;
	intParams["primary_linkPriority"]=&primary_linkPriority;
	intParams["trunk_linkPriority"]=&trunk_linkPriority;
	intParams["motorway_linkPriority"]=&motorway_linkPriority;
	intParams["bridlewayPriority"]=&bridlewayPriority;
	intParams["residentialPriority"]=&residentialPriority;
	intParams["unclassifiedPriority"]=&unclassifiedPriority;
	intParams["tertiaryPriority"]=&tertiaryPriority;
	intParams["secondaryPriority"]=&secondaryPriority;
	intParams["primaryPriority"]=&primaryPriority;
	intParams["trunkPriority"]=&trunkPriority;
	intParams["motorwayPriority"]=&motorwayPriority;
	intParams["railwayPriority"]=&railwayPriority;
	intParams["otherRoadTypePriority"]=&otherRoadTypePriority;

	
	intParams["maxDomainSize"]=&maxDomainSize;
	intParams["maxDomainSizeOrig"]=&maxDomainSizeOrig;
	intParams["maxDomainSizeDest"]=&maxDomainSizeDest;
	intParams["maxNbrOfNodesOrig"]=&maxNbrOfNodesOrig;
	intParams["maxNbrOfNodesOrigin"]=&maxNbrOfNodesOrigin;
	intParams["minNbrOfGeneratedPath"]=&minNbrOfGeneratedPath;
	intParams["pathSamplingAlgo"]=&pathSamplingAlgo;
	intParams["maxNumberOfGeneratedPaths"]=&maxNumberOfGeneratedPaths;
	intParams["maxTrialsForRandomWalk"]=&maxTrialsForRandomWalk;
	intParams["randomSeed"]=&randomSeed;
	intParams["simNumberOfTrips"]=&simNumberOfTrips;
	intParams["simNumberOfHorizonArcs"]=&simNumberOfHorizonArcs;
	intParams["biogemeEstimationDraws"]=&biogemeEstimationDraws;
	intParams["nbrOfIntegrationCalls"]=&nbrOfIntegrationCalls;
	intParams["selectBestPaths"]=&selectBestPaths;
	intParams["selectWorstPaths"]=&selectWorstPaths;
	intParams["selectShortestPaths"]=&selectShortestPaths;
	intParams["doSimulation"]=&doSimulation;
	intParams["maxStrangeHeading"]=&maxStrangeHeading;
	intParams["minGeneratedInterMediatePath"]=&minGeneratedInterMediatePath;
	intParams["selectDDRByDistance"]=&selectDDRByDistance;
	intParams["minDomainSize"]=&minDomainSize;
	intParams["minNbrOfStartNodes"]=&minNbrOfStartNodes;
	intParams["maxNbrOfCandidates"]=&maxNbrOfCandidates;
	intParams["minNbrOfGpsPerSegment"]=&minNbrOfGpsPerSegment;
	intParams["doMapMatching"]=&doMapMatching;
	intParams["exportDDR"]=&exportDDR;
	intParams["doSensitivityAnalysis"]=&doSensitivityAnalysis;

	intParams["doProbabilisticMapMatching"]=&doProbabilisticMapMatching;
	intParams["newGpsSamplingIntervalTestBase"]=&newGpsSamplingIntervalTestBase;
        intParams["repeatRuns"] = &repeatRuns;	

	strParams["gpsIcon"]=&gpsIcon;
	strParams["gpsIconScale"]=&gpsIconScale;
	strParams["gpsIconColor"]=&gpsIconColor;
	strParams["pathLineColor"]=&pathLineColor;
	strParams["pathLineWidth"]=&pathLineWidth;
	strParams["ddrLineColor"]=&ddrLineColor;
	strParams["ddrLineWidth"]=&ddrLineWidth;
	
	strParams["lengthUnit"]=&lengthUnit;
	
	strParams["selectPathCte"]=&selectPathCte;
	strParams["algoInSelection"]=&algoInSelection;
	strParams["integrationMethod"]=&integrationMethod;
	strParams["lowSpeedAlgo"]=&lowSpeedAlgo;
	strParams["dataDirectory"]=&dataDirectory;

        strParams["osm_xpi_url"]=&osm_xpi_url;

        strParams["OsmNetworkFileName"]=&OsmNetworkFileName;
        
        strParams["resultPath"]=&resultPath;

        strParams["SAResultPath"]=&SAResultPath;
        strParams["SAType"]=&SAType;
        strParams["SAPathFolder"]=&SAPathFolder;

	for(map<patString,patReal*>::iterator realIter = realParams.begin();
			realIter!=realParams.end();
			++realIter){
		patString name= realIter->first;
		patReal* value = realIter->second;
		//DEBUG_MESSAGE(name);
		if(getParam(name,value)!=patTRUE){
		stringstream str ;
		str << "Error reading param "<<name ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe());
		return  ;
		}
		//DEBUG_MESSAGE(name<<": "<<*value);
	}
	
	for(map<patString,patULong*>::iterator intIter = intParams.begin();
			intIter != intParams.end();
			++intIter){
		patString name= intIter->first;
		patULong* value = intIter->second;
		//DEBUG_MESSAGE(name);
		
		if(getParam(name,value)!=patTRUE){
		stringstream str ;
		str << "Error reading param "<<name ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe());
		return  ;
		}
		//DEBUG_MESSAGE(name<<": "<<*value);
	}
	for(map<patString,patString*>::iterator strIter = strParams.begin();
			strIter!=strParams.end();
			++strIter){
		patString name= strIter->first;
		patString* value = strIter->second;
		//DEBUG_MESSAGE(name);
		
		if(getParam(name,value)!=patTRUE){
		stringstream str ;
		str << "Error reading param "<<name ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe());
		return  ;
		}
		//DEBUG_MESSAGE(name<<": "<<*value);
	}
}
