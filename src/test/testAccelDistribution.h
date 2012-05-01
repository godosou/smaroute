#include <cxxtest/TestSuite.h>
#include <vector>;
#include "patAccelMeasurementModel.h"
#include "patNBParameters.h"
#include "patTransportMode.h"
#include "patError.h"
#include "patDisplay.h"
#include "patMixtureNormal.h"
#include <stdio.h> 

class MyTestAccelDistribution : public CxxTest::TestSuite{
public:
	void testAccelDistribution(void){

		patError* err(NULL);
		patNBParameters::the()->readFile("/Users/jchen/Documents/Project/biogeme/newbioroute/config.xml", err);
		patNBParameters::the()->init(err);
		TS_ASSERT_EQUALS(err,(void*)0);
		patAccelMeasurementModel::readParams(err);
		TS_ASSERT_EQUALS(err,(void*)0);
		double lower_bound=0.0;
		double upper_bound=1000.0;
		double step_size=0.1;

		int step = (int) (upper_bound-lower_bound)/step_size;


		patMixtureNormal mn_walk(patAccelMeasurementModel::m_params[WALK].components,
			patAccelMeasurementModel::m_params[WALK].w,
			patAccelMeasurementModel::m_params[WALK].mu,
			patAccelMeasurementModel::m_params[WALK].sigma,
		err);
		TS_ASSERT_EQUALS(err,(void*)0);

		patMixtureNormal mn_bike(patAccelMeasurementModel::m_params[BIKE].components,
			patAccelMeasurementModel::m_params[BIKE].w,
			patAccelMeasurementModel::m_params[BIKE].mu,
			patAccelMeasurementModel::m_params[BIKE].sigma,
		err);

		TS_ASSERT_EQUALS(err,(void*)0);

		patMixtureNormal mn_motor(patAccelMeasurementModel::m_params[CAR].components,
			patAccelMeasurementModel::m_params[CAR].w,
			patAccelMeasurementModel::m_params[CAR].mu,
			patAccelMeasurementModel::m_params[CAR].sigma,
		err);

		TS_ASSERT_EQUALS(err,(void*)0);
		
		std::vector<double> x;
		std::vector<double> p_walk;
		std::vector<double> p_bike;
		std::vector<double> p_motor;

		double sum_p_walk = 0.0;
		double sum_p_bike = 0.0;
		double sum_p_motor = 0.0;
		double last_value = lower_bound;
		FILE *f = fopen("pdf.csv", "w");
		for(int i =0;i<=step;++i){
			x.push_back(last_value);

			double p_walk_s = mn_walk.pdf(last_value);
			double p_bike_s = mn_bike.pdf(last_value);
			double p_motor_s = mn_motor.pdf(last_value);
			//DEBUG_MESSAGE(last_value<<":"<<p_walk_s<<","<<p_bike_s<<","<<p_motor_s);
			sum_p_walk+=p_walk_s*step_size;
			sum_p_bike+=p_bike_s*step_size;
			sum_p_motor+=p_bike_s*step_size;
			fprintf(f, "%.2f,%.5f,%.5f,%.5f\n", last_value, p_walk_s, p_bike_s,p_motor_s);
			p_walk.push_back(p_walk_s);
			p_bike.push_back(p_bike_s);
			p_motor.push_back(p_motor_s);
			last_value+=step_size;
		}
		fclose(f);
		TS_ASSERT_DELTA(sum_p_walk,1.0,1e-4);
		TS_ASSERT_DELTA(sum_p_bike,1.0,1e-4);
		TS_ASSERT_DELTA(sum_p_motor,1.0,1e-4);
	}
};