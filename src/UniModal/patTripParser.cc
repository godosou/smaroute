#include "patType.h"
#include "patConst.h"
#include<iostream>
#include<sstream>
#include<fstream>

#include <time.h>
#include "patTripParser.h"
#include "patDisplay.h"
#include "patPathDDR.h"
#include "patGpsPoint.h"
#include "patNetwork.h"
#include "patObservation.h"
#include "patTraveler.h"
#include <vector>
#include <list>
#include "patArc.h"
#include "patSample.h"
#include "patPathJ.h"
#include "patOdJ.h"
#include "patTripGraph.h"
#include "patPathProbaAlgoV4.h"
#include "patError.h"
#include "patArcTransition.h"
#include "patGenPathFromArcTranSet.h"
#include "patPathDevelop.h"
#include "patMapMatching.h"
#include "patMapMatchingV2.h"
#include "patReadPathFromKML.h"
#include "patNBParameters.h"
#include "patCalculateProbaForPaths.h"
#include "patErrMiscError.h"
//#include <boost/filesystem.hpp>
//using namespace boost::filesystem;
#include<sys/stat.h>
#include<sys/types.h>

patTripParser::patTripParser() {
}

patTripParser::patTripParser(patTraveler* theTraveler, unsigned long& theTripId,
		unsigned long& theStartTime) :
		generatedObservation(theTraveler, theTripId) {
	generatedObservation.setStartTime(theStartTime);

}

void patTripParser::setEnviroment(patSample* sample, patNetwork* network) {
	theSample = sample;
	theNetwork = network;
}

void patTripParser::setMapBounds(double minLat, double maxLat, double minLon,
		double maxLon) {
	generatedObservation.setMapBounds(minLat, maxLat, minLon, maxLon);
}

void patTripParser::inputGpsTrack(vector<patGpsPoint>* gpsTrack) {
	gpsSequence = *gpsTrack;
}

bool patTripParser::addPoint(patGpsPoint theGpsPoint) {
	if (theGpsPoint.getHorizonAccuracy()
			< patNBParameters::the()->maxHorizonAccuracy) {
		gpsSequence.push_back(theGpsPoint);
	} else {
		DEBUG_MESSAGE(
				"gps " << theGpsPoint.getTimeStamp() << " too inaccurate;");
	}
	return true;
}

unsigned long patTripParser::firstValidGps() {
	unsigned long i = 0;
	for (; i < gpsSequence.size(); ++i) {
		unsigned long next = i + 1;
		if (i == (gpsSequence.size() - 1)) {
			next = i;
		}
		gpsSequence[i].genInitDDRV2(&(gpsSequence[next]), theNetwork,
				tripGraph);
		if (gpsSequence[i].getLinkDDR()->empty()) {
			DEBUG_MESSAGE("No domain at:" << gpsSequence[i].getTimeStamp());
		} else {
			break;
		}
	}

	return i;
}

void patTripParser::genDDR(vector<list<pair<patArc*, unsigned long> > >* adjList,
set<patArcTransition>* arcTranSet

) {
	DEBUG_MESSAGE("Raw points:" << gpsSequence.size());
	DEBUG_MESSAGE("first point. " << gpsSequence[0].getTimeStamp());
	patGpsPoint* prevGpsPoint;
	patGpsPoint* prevNormalGpsPoint;

	unsigned long first_valid = firstValidGps(); //get first valid gps point

	DEBUG_MESSAGE(
			"First valid point:" << first_valid << ","
					<< gpsSequence[first_valid].getTimeStamp());
	if (first_valid == gpsSequence.size()) {
		return;
	}
	prevGpsPoint = &(gpsSequence[first_valid]);
	prevNormalGpsPoint = &(gpsSequence[first_valid]);
	pathDevelop->init(prevNormalGpsPoint);
	vector<patGpsPoint*> lowSpeedGpsPoints;
	double prevHeading = gpsSequence[first_valid].getHeading();
	unsigned long nbrOfStrangeHeading = 0;
	for (unsigned long i = first_valid + 1; i < gpsSequence.size(); ++i) {
		DEBUG_MESSAGE(i + 1 << "," << gpsSequence[i]);
		if (gpsSequence[i].getType() != patString("normal_speed")
				&& i != gpsSequence.size() - 1) {

			lowSpeedGpsPoints.push_back(&(gpsSequence[i]));
		} else {
			gpsSequence[i].genSegmentDDRV3(prevNormalGpsPoint, theNetwork,
					adjList, pathDevelop, &gpsSequence, &lowSpeedGpsPoints);
			DEBUG_MESSAGE(
					"Link DDR number:" << gpsSequence[i].getLinkDDR()->size());
			if (&(gpsSequence[i]) == pathDevelop->lastGpsPoint()) {
				DEBUG_MESSAGE(
						"Found Domain at (" << i + 1 << ") "
								<< gpsSequence[i].getTimeStamp());
				lowSpeedGpsPoints = vector<patGpsPoint*>();
				prevNormalGpsPoint = &(gpsSequence[i]);
			} else {
				DEBUG_MESSAGE("No domain at:" << gpsSequence[i].getTimeStamp());
			}

		}
	}
	if (!lowSpeedGpsPoints.empty()) {
		pathDevelop->lowSpeedPoints(&lowSpeedGpsPoints,
				&(prevNormalGpsPoint->getDomainSet()), theNetwork);
	}

}

bool patTripParser::addRealPath(list<unsigned long> listOfNodes) {
	list<unsigned long>::iterator nodeIter = listOfNodes.begin();
	unsigned long upNodeId = *nodeIter;

	nodeIter++;
	list<patArc*> listOfArcs;
	for (; nodeIter != listOfNodes.end(); ++nodeIter) {
		unsigned long downNodeId = *nodeIter;

		patArc* arc = theNetwork->getArcFromNodesUserId(upNodeId, downNodeId);
		if (arc == NULL) {
			return false;
		}
		realPath.addArcToBack(arc);
		upNodeId = downNodeId;
	}
	return true;
}

void patTripParser::cleanGpsData() {
	double prevHeading = gpsSequence.front().getHeading();
	double prevSpeed = gpsSequence.front().getSpeed();
	unsigned long nbrOfStrangeHeading = 0;
	unsigned long nbr = 0;
	double speedSum = 0.0;
	for (unsigned long i = 1; i < gpsSequence.size(); ++i) {
		if (prevHeading == gpsSequence[i].getHeading()
				&& prevSpeed == gpsSequence[i].getSpeed()) {
			nbrOfStrangeHeading += 1;
			if (nbrOfStrangeHeading
					> patNBParameters::the()->maxStrangeHeading) {
				//DEBUG_MESSAGE("strange heading data&heading at ("<<i+1<<"), value: "<<gpsSequence[i].getHeading());
				double newHeading = -10.0;
				double newSpeed;
				if (i == 0) {
					newHeading = gpsSequence[0].calHeading(&gpsSequence[1]);
					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* gpsSequence[0].distanceTo(&gpsSequence[1])
							/ (gpsSequence[1].getTimeStamp()
									- gpsSequence[0].getTimeStamp());

					gpsSequence[0].setHeading(newHeading);
					gpsSequence[0].setSpeed(newSpeed);
					//gpsSequence[0].setSpeedAccuracy(newSpeed * patNBParameters::the()->calStrangeSpeedVarianceA + patNBParameters::the()->calStrangeSpeedVarianceB);

				} else if (i == (gpsSequence.size() - 1) || i == 1) {
					newHeading = gpsSequence[i - 1].calHeading(&gpsSequence[i]);
					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* gpsSequence[i - 1].distanceTo(&gpsSequence[i])
							/ (gpsSequence[i].getTimeStamp()
									- gpsSequence[i - 1].getTimeStamp());
					gpsSequence[i].setSpeed(newSpeed);

					if (i == 1) {
						gpsSequence[0].setHeading(newHeading);
						gpsSequence[0].setSpeed(newSpeed);
						//gpsSequence[0].setSpeedAccuracy(newSpeed * patNBParameters::the()->calStrangeSpeedVarianceA + patNBParameters::the()->calStrangeSpeedVarianceB);

					}

				} else {

					newSpeed = 3.6
							* patNBParameters::the()->calStrangeSpeedRatio
							* gpsSequence[i].distanceTo(&gpsSequence[i + 1])
							/ (gpsSequence[i + 1].getTimeStamp()
									- gpsSequence[i].getTimeStamp());
					gpsSequence[i].setSpeed(newSpeed);
					newHeading = gpsSequence[i].calHeading(&gpsSequence[i - 1],
							&gpsSequence[i + 1]);

				}
				gpsSequence[i].setSpeed(newSpeed);
				//gpsSequence[i].setSpeedAccuracy(newSpeed * 0.3 );

				gpsSequence[i].setHeading(newHeading);

				//DEBUG_MESSAGE("after recalculate heading: "<<gpsSequence[i].getHeading());
				//DEBUG_MESSAGE("after recalculate speed: "<<gpsSequence[i].getSpeed());
			}
		} else {
			nbrOfStrangeHeading = 0;
			prevSpeed = gpsSequence[i].getSpeed();
			prevHeading = gpsSequence[i].getHeading();
		}
	}

}

void patTripParser::doMapMatching(
		vector<list<pair<patArc*, unsigned long> > >* adjList) {

	//patMapMatchingV2* mm = new patMapMatchingV2(gpsSequence, theNetwork, adjList);
	patMapMatching* mm = new patMapMatching(gpsSequence, theNetwork);
	patError * err(NULL);
	patPathJ thePath = mm->run(err);
	DEBUG_MESSAGE("finish map matching");

	delete mm;
	mm = NULL;
	if (err != NULL) {
		DEBUG_MESSAGE("wrong mathcing");
		return;
	}
	DEBUG_MESSAGE(thePath);

	patOdJ tod = thePath.generateOd(theNetwork, err);
	if (err != NULL) {
		WARNING("no od is associated with path" << thePath);
		return;
	}
	patOdJ* theOd = theSample->addOd(tod);
	thePath.assignOd(theOd);
	patPathJ* pathPointer = theOd->addPath(thePath);
	generatedObservation.pathDDRsReal[pathPointer] = vector<double>();
	generatedObservation.pathDDRsReal[pathPointer].push_back(patMaxReal);
	generatedObservation.pathDDRsReal[pathPointer].push_back(patMaxReal);
	generatedObservation.pathDDRsReal[pathPointer].push_back(patMaxReal);
	generatedObservation.pathDDRsReal[pathPointer].push_back(patMaxReal);
	genMapMatchingResult(adjList);
}

vector<patGpsPoint> patTripParser::increaseSamplingInterval(
		double newInterval) {
	if (gpsSequence.size() <= 1) {
		return gpsSequence;
	}
	vector<patGpsPoint> original_sampling_gps = gpsSequence;

	vector<patGpsPoint>::iterator gpsIter = gpsSequence.begin();
	double lastTime = gpsIter->getTimeStamp();
	gpsIter++;
	while (gpsIter != gpsSequence.end()) {
		double currentTime = gpsIter->getTimeStamp();
		if ((currentTime - lastTime) < newInterval) {
			gpsSequence.erase(gpsIter);
			DEBUG_MESSAGE("erased a point at time " << currentTime);
		} else {
			lastTime = currentTime;
			gpsIter++;
		}
	}

	lastTime = gpsSequence.back().getTimeStamp();
	gpsIter = original_sampling_gps.end();

	while (gpsIter != original_sampling_gps.begin()) {
		gpsIter--;
		if (gpsIter->getTimeStamp() != lastTime) {
			original_sampling_gps.erase(gpsIter);
		} else {
			break;
		}
	}

	return original_sampling_gps;

}

void patTripParser::doProbabilisticMapMatching(
		vector<list<pair<patArc*, unsigned long> > >* adjList) {
	setGpsSpeedType();
	DEBUG_MESSAGE("Prepare to generate DDR. ");
	set<patArcTransition>* arcTranSet = new set<patArcTransition>;
	tripGraph = new patTripGraph(theNetwork);
	pathDevelop = new patPathDevelop(theNetwork, arcTranSet, &gpsSequence);
	genDDR(adjList, arcTranSet);
	if (gpsSequence.empty()) {
		DEBUG_MESSAGE("No domain for any gps. ");
		return;
	}
	DEBUG_MESSAGE("Prepare to generate OD . ");
	DEBUG_MESSAGE("Prepare to generate OD DDR. ");
	DEBUG_MESSAGE("Prepare to generate paths. ");
	pathDevelop->lastStage(theSample, &generatedObservation);
	DEBUG_MESSAGE("After clean points:" << gpsSequence.size());
	if (gpsSequence.empty()) {
		return;
	}
	delete pathDevelop;
	pathDevelop = NULL;
	delete arcTranSet;
	arcTranSet = NULL;
	genOd();
	genMapMatchingResult(adjList);
}

void patTripParser::genMapMatchingResult(
		vector<list<pair<patArc*, unsigned long> > >* adjList) {
	DEBUG_MESSAGE("Prepare to write files. ");
	string userIdString, userTripString;
	string kmlFileName;
	stringstream kmlFileNameStream(kmlFileName);
	kmlFileNameStream << patNBParameters::the()->resultPath;
	//create_directories(kmlFileNameStream.str());
	mkdir(kmlFileNameStream.str().c_str(), 0777);
	kmlFileNameStream << "/" << generatedObservation.getTraveler()->getId();
	kmlFileNameStream << "-";
	kmlFileNameStream << generatedObservation.getId();
	if (patNBParameters::the()->doMapMatching == 1) {
		kmlFileNameStream << "-mm";
	}
	kmlFileNameStream << ".kml";
	kmlFileName = kmlFileNameStream.str();
	writeToKML(kmlFileName);
	DEBUG_MESSAGE("A trip have been created. ");
	DEBUG_MESSAGE("normalize probability values");
	generatedObservation.normalizeDDRs();
	//generatedObservation.setOdPathSetByDDRs();
	patError * err(NULL);

	//generatedObservation.sampleChoiceSet(theNetwork,adjList,err);
	generatedObservation.getTraveler()->addObservation(generatedObservation);
}

vector<double> patTripParser::prepareParamsForSA(patString paramName) {

	vector<double> theAlgoParamsVector;
	if (paramName == "networkAccuracy") {
		theAlgoParamsVector.push_back(double(5.0));
		theAlgoParamsVector.push_back(double(10.0));
		theAlgoParamsVector.push_back(double(20.0));
		theAlgoParamsVector.push_back(double(30.0));
		theAlgoParamsVector.push_back(double(40.0));
		theAlgoParamsVector.push_back(double(50.0));

	} else if (paramName == "minPointDDR") {
		theAlgoParamsVector.push_back(double(0.3));
		theAlgoParamsVector.push_back(double(0.4));
		theAlgoParamsVector.push_back(double(0.5));
		theAlgoParamsVector.push_back(double(0.6));
		theAlgoParamsVector.push_back(double(0.65));
		theAlgoParamsVector.push_back(double(0.7));
		theAlgoParamsVector.push_back(double(0.75));
		theAlgoParamsVector.push_back(double(0.8));

	}

	return theAlgoParamsVector;
}

void patTripParser::setPMMAlgoParam(patString paramName, double paramValue) {
	for (unsigned long i = 0; i < gpsSequence.size(); ++i) {
		gpsSequence[i].setAlgoParams(paramName, paramValue);
	}
}

void patTripParser::presentSAResults(patString SAType,
		map<patString, vector<double> > values) {

	string csvFileName;
	stringstream csvFileNameStream(csvFileName);
	csvFileNameStream << patNBParameters::the()->SAResultPath;
	csvFileNameStream << generatedObservation.getTraveler()->getId();
	csvFileNameStream << "-";
	csvFileNameStream << generatedObservation.getId();
	csvFileNameStream << SAType;
	csvFileNameStream << ".csv";
	csvFileName = csvFileNameStream.str();
	ofstream csv(csvFileName.c_str());
	for (map<patString, vector<double> >::iterator iter1 = values.begin();
			iter1 != values.end(); ++iter1) {
		unsigned long nbrv = iter1->second.size();
		if (nbrv == 0) {
			WARNING("no result generated");

		} else {
			csv << iter1->first;
			for (unsigned long i = 0; i < nbrv; ++i) {
				csv << ",";
				csv << iter1->second[i];

			}
		}
		csv << "\n";
		continue;
	}
	csv.close();
}

void patTripParser::doSensitivityAnalysis() {

	patError * err(NULL);
	string kmlFileName;
	stringstream kmlFileNameStream(kmlFileName);
	kmlFileNameStream << patNBParameters::the()->SAPathFolder;
	kmlFileNameStream << generatedObservation.getTraveler()->getId();
	kmlFileNameStream << " -";
	kmlFileNameStream << generatedObservation.getId();
	kmlFileNameStream << ".kml";
	kmlFileName = kmlFileNameStream.str();
	ifstream inp;
	inp.open(kmlFileName.c_str(), ifstream::in);
	inp.close();
	if (inp.fail()) {
		inp.clear(ios::failbit);
		DEBUG_MESSAGE("file " << kmlFileName << " doesn't exist");
		return;

	}
	DEBUG_MESSAGE("read file" << kmlFileName);
	set<patPathJ> pathSet;
	patReadPathFromKML* read_paths = new patReadPathFromKML(kmlFileName,
			theNetwork);
	if (read_paths->parseFile(err) == true) {
		pathSet = read_paths->path_set;

		delete read_paths;
		read_paths = NULL;

	} else {
		delete read_paths;
		read_paths = NULL;

		return;
	}
	patString SAType = patNBParameters::the()->SAType;
	DEBUG_MESSAGE(pathSet.size());
	vector<double> SAParams = prepareParamsForSA(SAType);
	map<patString, vector<double> > probaResults;

	for (unsigned long i = 0; i < SAParams.size(); ++i) {
		double paramValue = SAParams[i];
		DEBUG_MESSAGE("Sensitivity Analaysis:" << SAType << "=" << paramValue);
		setPMMAlgoParam(SAType, paramValue);
		setGpsSpeedType();
		vector<patGpsPoint> gpsSequenceCopy = gpsSequence;
		patCalculateProbaForPaths pathCalculation(theNetwork, &gpsSequenceCopy);
		pathCalculation.fromPaths(pathSet);

		string SAID;
		stringstream SAIDStream(SAID);

		SAIDStream << paramValue;
		SAID = patString(SAIDStream.str());

		probaResults[SAID] = pathCalculation.calculateProbas();

	}
	presentSAResults(SAType, probaResults);
}

void patTripParser::readGPSFromFile(patString& fileName, patError*& err) {
	ifstream inFile;

	inFile.open(fileName.c_str(), ios::in);
	if (!inFile) {
		stringstream str;
		str << "Error while parsing " << fileName;
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
		return;
	}
	DEBUG_MESSAGE("Read file:" << fileName);
	unsigned long currTripId = 1, lastTripId = patBadId; //current trip id
	unsigned long tripNumber = 0, pointNumber = 0;

	unsigned long currTime, currEndTime;
	double currLat, currLon, currSpeed, currHeading, currAccuracyH,
			currAccuracyV, currAccuracyS, currAccuracyHD;

	string line;
	getline(inFile, line);
	unsigned long userId = atol(line.c_str());
	DEBUG_MESSAGE("UserID: " << userId);

	while (getline(inFile, line)) {

		pointNumber++;
		istringstream linestream(line);

		string item;

		getline(linestream, item, ',');
		currTripId = atol(item.c_str());

		getline(linestream, item, ','); //get  time
		currTime = atof(item.c_str());

		getline(linestream, item, ','); //get longitude
		currLon = atof(item.c_str());
		getline(linestream, item, ','); //get latitude
		currLat = atof(item.c_str());
		getline(linestream, item, ','); //altitude
		getline(linestream, item, ','); //get speed
		currSpeed = atof(item.c_str());

		getline(linestream, item, ','); //get
		currHeading = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyH = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyV = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyS = atof(item.c_str());
		getline(linestream, item, ','); //get
		currAccuracyHD = atof(item.c_str());
		patGpsPoint currPoint(userId, currTime, currLat, currLon, currSpeed,
				currAccuracyH, currHeading, currAccuracyV, currAccuracyS,
				currAccuracyHD, NULL);
		addPoint(currPoint);
		currEndTime = currTime;
		lastTripId = currTripId;

	}
	DEBUG_MESSAGE("end time: " << currEndTime);
	++tripNumber;
	DEBUG_MESSAGE(
			"Travler: " << userId << ",Trips:" << tripNumber << ",GPS Points:"
					<< pointNumber);
	cleanGpsData();

	DEBUG_MESSAGE("GPS data cleaned")
	if (gpsSequence.empty()) {
		return;
	}

	generatedObservation.setEndTime(currEndTime);

}
void patTripParser::endOfTrip(vector<list<pair<patArc*, unsigned long> > >* adjList,
unsigned long theEndTime) {
	vector<patGpsPoint> original_sampling_gps = increaseSamplingInterval(
			patNBParameters::the()->newGpsSamplingInterval);
	if (patNBParameters::the()->newGpsSamplingIntervalTestBase == 1) {
		gpsSequence = original_sampling_gps;
	}
	cleanGpsData();
	DEBUG_MESSAGE("GPS data cleaned")
	if (gpsSequence.empty()) {
		return;
	}

	generatedObservation.setEndTime(theEndTime);
	if (patNBParameters::the()->doMapMatching == 1) {

		doMapMatching(adjList);

	} else if (patNBParameters::the()->doProbabilisticMapMatching == 1) {

		doProbabilisticMapMatching(adjList);

	} else if (patNBParameters::the()->doSensitivityAnalysis == 1) {
		DEBUG_MESSAGE("do sensitivity analysis");
		doSensitivityAnalysis();
	}
	/*
	 else if(patNBParameters::the()->doMultiModalMapMatching == 1){
	 DEBUG_MESSAGE("do multi-modal map matching");
	 doSensitivityAnalysis();
	 }
	 */
	else {
		WARNING("Nothing to be do.")
	}

}

void patTripParser::setGpsSpeedType() {

	for (unsigned long i = 0; i < gpsSequence.size(); ++i) {
		gpsSequence[i].setSpeedType();
	}
}

void patTripParser::setOd(patOdJ* theOd) {
	od = theOd;
}

void patTripParser::writeToKML(patString fileName) {

	ofstream kml(fileName.c_str());
	kml.precision(15);
	kml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	kml << "      <kml xmlns=\"http://earth.google.com/kml/2.0\">" << endl;
	kml << "      <Document>" << endl;

	patString theName(fileName);
	replaceAll(&theName, patString("&"), patString("and"));

	kml << "            <name>" << theName << "</name>" << endl;
	kml << "            <description>File created by bioroute</description>"
			<< endl;
	kml << "<Folder>";
	kml << "            <name>paths</name>" << endl;

	map<patOdJ*, map<patPathJ*, vector<double> > > pathDDRsOrderByOd =
			generatedObservation.getPathDDRsOrderByOd_Real();

	vector<double> ddrSum = generatedObservation.calDDRAll();
	for (map<patOdJ*, map<patPathJ*, vector<double> > >::iterator odIter =
			pathDDRsOrderByOd.begin(); odIter != pathDDRsOrderByOd.end();
			++odIter) {
		kml << "<Folder>";
		kml << "<name>" << endl;
		kml << "OD:" << *(odIter->first) << endl;
		kml << "</name>" << endl;

		for (map<patPathJ*, vector<double> >::iterator pathIter =
				odIter->second.begin(); pathIter != odIter->second.end();
				++pathIter) {
			kml << "<Folder>";
			kml << "<description>";

			kml << "ts: " << pathIter->second[0] << "("
					<< pathIter->second[0] / (ddrSum[0]) << "), simple: "
					<< pathIter->second[1] << "("
					<< pathIter->second[1] / (ddrSum[1]) << "), raw:"
					<< pathIter->second[2] << "("
					<< pathIter->second[2] / (ddrSum[2]) << " ), mm "
					<< pathIter->second[3] << "("
					<< pathIter->second[3] / (ddrSum[3]) << " )";
			kml << "</description>";
			list<patArc*>* listOfArcs = pathIter->first->getArcList();
			for (list<patArc*>::iterator aIter = listOfArcs->begin();
					aIter != listOfArcs->end(); ++aIter) {
				kml << "            <Placemark>" << endl;
				patString theName((*aIter)->name);
				replaceAll(&theName, patString("&"), patString("and"));
				kml << "                  <name>" << theName << "</name>"
						<< endl;
				kml << "                  <description>Arc " << (*aIter)->userId
						<< " from node " << (*aIter)->upNodeId << " to node "
						<< (*aIter)->downNodeId << "</description>" << endl;
				kml << " " << endl;
				kml << "                  <Style>" << endl;
				kml << "                        <LineStyle>" << endl;
				kml << "                              <color>"
						<< patNBParameters::the()->pathLineColor << "</color>"
						<< endl;
				kml << "                              <width>"
						<< patNBParameters::the()->pathLineWidth << "</width>"
						<< endl;
				kml << "                        </LineStyle>" << endl;
				kml << "                  </Style>" << endl;
				kml << " " << endl;
				kml << "                  <LineString>" << endl;
				kml << "                        <coordinates>" << endl;

				list<patGeoCoordinates>::iterator gIter(
						(*aIter)->polyline.begin());
				list<patGeoCoordinates>::iterator hIter(
						(*aIter)->polyline.begin());
				++hIter;
				for (; hIter != (*aIter)->polyline.end(); ++gIter, ++hIter) {

					double a1 = gIter->longitudeInDegrees;
					double a2 = gIter->latitudeInDegrees;
					double b1 = hIter->longitudeInDegrees;
					double b2 = hIter->latitudeInDegrees;

					kml << a1 << "," << a2 << ",0 " << b1 << "," << b2 << ",0"
							<< endl;
				}

				kml << "</coordinates>	" << endl;
				kml << "                  </LineString>" << endl;
				kml << "            </Placemark>" << endl;
			}
			kml << "</Folder>" << endl;
		}
		kml << "</Folder>" << endl;
	}
	kml << "</Folder>" << endl;

	kml << "<Folder>" << endl;
	kml << "<name>GPS Points</name>" << endl;

	for (int i = 0; i < gpsSequence.size(); ++i) {
		kml << "<name>GPS:" << i + 1 << "</name>" << endl;
		kml << "<Placemark>" << endl;
		kml << "<Style >" << endl;
		kml << "<IconStyle>" << endl;
		kml << "<color>" << patNBParameters::the()->gpsIconColor << "</color>"
				<< endl;
		kml << "<scale>" << patNBParameters::the()->gpsIconScale << "</scale>"
				<< endl;
		/* kml<<"<heading>"<<endl;
		 kml<<gpsSequence[i].heading;
		 kml<<"</heading>"<<endl;
		 */
		kml << "<Icon>" << endl;
		kml << "<href>" << patNBParameters::the()->gpsIcon << "</href>" << endl;
		kml << "</Icon>" << endl;
		kml << "</IconStyle>" << endl;
		kml << "</Style>" << endl;
		kml << "<TimeStamp>" << endl;
		kml << "	<when>" << endl;
		time_t rawtime = gpsSequence[i].getTimeStamp();
		tm* ptm = gmtime(&rawtime);
		char buffer[50];
		sprintf(
				buffer,
				"%4d-%02d-%02dT%02d:%02d:%02dZ",
				(ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday, (ptm->tm_hour) % 24, ptm->tm_min, ptm->tm_sec);
		kml << (buffer) << endl;
		kml << "	</when>" << endl;
		kml << "</TimeStamp>" << endl;
		//kml<<"<name>GPS:"<<i+1<<"</name>"<<endl;
		kml << "                  <description> " << gpsSequence[i]
				<< "</description>" << endl;
		kml << "                  <Point>" << endl;
		kml << "                        <coordinates>"
				<< gpsSequence[i].getGeoCoord()->getKML() << ", 0</coordinates>"
				<< endl;
		kml << "                  </Point>" << endl;
		kml << "</Placemark>" << endl;
	}
	kml << "</Folder>" << endl;
	if (patNBParameters::the()->exportDDR == 1) {
		kml << "<Folder>" << endl;
		kml << "<name>GPS Points+DDR</name>" << endl;

		for (int i = 0; i < gpsSequence.size(); ++i) {
			kml << "<Folder>" << endl;
			kml << "<name>GPS:" << i + 1 << "</name>" << endl;
			kml << "<Placemark>" << endl;
			kml << "<Style >" << endl;
			kml << "<IconStyle>" << endl;
			kml << "<color>" << patNBParameters::the()->gpsIconColor
					<< "</color>" << endl;
			kml << "<scale>" << patNBParameters::the()->gpsIconScale
					<< "</scale>" << endl;
			/* kml<<"<heading>"<<endl;
			 kml<<gpsSequence[i].heading;
			 kml<<"</heading>"<<endl;
			 */
			kml << "<Icon>" << endl;
			kml << "<href>" << patNBParameters::the()->gpsIcon << "</href>"
					<< endl;
			kml << "</Icon>" << endl;
			kml << "</IconStyle>" << endl;
			kml << "</Style>" << endl;
			kml << "<TimeStamp>" << endl;
			kml << "	<when>" << endl;
			time_t rawtime = gpsSequence[i].getTimeStamp();
			tm* ptm = gmtime(&rawtime);
			char buffer[50];
			sprintf(
					buffer,
					"%4d-%02d-%02dT%02d:%02d:%02dZ",
					(ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday, (ptm->tm_hour) % 24, ptm->tm_min, ptm->tm_sec);
			kml << (buffer) << endl;
			kml << "	</when>" << endl;
			kml << "</TimeStamp>" << endl;
			//kml<<"<name>GPS:"<<i+1<<"</name>"<<endl;
			kml << "                  <description> " << gpsSequence[i]
					<< "</description>" << endl;
			kml << "                  <Point>" << endl;
			kml << "                        <coordinates>"
					<< gpsSequence[i].getGeoCoord()->getKML()
					<< ", 0</coordinates>" << endl;
			kml << "                  </Point>" << endl;
			kml << "</Placemark>" << endl;
			map<patArc*, double>* linkDDR = gpsSequence[i].getLinkDDR();
			for (map<patArc*, double>::iterator aIter = linkDDR->begin();
					aIter != linkDDR->end(); ++aIter) {
				kml << "            <Placemark>" << endl;
				kml << "<TimeStamp>" << endl;
				kml << "	<when>" << endl;

				sprintf(
						buffer,
						"%4d-%02d-%02dT%02d:%02d:%02dZ",
						(ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday, (ptm->tm_hour) % 24, ptm->tm_min, ptm->tm_sec);
				kml << (buffer) << endl;
				kml << "	</when>" << endl;
				kml << "</TimeStamp>" << endl;
				patString theName(aIter->first->name);
				replaceAll(&theName, patString("&"), patString("and"));
				kml << "                  <name>" << theName << "</name>"
						<< endl;
				kml << "                  <description>Arc "
						<< aIter->first->userId << "DDR Value:" << aIter->second
						<< "heading:" << aIter->first->attributes.heading
						<< " from node " << aIter->first->upNodeId
						<< " to node " << aIter->first->downNodeId << "length "
						<< aIter->first->length << "</description>" << endl;
				kml << " " << endl;
				kml << "                  <Style>" << endl;
				kml << "                        <LineStyle>" << endl;
				kml << "                              <color>ff00ffff</color>"
						<< endl;
				kml << "                              <width>4</width>" << endl;
				kml << "                        </LineStyle>" << endl;
				kml << "                  </Style>" << endl;
				kml << " " << endl;
				kml << "                  <LineString>" << endl;
				kml << "                        <coordinates>" << endl;

				list<patGeoCoordinates>::iterator gIter(
						aIter->first->polyline.begin());
				list<patGeoCoordinates>::iterator hIter(
						aIter->first->polyline.begin());
				++hIter;
				for (; hIter != aIter->first->polyline.end();
						++gIter, ++hIter) {

					double a1 = gIter->longitudeInDegrees;
					double a2 = gIter->latitudeInDegrees;
					double b1 = hIter->longitudeInDegrees;
					double b2 = hIter->latitudeInDegrees;

					kml << a1 << "," << a2 << ",0 " << b1 << "," << b2 << ",0"
							<< endl;
				}

				kml << "</coordinates>	" << endl;
				kml << "                  </LineString>" << endl;
				kml << "            </Placemark>" << endl;
			}
			kml << "</Folder>" << endl;
		}
		kml << "</Folder>" << endl;
	}
	kml << " " << endl;
	kml << "      </Document>" << endl;
	kml << "      </kml>" << endl;

	kml.close();
}

void patTripParser::genOd() {
	DEBUG_MESSAGE("OK");

	map<patArc*, double>* destinationLinkDDR = gpsSequence.back().getLinkDDR();
	//leastDistance = 10000;
	for (map<patArc*, double>::const_iterator iter =
			destinationLinkDDR->begin(); iter != destinationLinkDDR->end();
			++iter) {
		patNode* currNode = theNetwork->getNodeFromUserId(
				iter->first->downNodeId);

		DEBUG_MESSAGE(
				"destination. " << currNode->internalId << ","
						<< currNode->geoCoord);
		map<patNode*, double>::iterator found = originDDR.find(currNode);
		destinationDDR[currNode] =
				(found == originDDR.end()) ?
						iter->second : (originDDR[currNode] + iter->second);
	}

}

void patTripParser::genOdDDR() {
	map<patPathJ*, patPathDDR>* pathDDRs = generatedObservation.getPathDDRs();
	for (map<patPathJ*, patPathDDR>::iterator pathDDRIter = pathDDRs->begin();
			pathDDRIter != pathDDRs->end(); ++pathDDRIter) {

		patOdJ* odFound = generatedObservation.odDDRs.find(
				pathDDRIter->first->getOd())->first;
		if (odFound == NULL) {
			generatedObservation.odDDRs[odFound] =
					pathDDRIter->second.getPathValue();
		} else {
			generatedObservation.odDDRs[odFound] +=
					pathDDRIter->second.getPathValue();
		}
	}
}

patObservation patTripParser::rtnObservation() {
	return generatedObservation;
}

void patTripParser::genAssocDomain(
		map<patArc*, set<unsigned long> >* associateDomain) {
	for (int i = 0; i < gpsSequence.size(); i++) {
		map<patArc*, double>* linkDDRTemp = gpsSequence[i].getLinkDDR();
		for (map<patArc*, double>::iterator iter = linkDDRTemp->begin();
				iter != linkDDRTemp->end(); iter++) {

			if (associateDomain->find(iter->first) == associateDomain->end()) {
				(*associateDomain)[iter->first] = set<unsigned long>();
			}
			(*associateDomain)[iter->first].insert(i);
		}
	}
}

vector<patGpsPoint>* patTripParser::getGpsSequence(){
	return &gpsSequence;
}
