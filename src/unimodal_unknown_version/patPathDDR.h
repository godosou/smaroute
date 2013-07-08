#ifndef patPathDDR_h
#define patPathDDR_h

#include "patPath.h"
#include <vector>
#include "patType.h"
#include "patDDR.h"
class patPathJ;
class patGpsPoint;
#include "patArc.h"
#include "patNetwork.h"
class patPathDDR : public patDDR{
	public:
	patPathDDR();
		patPathDDR(patPathJ* thePath,
		 vector<patGpsPoint>* theGpsSequence);
		double getPathValue();
		vector<double> getArcValues();
	  friend ostream& operator<<(ostream& str, const patPathDDR& x) ;
		double rawAlgorithm( );

		void timeSpaceAlgorithm( patNetwork* theNetwork );

		double locationPDF(double position,
								unsigned long currGpsNumber,
								map<patArc*, map<char*, double> >* distanceToPath,
								double pointProbaDenumerator,
								vector<double>* theNodePosition,
								double alpha);
		
		double pointPDF(double position,
						unsigned long currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						double pointProbaDenumerator,
						vector<double>* theNodePosition,
						double alpha);
						
		double  calPointProba(unsigned long& currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						double pointProbaDenumerator,
						vector<double>* theNodePosition,
						double alpha);
		double  integration(double (patPathDDR::*fp)(double ,
						unsigned long ,
						map<patArc*, map<char*, double> >* ,
						double ,
						vector<double>* ,
						double ),
			 double a, 
			 double b, 
			 unsigned long currGpsNumber,
			 map<patArc*, map<char*, double> >* distanceToPath,
			 double pointProbaDenumerator,
			vector<double>* theNodePosition,
			 double alpha
			 );
		double  pointPDFNumerator(double position, 
								unsigned long currGpsNumber, 
								map<patArc*, map<char*, double> >* distanceToPath,
								vector<double>* theNodePosition,
								double alpha);
		
		double pointPDFDenumerator(double position, 
								unsigned long currGpsNumber, 
								map<patArc*, map<char*, double> >* distanceToPath,
																double nothing,
								vector<double>* theNodePosition,
								double alpha);
								
		double  distancePDF(double position,
						unsigned long currGpsNumber,
						map<patArc*, map<char*, double> >* distanceToPath,
						vector<double>* theNodePosition);
						
		double  priorPDF(double position, 
						unsigned long currGpsNumber,
						double alpha); 
						
		double  manipG(double position, 
					unsigned long currGpsNumber);
		
		double  getAlpha(double position,
					unsigned long currGpsNumber,
					map<patArc*, map<char*, double> >* distanceToPath,
					double pointProbaDenumerator,
					vector<double>* theNodePosition,
					double alpha);
					
		void  calDistance(vector<map<patArc*, map<char*, double> > >* theDistance,
						patNetwork* theNetwork);
		
		void  calNodePosition(vector<double>* theNodePosition);
		
		unsigned long  getArcNumberFromPosition(double position,
						vector<double>* theNodePosition);
						
		double  uniform();
		
		double  estimatePosition(unsigned long currGpsNumber,
								map<patArc*, map<char*, double> >* distanceToPath,
								double pointProbaDenumerator,
								vector<double>* theNodePosition,
								double alpha);
								
		double rawAlgorithm_average( );

	protected:
		double pathValue;
		vector<double> arcValues;
		vector<double> arcDistance;
		patPathJ* thePath;
		vector<patGpsPoint>* theGpsSequence;
		static double lastEstimate;
		static  double upperBound;
		static  double lowerBound;
		static unsigned long timeDiff; 
		static double x0;
		static double x1;
static double x2;
static double x3;

		//patString algoName;

};
#endif
