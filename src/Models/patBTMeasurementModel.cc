/* 
 * File:   patBTMeasurementModel.cc
 * Author: jchen
 * 
 * Created on May 4, 2011, 4:34 PM
 */

#include "patBTMeasurementModel.h"
#include <vector>
#include "patType.h"
patBTMeasurementModel::patBTMeasurementModel() {
    //for car
    vector<patReal> car_proba;
    car_proba.push_back(patReal(0.01));
    car_proba.push_back(patReal(0.85));
    car_proba.push_back(patReal(0.1));
    car_proba.push_back(patReal(0.03));
    car_proba.push_back(patReal(0.01));
    threshold.push_back(patULong(car_proba.size()-1));
    proba_matrix.push_back(car_proba);


    vector<patReal> bus_proba;
    bus_proba.push_back(patReal(0.28));
    bus_proba.push_back(patReal(0.28));
    bus_proba.push_back(patReal(0.22));
    bus_proba.push_back(patReal(0.10));
    bus_proba.push_back(patReal(0.10));
    bus_proba.push_back(patReal(0.02));
    threshold.push_back(patULong(bus_proba.size()-1));
    proba_matrix.push_back(bus_proba);


    vector<patReal> train_proba;
    train_proba.push_back(patReal(0.01));
    train_proba.push_back(patReal(0.38));
    train_proba.push_back(patReal(0.27));
    train_proba.push_back(patReal(0.18));
    train_proba.push_back(patReal(0.06));
    train_proba.push_back(patReal(0.06));
    train_proba.push_back(patReal(0.02));
    train_proba.push_back(patReal(0.02));
    threshold.push_back(patULong(train_proba.size()-1));
    proba_matrix.push_back(train_proba);


    vector<patReal> metro_proba;
    metro_proba.push_back(patReal(0.09));
    metro_proba.push_back(patReal(0.22));
    metro_proba.push_back(patReal(0.24));
    metro_proba.push_back(patReal(0.18));
    metro_proba.push_back(patReal(0.16));
    metro_proba.push_back(patReal(0.06));
    metro_proba.push_back(patReal(0.05));
    threshold.push_back(patULong(metro_proba.size()-1));
    proba_matrix.push_back(metro_proba);
    
    vector<patReal> walk_proba;
    walk_proba.push_back(patReal(0.49));
    walk_proba.push_back(patReal(0.30));
    walk_proba.push_back(patReal(0.11));
    walk_proba.push_back(patReal(0.02));
    walk_proba.push_back(patReal(0.02));
    walk_proba.push_back(patReal(0.03));
    walk_proba.push_back(patReal(0.02));
    walk_proba.push_back(patReal(0.01));
    threshold.push_back(patULong(walk_proba.size()-1));
    proba_matrix.push_back(walk_proba);

    vector<patReal> bike_proba;
    bike_proba.push_back(patReal(0.54));
    bike_proba.push_back(patReal(0.21));
    bike_proba.push_back(patReal(0.13));
    bike_proba.push_back(patReal(0.08));
    bike_proba.push_back(patReal(0.03));
    bike_proba.push_back(patReal(0.01));
    threshold.push_back(patULong(bike_proba.size()-1));
    proba_matrix.push_back(bike_proba);
    
}

/*
 Calculate probability for mode
 * @param mode
 * @param number
 * return proba
 * mode:
 * 1. car
 * 2. bus
 * 3. train
 * 4. metro
 * 5. walk
 * 6. bike
 */
patReal patBTMeasurementModel::calProba(patULong mode, patULong number){
   if(number>threshold[mode]){
       return proba_matrix[mode][threshold];
   }
   else{
       return proba_matrix[mode][number];
   }
    
}
patBTMeasurementModel::patBTMeasurementModel(const patBTMeasurementModel& orig) {
}

patBTMeasurementModel::~patBTMeasurementModel() {
}

