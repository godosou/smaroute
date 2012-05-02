/*
 * testRandomNumber.h
 *
 *  Created on: Apr 20, 2012
 *      Author: jchen
 */

#ifndef TESTRANDOMNUMBER_H_
#define TESTRANDOMNUMBER_H_

#include <cxxtest/TestSuite.h>

#include <vector>
#include "patNBParameters.h"
#include "patDisplay.h"
#include "patError.h"
#include "patRandomNumber.h"
#include <boost/random.hpp>
class MyTestRandomNumberGenrator: public CxxTest::TestSuite{
public:
	void testRNG(void){
		patError* err(NULL);
		patNBParameters::the()->readFile("/Users/jchen/Documents/Project/newbioroute/src/params/config.xml", err);
		patNBParameters::the()->init(err);
		TS_ASSERT_EQUALS(err,(void*)0);
		int count= 100000;

		double sum=0.0;
		patRandomNumber rng(patNBParameters::the()->randomSeed);
		for (int i=0;i<count;i++){
			//DEBUG_MESSAGE(rng.nextDouble());
			sum+=rng.nextDouble();
		}

		double avg = sum/count;
		TS_ASSERT_DELTA(avg, 0.5, 1e-2);

		int bigest_int=4;
		unsigned long plain_int_sum=0;
		for (int i=0;i<count;i++){
			//DEBUG_MESSAGE(rng.nextInt(bigest_int-1));
			plain_int_sum+=rng.nextInt(bigest_int);
		}
		TS_ASSERT_DELTA(((double) plain_int_sum/count), ((double) (bigest_int-1)/2.0), 1e-2);

		 bigest_int=9;
		 plain_int_sum=0;
		for (int i=0;i<count;i++){
			//DEBUG_MESSAGE(rng.nextInt(bigest_int-1));
			plain_int_sum+=rng.nextInt(bigest_int);
		}
		TS_ASSERT_DELTA(((double) plain_int_sum/count), ((double) (bigest_int-1)/2.0), 1e-2);
		map<int,double> probas;
		int count_probas = 5;
		for(int i=0;i<count_probas;++i){
			probas[i]=0.1;
		}
		unsigned long sum_int_proba=0;
		for (int i=0;i<count;++i){
			//DEBUG_MESSAGE(rng.sampleWithProba(probas))
			sum_int_proba+=rng.sampleWithProba(probas);
		}

		TS_ASSERT_DELTA(((double) sum_int_proba/count), ((double) (count_probas-1)/2), 1e-2);
	}
};

#endif /* TESTRANDOMNUMBER_H_ */
