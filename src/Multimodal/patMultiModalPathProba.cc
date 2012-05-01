#include "patConst.h"
#include "patPathJ.h"
#include "patNetwork.h"
#include "patArc.h"
#include "patGpsPoint.h"
#include "patPathProbaAlgoV4.h"
#include "patTripGraph.h"
#include "patDisplay.h"
#include "patError.h"
#include "patErrMiscError.h"
#include "patArcTransition.h"
#include "patNBParameters.h"
#include "patPower.h"

/*
 *
 */

patMultiModalPathProba::patMultiModalPathProba(patPathJ* path,
		patMapMatchingIteration* mm_iteration,
		set<patArcTransition>* arc_tran_set):
m_path(path),
m_mm_iteartion(mm_iteration),
m_proba(mm_iteration->getCurentGpsSequence()->size(),0),
m_point_arc_probas(mm_iteration->getCurentGpsSequence()->size(),map<patArc*,double>()),
m_arc_tran_set(arc_tran_set)
{
	//DEBUG_MESSAGE("Initiate path probability calculation");
}

void patMultiModalPathProba::initArc() {

	//DEBUG_MESSAGE("INIT");
	m_m_proba[0] = 1.0;
	m_point_arc_m_proba[0][m_m_path->front()] = 1.0;
	m_m_proba[0] = calPointProba("raw", 0);

	//DEBUG_MESSAGE("poit proba 0"<<m_proba[0]);
}

double patMultiModalPathProba::run_raw() {
	double path_proba = 1.0;
	for (int j = 0; j < m_mm_iteartion->getCurentGpsSequence()->size(); ++j) {

		calPointProba("raw", j);
		if (m_proba[j] == 0.0) {
			WARNING("zero value at" << j);
			return 0.0;
		}
		//DEBUG_MESSAGE("point proba "<<j<<":"<<m_proba[j]);
	}

	for (int j = 0; j < proba.size(); j++) {
		path_proba *= m_proba[j];
	}

	//DEBUG_MESSAGE("raw proba "<<probaPath);
	return path_proba;
}

double patMultiModalPathProba::run(string algo_type) {

	if (algo_type == "raw") {
		return run_raw();
	} else {
		return run_ts(algo_type);
	}
	return 0.0;

}

double patMultiModalPathProba::firstPointProba(string algo_type) {
	m_proba[0] = 0.0;
	map<patArc*, double>* ddr_arcs = m_mm_iteartion->getValidIterations()->at(0)->getDDRArcs();//get DDR of the first valid GPS point

	for (map<patArc*, double>::iterator arc_iter = ddr_arcs->begin();
			arc_iter != ddr_arcs->end();
			++arc_iter) {
		m_point_arc_m_proba[0][arc_iter->first] = 0.0;
		if (m_path->isLinkInPath(arc_iter->first)) {
			m_proba[0] += arc_iter->second;
		}
	}
	if (m_proba[0] / m_path->getPathLength() > 1.0) {
		//proba greater than zero, show warning message.
		WARNING(
				m_proba[0] / m_path->getPathLength() << "=" << m_proba[0] << "/"
						<< m_path->getPathLength());
	}
	m_proba[0] = m_proba[0] / m_path->getPathLength();
	return m_proba[0];

}

double patMultiModalPathProba::run_ts(string algo_type) {
	/*to do*/
	//double probaPath = 1.0 * gpsSequence->front()->getddr_arcsValue(m_path->front(),baseNetwork)/gpsSequence->at(0)->getDDRSum();
	//probaPath = 1.0;
	double path_proba = 1.0;
	path_proba=firstPointProba(algo_type);
	if (path_proba == 0.0) {
		return 0.0;
	}
	//DEBUG_MESSAGE(m_mm_iteartion->getCurentGpsSequence()->size());
	for (unsigned long j = 1; j < m_mm_iteartion->getCurentGpsSequence()->size(); ++j) {
		//	DEBUG_MESSAGE("point "<<j);
		calPointProba(algo_type, j);
		if (m_proba[j] == 0.0) {
			DEBUG_MESSAGE(
					"algo:" << algo_type << "zero probability at [" << (j + 1)
							<< "]" << m_mm_iteartion->getCurentGpsSequence()->at(j)->getTimeStamp()
							<< " total gps:" << m_mm_iteartion->getCurentGpsSequence()->size());
			return 0.0;
		}
	}
	for (int j = 0; j < m_proba.size(); j++) {
		//DEBUG_MESSAGE("point "<<j+1<<": "<<m_proba[j])
		if (m_proba[j] > 1.0) {
			DEBUG_MESSAGE(
					"point " << j + 1 << " probability greater than 1.0: "
							<< m_proba[j]);
		}

		path_proba *= m_proba[j];
	}
	DEBUG_MESSAGE("path proba" << path_proba);

	return path_proba;
}

double patMultiModalPathProba::calPointProba(string algo_type,
		unsigned long j) {
	/*
	string at = algo_type;
	if (algo_type == "ts"
			&& (j == 0 || gpsSequence->at(j)->getType() != "normal_speed"
					|| (j > 0
							&& gpsSequence->at(j - 1)->getType()
									!= "normal_speed"))) {
		low_speed_algo = patNBParameters::the()->lowSpeedAlgo;

	}
	*/
	m_proba[j] = 0.0;
	map<patArc*, double>* ddr_arcs = m_mm_iteartion->getCurentGpsSequence()->at(j)->getGpsDDR()->getDDRArcs();
	double backLength = 0.0;
	for (map<patArc*, double>::iterator
			arc_iter = ddr_arcs->begin();
			arc_iter != ddr_arcs->end();
			++arc_iter) {
		m_point_arc_m_proba[j][arc_iter->first] = 0.0;
		if (m_path->isLinkInPath(arc_iter->first)) {
			m_point_arc_probas[j][arc_iter->first] = calPointArcProba(algo_type, j,
					pair<patArc*, double>(arc_iter->first, arc_iter->second));

			m_proba[j] += m_point_arc_probas[j][arc_iter->first];

			//		backLength+=arc_iter->first->getLength();
		}
	}

	if (j > 0) {
		DEBUG_MESSAGE(m_proba[j] << "," << getPointSimpleDDR(j - 1));
		m_proba[j] = m_proba[j] / getPointSimpleDDR(j - 1); //normalization
		DEBUG_MESSAGE(j + 1 << "," << m_proba[j]);
	}
	//if(backLength>0.0){
	//	m_proba[j] /= patPower(backLength/1000, patNBParameters::the()->pointProbaPower);
	//}

	return m_proba[j];
}


double patMultiModalPathProba::getPointSimpleDDR(int g) {

	double point_ddr_sum = 0.0;
	map<patArc*, double>* ddr_arcs = m_mm_iteartion->getCurentGpsSequence()->at(j)->getGpsDDR()->getDDRArcs();
	for (map<patArc*, double>::iterator arc_iter = ddr_arcs->begin();
			arc_iter != ddr_arcs->end();
			++arc_iter) {
		if (m_path->isLinkInPath(arc_iter->first)) {
			point_ddr_sum += arc_iter->first->getLength() * arc_iter->second;
		}
	}
	//DEBUG_MESSAGE(pointDDRSum);
	if (point_ddr_sum == 0.0) {
		DEBUG_MESSAGE("zero point ddr sum at point" << g);
	}
	return point_ddr_sum;
}
double patMultiModalPathProba::calPointArcProbas(string algo_type, int j,
		pair<patArc*, double> ddr_arcs) {

	if (algo_type == "raw" || j == 0) {
		DEBUG_MESSAGE("arc ddr:" << ddr_arcs.first << "," << ddr_arcs.second);

		return ddr_arcs.second / m_path->getPathLength();
	}
	double point_arc_proba = 0.0;
	struct gps_params GP;
	GP.time_diff = -1.0;
	map<patArc*, double>* prev_ddr_arcs = m_mm_iteartion->getCurentGpsSequence()->at(j-1)->getGpsDDR()->getDDRArcs();
	for (map<patArc*, double>::iterator arc_iter = prev_ddr_arcs->begin();
			arc_iter != prev_ddr_arcs->end();
			++arc_iter) {

		list < pair<patArc*, TransportMode> > interSeg = m_path->getSeg(
				arc_iter->first, ddr_arcs.first);
		if (interSeg.empty()) {
			continue;
		}
		patArcTransition theTran(interSeg, m_mm_iteartion->getCurentGpsSequence()->at(j-1),
				m_mm_iteartion->getCurentGpsSequence()->at(j-1));
		double tranValue;
		if (GP.time_diff <= 0.0) {
			gpsSequence->at(j)->setGpsParams(&GP, m_mm_iteartion->getCurentGpsSequence()->at(j - 1),
					m_mm_iteartion->getOriginalGpsSequence());
		}

		set<patArcTransition>::iterator found = arcTranSet->find(theTran);
		if (found != arcTranSet->end()) {
			tranValue = const_cast<patArcTransition*>(&(*found))->getValue(
					algo_type, baseNetwork, GP);
		} else {

			tranValue = theTran.getValue(algo_type, baseNetwork, GP);
			arcTranSet->insert(theTran);

		}

		point_arc_proba += tranValue;

	}
//	DEBUG_MESSAGE(point_arc_proba);
	return point_arc_proba;
}

