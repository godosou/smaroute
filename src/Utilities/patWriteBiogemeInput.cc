#include "patWriteBiogemeInput.h"
#include <iomanip>
#include "patOdJ.h"
#include <vector>
#include <set>
#include <list>
#include <map>
#include "patNode.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patPathJ.h"
#include "patTraveler.h"
#include "patSample.h"
#include "patPathSampling.h"

#include "patObservation.h"
#include "patErrMiscError.h"
#include "patPathDDR.h"
#include "patDisplay.h"
#include "patNBParameters.h"
#include "patPathSampling.h"
patWriteBiogemeInput::patWriteBiogemeInput( patSample* sample,patNetwork* network,patError*& err):
	theSample(sample),
	theNetwork(network)
{
	computeMaxPathNumber(err);
	if (maxPathNumber == 0){
		stringstream str ;
		str << "No path connects data";
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return  ;
	}
	
	if (maxPathNumber == 1){
		stringstream str ;
		str << "At most one path connect ODs";
		err = new patErrMiscError(str.str()) ;
		WARNING(err->describe()) ;
		return  ;
	}
	
	DEBUG_MESSAGE("There are at most " << maxPathNumber << " alternatives") ;
	
}

void patWriteBiogemeInput::computeMaxPathNumber(patError*& err){
	maxPathNumber = 0;
	
	set<patTraveler>* theTravelerSet = theSample->getAllTravelers();
	for(set<patTraveler>::iterator travelerIter=theTravelerSet->begin();
		 travelerIter!=theTravelerSet->end();
		++travelerIter){
			patTraveler* aTraveler=const_cast<patTraveler*>(&(*travelerIter));
		
			set<patObservation>* ObsSet = aTraveler->getAllObservations();
			
			for(set<patObservation>::iterator obsIter = ObsSet->begin();
				obsIter!=ObsSet->end();
				++obsIter){
					map<patPathJ*, patPathSampling>* AllChoiceSet = const_cast<patObservation*>(&(*obsIter))->getChoiceSet();
					for(map<patPathJ*, patPathSampling>::iterator pathIter = AllChoiceSet->begin();
					pathIter!=AllChoiceSet->end();
					++pathIter){
						unsigned long curr=pathIter->second.getChoiceSet()->size();
						if(curr>maxPathNumber){
							maxPathNumber=curr;
						}
					}
					
			}
	}
	
}

void patWriteBiogemeInput::computeColumNumber(){
/*
	patULong nbrColumns = 5; //id, AggLast, AggWeight, choiceRP, ChosenPathId
	nbrColumns += 2; //O-D
	nbrColumns += maxPathNumber * nbrOfArcCharacteristics;// arc characteristics
	nbrColumns += maxPathNumber * nbrOfPathCharacteristics;// arc characteristics
	nbrColumns += maxPathNumber * 3; // path id, availability and path size
	return nbrColumns;
	*/
}

void patWriteBiogemeInput::writeSampleHeader(const patString& fileName){
DEBUG_MESSAGE("Create " << fileName) ;
	ofstream sampleFile(fileName.c_str(),ios::app) ;
	
	sampleFile << "id" ;
	sampleFile << '\t' << "AggLast" ;
	sampleFile << '\t' << "AggWeight" ;
	sampleFile << '\t' << "AggWeightNormalized" ;
	sampleFile << '\t' << "AggWeightSimple" ;
	sampleFile << '\t' << "AggWeightSimpleNormalized" ;
	sampleFile << '\t' << "AggWeightRaw" ;
	sampleFile << '\t' << "AggWeightRawNormalized" ;
	sampleFile << '\t' << "choiceRP" ;
	sampleFile << '\t' << "ChosenPathId" ;
	sampleFile << '\t' << "Orig" ;
	sampleFile << '\t' << "Dest" ;
	sampleFile << '\t' << "userId";
	sampleFile << '\t' << "tripId";
	//path spec
	for (unsigned long alt = 1 ; alt <= maxPathNumber ; ++alt) {
		// First, the pathID
		sampleFile << '\t' << "pathId" << alt ;
		sampleFile << '\t' << "length" << alt ;
		sampleFile << '\t' << "pathSize" << alt ;
		sampleFile << '\t' << "ExtendedPathSize" << alt ;
		sampleFile << '\t' << "signals" << alt ;
		sampleFile << '\t' << "leftTurns" << alt ;
		sampleFile << '\t' << "rightTurns" << alt ;
		sampleFile << '\t' << "straightTurns" << alt ;
		sampleFile << '\t' << "uTurns" << alt ;
		sampleFile << '\t' << "perPrimaryLinkRd" <<alt;
		sampleFile << '\t' << "perTrunkLinkRd" <<alt;
		sampleFile << '\t' << "perMotorwayLinkRd" <<alt;
		sampleFile << '\t' << "perBridleRd" <<alt;
		sampleFile << '\t' << "perResidentialRd" <<alt;
		sampleFile << '\t' << "perUnclassifiedRd" <<alt;
		sampleFile << '\t' << "perTertiaryRd" <<alt;
		sampleFile << '\t' << "perSecondaryRd" <<alt;
		sampleFile << '\t' << "perPrimaryRd" <<alt;
		sampleFile << '\t' << "perTrunkRd" <<alt;
		sampleFile << '\t' << "perMotorwayRd" <<alt;
		sampleFile << '\t' << "correction" <<alt;
		sampleFile << '\t' << "avail" <<alt;
	/*
		// Then the arc characteristics
		for (vector<patString>::iterator j = edgeCharacteristics.begin() ;
					j != edgeCharacteristics.end() ;
					++j) {
			sampleFile << '\t' << "x" << alt << "_" << *j  ;
		}
		
		// Then the paths characteristics
		for (vector<patString>::iterator j = pathCharacteristics.begin() ;
					j != pathCharacteristics.end() ;
					++j) {
			sampleFile << '\t' << "x" << alt << "_" << *j  ;
		}
		
		// And the path sizes
		sampleFile << "\tpathSize" << alt ;
		// Finally the availability
		sampleFile << "\tavail" << alt ;

		}*/
	}
		sampleFile << endl ;
sampleFile.close();
	}
void patWriteBiogemeInput::writeSampleFile(const patString& fileName){
	DEBUG_MESSAGE("Create " << fileName) ;
	ofstream sample(fileName.c_str()) ;
	sample << setprecision(7) << setiosflags(ios::scientific) ;

	writeSampleHeader(fileName);
	writeData(fileName);
	sample.close();
	
}
void patWriteBiogemeInput::writeSpecFile(const patString& fileName){
	DEBUG_MESSAGE("Create " << fileName) ;
	ofstream spec(fileName.c_str()) ;
	patAbsTime now ;
	now.setTimeOfDay() ;
	
	spec << "//This file has automatically been generated by"<<endl;
	spec << "//bioroute"<<endl;
	spec << "// " << now.getTimeString(patTsfFULL) << endl ;
	spec << "// It complies with the syntax of BIOGEME Version 1.8" << endl ;
	spec << ""<<endl;
	
	spec <<"[Choice]"<<endl;
	spec <<"choiceRP"<<endl;
	spec << ""<<endl;
	
	spec <<"[AggregateLast]"<<endl;
	spec <<"AggLast"<<endl;
	spec << ""<<endl;
	
	spec <<"[AggregateWeight]"<<endl;
	spec <<"AggWeight"<<endl;
	spec << ""<<endl;
	
	
	spec <<"[ModelDescription]"<<endl;
	spec <<"\"path size\""<<endl;
	spec <<endl;
	
	spec <<"[Beta]"<<endl;
	spec << "// WARNING: this section very likely needs to be updated by hand" << endl ;
	spec << "// A generic parameter for each path attribute is defined here." << endl ;
	spec << "// Parameters capturing socio-economic attributes must be added by hand." << endl ;
	spec << "// Note that bounds are arbitrary. You may need to change them by hand" << endl ;
	spec << "// Name Value  LowerBound UpperBound  status (0=variable, 1=fixed)" << endl ;
	patString betaPathSize("BETA_PathSize") ;
	/*
	spec << betaPathSize << "\t0.0\t-10.0\t10.0\t0" << endl ;
	for (vector<patString>::iterator i = arcCharacteristics.begin() ;
				i != edgeCharacteristics.end() ;
				++i) {
		spec << "BETA_" << *i << '\t' ;
		spec << "0.0\t-10.0\t10.0\t0" << endl ;
	}

	for (vector<patString>::iterator i = pathCharacteristics.begin() ;
				i != pathCharacteristics.end() ;
				++i) {
		spec << "BETA_" << *i << '\t' ;
		spec << "0.0\t-10.0\t10.0\t0" << endl ;
	}
	*/
	
	spec << "BETA_PathSize"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	spec << "BETA_length"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	spec << "BETA_signals"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	spec << "BETA_LeftTurn"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	spec << "BETA_Primary"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	//spec << "ZERO"<<"\t0.0\t-1000.0\t1000.0\t1" << endl ;
	//spec << "SIGMA"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;
	
	spec << "" << endl ;
	
	spec << "" << endl ;
	spec << "[Utilities]" << endl ;
	spec << "// Id Name  Avail  linear-in-parameter expression (beta1*x1 + beta2*x2 + ... )" << endl ;
	
	for (unsigned long alt = 1 ; alt <= maxPathNumber ; ++alt) {
		spec << alt << '\t' ;// id
		spec << "alt" << alt << '\t' ;//alt
		spec << "avail" << alt << '\t' ;//avaiblability
		spec << betaPathSize << " * " << "pathSize" << alt ;
		spec <<" + ";
		spec << "BETA_length" << " * " << "length" << alt;
		spec <<" + ";
		spec << "BETA_signals" << " * " << "signals" << alt;
		spec <<" + ";
		spec << "BETA_LeftTurn" << " * " << "leftTurns" << alt;
		
		/*
		for (vector<patString>::iterator j = edgeCharacteristics.begin() ;
					j != edgeCharacteristics.end() ;
					++j) {
			spec << " + " 
			<< "BETA_" 
			<< *j 
			<< " * " 
			<< "x" 
			<< alt 
			<< "_" 
			<< *j ;
		}
    
		for (vector<patString>::iterator j = pathCharacteristics.begin() ;
					j != pathCharacteristics.end() ;
					++j) {
			spec << " + " ;
			spec << "BETA_" << *j << " * " << "x" << alt << "_" << *j ;
		}
		*/
		spec << endl ;
	}
	
	spec << ""<<endl;
	
	spec << "[PanelData]"<< endl;
	spec << "userId"<<endl;
	spec << ""<<endl;
	spec << "[Draws]"<< endl;
	spec << patNBParameters::the()->biogemeEstimationDraws<<endl;
	spec << ""<<endl;
	
	spec << "[Expressions] " << endl ;
	spec << "// Define here arithmetic expressions for name that are not directly " << endl ;
	spec << "// available from the data" << endl ;
	spec << "one = 1" << endl ;
	spec << "" << endl ;
	spec << "[Group]" << endl ;
	spec << "$NONE" << endl ;
	spec << "" << endl ;
	spec << "[Exclude]" << endl ;
	spec << "$NONE" << endl ;
	spec << "" << endl ;
	spec << "[Model]" << endl ;
	spec << "// Currently, only $MNL (multinomial logit), $NL (nested logit) and $CNL" << endl ;
	spec << "// (cross-nested logit) are valid keywords" << endl ;
	spec << "$MNL" << endl ;
	spec << "//$NL" << endl ;
	spec << "//$CNL" << endl ;
	spec << endl ;
	spec << "[Scale]" << endl ;
	spec << "// The sample can be divided in several groups of individuals. The" << endl ;
	spec << "//utility of an individual in a group will be multiplied by the scale factor" << endl ;
	spec << "//associated with the group." << endl ;
	spec << "" << endl ;
	spec << "// Group_number  scale LowerBound UpperBound status" << endl ;
	spec << "$NONE" << endl ;
	spec << endl ;
	spec << "[Ratios] " << endl ;
	spec << "// List of ratios of estimated coefficients that must be produced in" << endl ;
	spec << "// the output. The most typical is the value-of-time." << endl ;
	spec << "// Numerator   Denominator  Name" << endl ;
	spec << "$NONE" << endl ;
	spec.close() ;

}
struct observation{
unsigned long id;
unsigned long AggLast;
double AggWeight;
double ChoiceRP;
double ChosenPathId;
unsigned long Orig;
unsigned long Dest;
};
void patWriteBiogemeInput::writeData(const patString& fileName){
DEBUG_MESSAGE("Create " << fileName) ;
	ofstream sampleFile(fileName.c_str(),ios::app) ;
	sampleFile << setprecision(7) << setiosflags(ios::scientific) ;
	set<patTraveler>* travelers = const_cast<patSample*>(theSample)->getAllTravelers();
//	DEBUG_MESSAGE("there are "<<travelers->size()<<" travelers");
	for (set<patTraveler>::iterator travelerIter = travelers->begin();
					travelerIter!=travelers->end();
					travelerIter++){
		set<patObservation>* obs = const_cast<patTraveler*>(&(*travelerIter))->getAllObservations();
	//DEBUG_MESSAGE("there are "<<obs->size()<<" observations");
						patError* err;
		for(set<patObservation>::iterator obsIter = obs->begin();
					obsIter!=obs->end();
					obsIter++){
				unsigned long i=0;
				map<patPathJ*,vector<double> >* ddrs = const_cast<patObservation*>(&(*obsIter))->getPathDDRsReal();
			map<patPathJ*,vector<double> >* ddrsNormalized = const_cast<patObservation*>(&(*obsIter))->getPathDDRsRealNormalized();
			
				if(ddrs->empty()){
					DEBUG_MESSAGE("a null observation");
					continue;
				}
				map<patPathJ*,vector<double> >::iterator endIter = ddrs->end();
				endIter--;
				//DEBUG_MESSAGE("there are "<<ddrs->size()<<" paths");
				for (map<patPathJ*,vector<double> >::iterator pathIter = ddrs->begin();
							pathIter!=ddrs->end();
							pathIter++){
						if(pathIter->second[0]==0.0){
							continue;
						}
						sampleFile<<i;//id
						if (pathIter==endIter){
							sampleFile<<'\t'<<1;//AggLast
						
						}
						else{
							sampleFile<<'\t'<<0;//AggLast
						
						}
						sampleFile<<'\t'<<pathIter->second[0];//AggWeight time space
					sampleFile<<'\t'<<(*ddrsNormalized)[pathIter->first][0];
						sampleFile<<'\t'<<pathIter->second[1];//AggWeight simple
					sampleFile<<'\t'<<(*ddrsNormalized)[pathIter->first][1];
						sampleFile<<'\t'<<pathIter->second[2];//AggWeight raw
					sampleFile<<'\t'<<(*ddrsNormalized)[pathIter->first][2];
						patOd* od = pathIter->first->getOd();
						patPathSampling* thePathSampling=const_cast<patObservation*>(&(*obsIter))->getPathSampling(pathIter->first);
						
						set<patPathJ*>* choiceSet = thePathSampling->getChoiceSet();
						//DEBUG_MESSAGE("choice set size:"<<choiceSet->size());
						unsigned long chosenRP = 1;
						
						
						for (set<patPathJ*>::iterator choiceIter = choiceSet->begin();
								choiceIter!=choiceSet->end();
								++choiceIter){
							
							if ((*choiceIter)==pathIter->first){
								
								break;
							}
							
							chosenRP++;
							
						}
						if(chosenRP>choiceSet->size()){
							WARNING("chosen path is not in choice set");
						}
						sampleFile<<'\t'<<chosenRP;
						sampleFile<<'\t'<<pathIter->first->getId();
						sampleFile<<'\t'<<od->getOrigin()->userId; //orig
						sampleFile<<'\t'<<od->getDestination()->userId;//dest
						sampleFile<<'\t'<<const_cast<patTraveler*>(&(*travelerIter))->getId();//userid
						sampleFile<<'\t'<<const_cast<patObservation*>(&(*obsIter))->getId();//userid
						
						unsigned long j=1;
						for (set<patPathJ*>::iterator choiceIter = choiceSet->begin();
								choiceIter!=choiceSet->end();
							++choiceIter){
								patPathJ* pathAlter=const_cast<patPathJ*>(*choiceIter);
								if(thePathSampling->getPathReplicates(pathAlter,err)==0 && pathAlter!=pathIter->first){
									WARNING("a path is in the choice set but not sampled, nor the chosen path")
									continue;
								}
							sampleFile<<'\t'<< pathAlter->getId();//path id
							if(patNBParameters::the()->lengthUnit=="km"){
								sampleFile<<'\t'<< pathAlter->getLenth()/1000.0;//length
							
							}
							else{
								sampleFile<<'\t'<< pathAlter->getLenth();//length
							
							}
							sampleFile<<'\t'<< thePathSampling->getPathPS(pathAlter,err);
							sampleFile<<'\t'<< thePathSampling->getPathEPS(pathAlter,err);//path size							
							sampleFile<<'\t'<< pathAlter->getTrafficSignals();//traffic signals
							
							sampleFile<<'\t'<< pathAlter->getLeftTurns();//left turn
							sampleFile<<'\t'<< pathAlter->getRightTurns();//right turn
							sampleFile<<'\t'<< pathAlter->getStraightTurns();//straight turn
							sampleFile<<'\t'<< pathAlter->getUTurns();//u turn
							
							
							sampleFile<<'\t'<< pathAlter->getPerPrimaryLinkRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerTrunkLinkRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerMotorwayLinkRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerBridleRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerResidentialRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerUnclassifiedRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerTertiaryRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerSecondaryRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerPrimaryRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerTrunkRd();//u turn
							sampleFile<<'\t'<< pathAlter->getPerMotorwayRd();//u turn
							
							sampleFile<<'\t'<<log(thePathSampling->getPathReplicates(pathAlter,err))-thePathSampling->getPathRwLogProba(pathAlter,err);//correction
							sampleFile<<'\t'<<"1";//availability
							j++;
						}
						
					for (;j<=maxPathNumber;j++){
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";	
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"9999";
							sampleFile<<'\t'<<"0";
						}
						i++;
		sampleFile<<endl;
}
		}
			
	}
			
	DEBUG_MESSAGE("Finish writing biogeme input file.");
	sampleFile.close();		
	}
	