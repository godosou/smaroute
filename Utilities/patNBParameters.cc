#include "patDisplay.h"
#include "patNBParameters.h"
#include "patErrMiscError.h"
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

bool patNBParameters::readFile(patString fileName,patError*& err){

DEBUG_MESSAGE("parsing config file:"<<fileName);
	xml::init xmlinit ;  
  
  xml::tree_parser parser(fileName.c_str(),true); 
  xml::document &doc = parser.get_document(); 
  doc.save_to_file ("t.xml", 0);
  if (!parser) {
    stringstream str ;
    str << "Error while parsing " << fileName ;
    err = new patErrMiscError(str.str()) ;
    WARNING(err->describe());
    return false ;
  }

  xml::node theRoot = doc.get_root_node() ;
	
  xml::node::node_type theType = theRoot.get_type() ;
	
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
			xml::nodes_view params(mIter->elements("param"));
			unsigned long k=0;
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
					
				setParam(pName,pValue,pType);
				}
	
				
			}

	
		}
		
	}
	setTrafficModelParam();
  return true ;
}
bool patNBParameters::setParam(patString name,patString value,patString type){
	pair<patString, pair<patString,patString> > el = pair<patString,pair<patString,patString> >(name,pair<patString,patString>(value,type)) ;
	if(params.insert(el).second){
		return true;
	}
	return false;
}
bool patNBParameters::setTrafficModelParam(){
	getParam(patString("CARw"),&CAR_param.w);
	getParam(patString("CARlambda"),&CAR_param.lambda);
	getParam(patString("CARmu"),&CAR_param.mu);
	getParam(patString("CARsigma"),&CAR_param.sigma);
	CAR_param.mode = TransportMode(CAR);

	getParam(patString("BUSw"),&BUS_param.w);
	getParam(patString("BUSlambda"),&BUS_param.lambda);
	getParam(patString("BUSmu"),&BUS_param.mu);
	getParam(patString("BUSsigma"),&BUS_param.sigma);
	BUS_param.mode = TransportMode(BUS);

	getParam(patString("WALKw"),&WALK_param.w);
	getParam(patString("WALKlambda"),&WALK_param.lambda);
	getParam(patString("WALKmu"),&WALK_param.mu);
	getParam(patString("WALKsigma"),&WALK_param.sigma);
	WALK_param.mode = TransportMode(WALK);

	getParam(patString("BIKEw"),&BIKE_param.w);
	getParam(patString("BIKElambda"),&BIKE_param.lambda);
	getParam(patString("BIKEmu"),&BIKE_param.mu);
	getParam(patString("BIKEsigma"),&BIKE_param.sigma);
	BIKE_param.mode = TransportMode(BIKE);

	getParam(patString("TRAINw"),&TRAIN_param.w);
	getParam(patString("TRAINlambda"),&TRAIN_param.lambda);
	getParam(patString("TRAINmu"),&TRAIN_param.mu);
	getParam(patString("TRAINsigma"),&TRAIN_param.sigma);
	TRAIN_param.mode = TransportMode(TRAIN);

	getParam(patString("METROw"),&METRO_param.w);
	getParam(patString("METROlambda"),&METRO_param.lambda);
	getParam(patString("METROmu"),&METRO_param.mu);
	getParam(patString("METROsigma"),&METRO_param.sigma);
	METRO_param.mode = TransportMode(METRO);

}
bool patNBParameters::getParam(patString name, double* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "float"){
		return false;
	}
	else{
		*value = atof(pIter->second.first.c_str());
		return true;
	}
}


bool patNBParameters::getParam(patString name, unsigned long* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "int"){
		return false;
	}
	else{
		*value = atol(pIter->second.first.c_str());
		return true;
	}
}


bool patNBParameters::getParam(patString name, patString* value){
	map<patString, pair<patString,patString> >::iterator pIter = params.find(name);
	if (pIter==params.end()||pIter->second.second != "string"){
		return false;
	}
	else{
		*value = pIter->second.first;
		return true;
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
	map<patString,double*> realParams;
	map<patString,unsigned long*> intParams;
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


	realParams["boundingBoxLeftUpLatitude"]=&boundingBoxLeftUpLatitude;
	realParams["boundingBoxLeftUpLongitude"]=&boundingBoxLeftUpLongitude;
	realParams["boundingBoxRightBottumLatitude"]=&boundingBoxRightBottumLatitude;
	realParams["boundingBoxRightBottumLongitude"]=&boundingBoxRightBottumLongitude;

	realParams["walkNetworkMinSpeed"]=&walkNetworkMinSpeed;
	realParams["walkNetworkMaxSpeed"]=&walkNetworkMaxSpeed;
	realParams["trainNetworkMinSpeed"]=&trainNetworkMinSpeed;
	realParams["trainNetworkMaxSpeed"]=&trainNetworkMaxSpeed;
	realParams["busNetworkMinSpeed"]=&busNetworkMinSpeed;
	realParams["busNetworkMaxSpeed"]=&busNetworkMaxSpeed;
	realParams["carNetworkMinSpeed"]=&carNetworkMinSpeed;
	realParams["carNetworkMaxSpeed"]=&carNetworkMaxSpeed;
	realParams["bikeNetworkMinSpeed"]=&bikeNetworkMinSpeed;
	realParams["bikeNetworkMaxSpeed"]=&bikeNetworkMaxSpeed;
	

	realParams["minPathTravelTimeRatio"]=&minPathTravelTimeRatio;

	realParams["minChangeLengthBackToTheSame"]=&minChangeLengthBackToTheSame;


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

	for(map<patString,double*>::iterator realIter = realParams.begin();
			realIter!=realParams.end();
			++realIter){
		patString name= realIter->first;
		double* value = realIter->second;
		//DEBUG_MESSAGE(name);
		if(getParam(name,value)!=true){
		stringstream str ;
		str << "Error reading param "<<name ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe());
		return  ;
		}
		//DEBUG_MESSAGE(name<<": "<<*value);
	}
	
	for(map<patString,unsigned long*>::iterator intIter = intParams.begin();
			intIter != intParams.end();
			++intIter){
		patString name= intIter->first;
		unsigned long* value = intIter->second;
		//DEBUG_MESSAGE(name);
		
		if(getParam(name,value)!=true){
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
		
		if(getParam(name,value)!=true){
		stringstream str ;
		str << "Error reading param "<<name ;
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe());
		return  ;
		}
		//DEBUG_MESSAGE(name<<": "<<*value);
	}
}