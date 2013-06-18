/* 
 * File:   patBTMeasurementModel.h
 * Author: jchen
 *
 * Created on May 4, 2011, 4:34 PM
 */

#ifndef PATBTMEASUREMENTMODEL_H
#define	PATBTMEASUREMENTMODEL_H
#include <vector>
#include "patType.h"
class patBTMeasurementModel {
public:
    patBTMeasurementModel();
    patReal calProba(patULong mode, patULong number);
    patBTMeasurementModel(const patBTMeasurementModel& orig);
    virtual ~patBTMeasurementModel();
private:
    vector<patULong> threshold;
    vector<vector<patReal> > proba_matrix;
};

#endif	/* PATBTMEASUREMENTMODEL_H */

