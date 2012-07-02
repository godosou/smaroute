/*
 * patWriteBiogemeData.cc
 *
 *  Created on: May 4, 2012
 *      Author: jchen
 */

#include "patWriteBiogemeData.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include "patDisplay.h"
#include "patLinkAndPathCost.h"
#include "patNBParameters.h"
#include "patArc.h"
using namespace std;
patWriteBiogemeData::patWriteBiogemeData(
		const vector<patObservation>& observations,
		const patUtilityFunction* utility_function,
		const patPathGenerator* path_generator, const patChoiceSet* choice_set) :
		m_observations(observations), m_utility_function(utility_function), m_path_generator(
				path_generator), m_universal_choiceset(choice_set) {
	// TODO Auto-generated constructor stub
	genHeader();
}

patWriteBiogemeData::~patWriteBiogemeData() {
	// TODO Auto-generated destructor stub
}
void patWriteBiogemeData::genHeader() {
	m_header.clear();
	m_header.push_back("id");
	m_header.push_back("AggLast");
	m_header.push_back("AggWeight");
	m_header.push_back("AggWeightNormalized");
	m_header.push_back("choiceRP");
	m_header.push_back("tripId");
	m_header.push_back("Orig");
	m_header.push_back("Dest");
	for (int i = 0; i <= patNBParameters::the()->choiceSetInBiogemeData; ++i) {
		{
			stringstream ss;
			ss << "sc" << i;
			m_header.push_back(ss.str());
		}
		{
			stringstream ss;
			ss << "ps" << i;
			m_header.push_back(ss.str());
		}
		{
			stringstream ss;
			ss << "ps_u" << i;
			m_header.push_back(ss.str());
		}
		{
			stringstream ss;
			ss << "avail" << i;
			m_header.push_back(ss.str());
		}
		for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator link_coef_iter =
				m_utility_function->getLinkCoefficients().begin();
				link_coef_iter
						!= m_utility_function->getLinkCoefficients().end();
				++link_coef_iter) {
			stringstream ss;
			ss << patArc::getAttributeTypeString(link_coef_iter->first) << i;
			m_header.push_back(ss.str());
		}
		for (unordered_map<const char*, double>::const_iterator path_coef_iter =
				m_utility_function->getPathCoefficients().begin();
				path_coef_iter
						!= m_utility_function->getPathCoefficients().end();
				++path_coef_iter) {
			stringstream ss;
			ss << path_coef_iter->first << i;
			m_header.push_back(ss.str());
		}
	}
}
void patWriteBiogemeData::writeHeader(const string& fileName) {
//	DEBUG_MESSAGE("Create " << fileName);
	ofstream sampleFile(fileName.c_str(), ios::app);
	sampleFile << m_header[0];
	for (int i = 1; i < m_header.size(); ++i) {
		sampleFile << "\t" << m_header[i];
	}
	sampleFile << endl;
	sampleFile.close();
}
void patWriteBiogemeData::writeData(const string& fileName) {

	ofstream sampleFile(fileName.c_str(), ios::app);
	int i = 0;
	for (vector<patObservation>::const_iterator obs_iter =
			m_observations.begin(); obs_iter != m_observations.end();
			++obs_iter) {

		list<unordered_map<string, string> > abs_attributes =
				(*obs_iter).genAttributes(m_utility_function, m_path_generator,
						m_universal_choiceset);
		for (list<unordered_map<string, string> >::const_iterator p_iter =
				abs_attributes.begin(); p_iter != abs_attributes.end();
				++p_iter) {
			unordered_map<string, string> p_attributes = *p_iter;
			sampleFile << i;
			for (int i = 1; i < m_header.size(); ++i) {
				string k = m_header[i];
//				if(k.find("sc")!=string::npos){
//					sampleFile<<endl;
//				}
				unordered_map<string, string>::const_iterator find_coef =
						p_attributes.find(k);
				if (find_coef == p_attributes.end()) {
					//WARNING("not found coef"<<k);
					sampleFile << "\t" << 9999;
				} else {
					if (k == "AggLast" || k == "choiceRP"
							|| k.find("avail") != string::npos) {
						stringstream convert(p_attributes[k]);
						int int_n;
						convert >> int_n;
						if (k == "choiceRP") {
//							DEBUG_MESSAGE(int_n);
						}
//						DEBUG_MESSAGE(p_attributes[k]<<","<<atoi(p_attributes[k].c_str()));
						sampleFile << "\t" << int_n;
					} else if (k == "tripId") {
						string trip_id = p_attributes[k];
						try{
							trip_id.erase(trip_id.find(" "),1);
							trip_id.erase(trip_id.find("-"),1);
						}
						catch(...){

						}
						sampleFile << "\t" << trip_id;

					} else {
						sampleFile << "\t" << p_attributes[k];
					}
				}
			}
			sampleFile << endl;
			++i;
		}
	}

	sampleFile.close();
}
void patWriteBiogemeData::writeSampleFile(const string& fileName) {
	DEBUG_MESSAGE(m_observations.size()<<" observations write to"<<fileName);
//	DEBUG_MESSAGE("Create " << fileName);
	ofstream sample(fileName.c_str());
	sample << setprecision(7) << setiosflags(ios::scientific);

	writeHeader(fileName);
	writeData(fileName);
	sample.close();

}

void patWriteBiogemeData::writeSpecFile(const string& fileName) {
	ofstream spec(fileName.c_str());
	spec << "[Choice]" << endl;
	spec << "choiceRP" << endl;
	spec << "" << endl;

	spec << "[AggregateLast]" << endl;
	spec << "AggLast" << endl;
	spec << "" << endl;

	spec << "[AggregateWeight]" << endl;
	spec << "AggWeight" << endl;
	spec << "" << endl;

	spec << "[ModelDescription]" << endl;
	spec << "\"path size\"" << endl;
	spec << endl;

	spec << "[Beta]" << endl;
	spec << "// WARNING: this section very likely needs to be updated by hand"
			<< endl;
	spec << "// A generic parameter for each path attribute is defined here."
			<< endl;
	spec
			<< "// Parameters capturing socio-economic attributes must be added by hand."
			<< endl;
	spec
			<< "// Note that bounds are arbitrary. You may need to change them by hand"
			<< endl;
	spec << "// Name Value  LowerBound UpperBound  status (0=variable, 1=fixed)"
			<< endl;
	patString betaPathSize("BETA_PathSize");
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
	const map<ARC_ATTRIBUTES_TYPES, double>& link_coefficients_map =
			m_utility_function->getLinkCoefficients();
	vector<string> link_coefficients;
	for (map<ARC_ATTRIBUTES_TYPES, double>::const_iterator lc_iter =
			link_coefficients_map.begin();
			lc_iter != link_coefficients_map.end(); ++lc_iter) {
		link_coefficients.push_back(patArc::getAttributeTypeString(lc_iter->first));
	}

	spec << "BETA_PS" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	spec << "BETA_PS_U" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	for (vector<string>::const_iterator lc_iter = link_coefficients.begin();
			lc_iter != link_coefficients.end(); ++lc_iter) {
		spec << "BETA_" << *lc_iter << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	}
//	spec << "BETA_length" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
//	spec << "BETA_sb" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	spec << "MU" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	spec << "corr_one" << "\t0.0\t-1000.0\t1000.0\t0" << endl;
	//spec << "ZERO"<<"\t0.0\t-1000.0\t1000.0\t1" << endl ;
	//spec << "SIGMA"<<"\t0.0\t-1000.0\t1000.0\t0" << endl ;

	spec << "" << endl;

	spec << "" << endl;
	spec << "[Utilities]" << endl;
	spec
			<< "// Id Name  Avail  linear-in-parameter expression (beta1*x1 + beta2*x2 + ... )"
			<< endl;

	for (unsigned long alt = 0;
			alt <= patNBParameters::the()->choiceSetInBiogemeData; ++alt) {
		spec << alt << '\t'; // id
		spec << "alt" << alt << '\t'; //alt
		spec << "avail" << alt << '\t'; //alt
		spec << " BETA_PS_U " << " * " << " log_ps_u" << alt;
		spec << " + ";
		for (vector<string>::const_iterator lc_iter = link_coefficients.begin();
				lc_iter != link_coefficients.end(); ++lc_iter) {
			spec << " BETA_" << *lc_iter << " * " << *lc_iter << alt;
			spec << " + ";

		}

//		spec << " BETA_length " << " * " << " length" << alt;
//		spec << " + ";
//		spec << " BETA_sb " << " * " << " speed_bump" << alt;
//		spec << " + ";
		spec << "corr_one" << " * " << "sc" << alt;
		spec << endl;
	}

	spec << "" << endl;

	spec << "[GeneralizedUtilities]" << endl;
	spec
			<< "// Id   linear-in-parameter expression (beta1*x1 + beta2*x2 + ... )"
			<< endl;

	for (unsigned long alt = 0;
			alt <= patNBParameters::the()->choiceSetInBiogemeData; ++alt) {
		spec << alt << '\t'; // id
		spec << "MU" << " * " << " BETA_PS_U " << " * " << " log_ps_u" << alt;
		spec << " + ";
		spec << "MU" << " * " << " BETA_length " << " * " << " length" << alt;
		spec << " + ";
		spec << "MU" << " * " << " BETA_PS " << " * " << " log_ps" << alt;

		spec << " " << endl;
	}

	spec << "" << endl;

	spec << "[Draws]" << endl;
	spec << patNBParameters::the()->biogemeEstimationDraws << endl;
	spec << "" << endl;

	spec << "[Expressions] " << endl;
	spec
			<< "// Define here arithmetic expressions for name that are not directly "
			<< endl;
	spec << "// available from the data" << endl;
	spec << "one = 1" << endl;
	spec << "" << endl;

	for (unsigned long alt = 0;
			alt <= patNBParameters::the()->choiceSetInBiogemeData; ++alt) {
		spec << "log_ps" << alt << " = log( ps" << alt << " )";
		spec << " " << endl;
	}

	spec << "" << endl;
	for (unsigned long alt = 0;
			alt <= patNBParameters::the()->choiceSetInBiogemeData; ++alt) {
		spec << "log_ps_u" << alt << " = log( ps_u" << alt << " )";
		spec << " " << endl;
	}
	spec << "[Exclude]" << endl;
	spec << "$NONE" << endl;
	spec << "" << endl;
	spec << "[Model]" << endl;
	spec
			<< "// Currently, only $MNL (multinomial logit), $NL (nested logit) and $CNL"
			<< endl;
	spec << "// (cross-nested logit) are valid keywords" << endl;
	spec << "$MNL" << endl;
	spec << "//$NL" << endl;
	spec << "//$CNL" << endl;
	spec << endl;
	spec << "[Scale]" << endl;
	spec << "// The sample can be divided in several groups of individuals. The"
			<< endl;
	spec
			<< "//utility of an individual in a group will be multiplied by the scale factor"
			<< endl;
	spec << "//associated with the group." << endl;
	spec << "" << endl;
	spec << "// Group_number  scale LowerBound UpperBound status" << endl;
	spec << "$NONE" << endl;
	spec << endl;
	spec << "[Ratios] " << endl;
	spec
			<< "// List of ratios of estimated coefficients that must be produced in"
			<< endl;
	spec << "// the output. The most typical is the value-of-time." << endl;
	spec << "// Numerator   Denominator  Name" << endl;
	spec << "$NONE" << endl;
	spec.close();

}
