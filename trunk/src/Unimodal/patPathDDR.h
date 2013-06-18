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
		patReal getPathValue();
		vector<patReal> getArcValues();
	  friend ostream& operator<<(ostream& str, const patPathDDR& x) ;
		patReal rawAlgorithm( );

		void timeSpaceAlgorithm( patNetwork* theNetwork );

		patReal locationPDF(patReal position,
								patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								patReal pointProbaDenumerator,
								vector<patReal>* theNodePosition,
								patReal alpha);
		
		patReal pointPDF(patReal position,
						patULong currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						patReal pointProbaDenumerator,
						vector<patReal>* theNodePosition,
						patReal alpha);
						
		patReal  calPointProba(patULong& currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						patReal pointProbaDenumerator,
						vector<patReal>* theNodePosition,
						patReal alpha);
		patReal  integration(patReal (patPathDDR::*fp)(patReal ,
						patULong ,
						map<patArc*, map<char*, patReal> >* ,
						patReal ,
						vector<patReal>* ,
						patReal ),
			 patReal a, 
			 patReal b, 
			 patULong currGpsNumber,
			 map<patArc*, map<char*, patReal> >* distanceToPath,
			 patReal pointProbaDenumerator,
			vector<patReal>* theNodePosition,
			 patReal alpha
			 );
		patReal  pointPDFNumerator(patReal position, 
								patULong currGpsNumber, 
								map<patArc*, map<char*, patReal> >* distanceToPath,
								vector<patReal>* theNodePosition,
								patReal alpha);
		
		patReal pointPDFDenumerator(patReal position, 
								patULong currGpsNumber, 
								map<patArc*, map<char*, patReal> >* distanceToPath,
																patReal nothing,
								vector<patReal>* theNodePosition,
								patReal alpha);
								
		patReal  distancePDF(patReal position,
						patULong currGpsNumber,
						map<patArc*, map<char*, patReal> >* distanceToPath,
						vector<patReal>* theNodePosition);
						
		patReal  priorPDF(patReal position, 
						patULong currGpsNumber,
						patReal alpha); 
						
		patReal  manipG(patReal position, 
					patULong currGpsNumber);
		
		patReal  getAlpha(patReal position,
					patULong currGpsNumber,
					map<patArc*, map<char*, patReal> >* distanceToPath,
					patReal pointProbaDenumerator,
					vector<patReal>* theNodePosition,
					patReal alpha);
					
		void  calDistance(vector<map<patArc*, map<char*, patReal> > >* theDistance,
						patNetwork* theNetwork);
		
		void  calNodePosition(vector<patReal>* theNodePosition);
		
		patULong  getArcNumberFromPosition(patReal position,
						vector<patReal>* theNodePosition);
						
		patReal  uniform();
		
		patReal  estimatePosition(patULong currGpsNumber,
								map<patArc*, map<char*, patReal> >* distanceToPath,
								patReal pointProbaDenumerator,
								vector<patReal>* theNodePosition,
								patReal alpha);
								
		patReal rawAlgorithm_average( );

	protected:
		patReal pathValue;
		vector<patReal> arcValues;
		vector<patReal> arcDistance;
		patPathJ* thePath;
		vector<patGpsPoint>* theGpsSequence;
		static patReal lastEstimate;
		static  patReal upperBound;
		static  patReal lowerBound;
		static patULong timeDiff; 
		static patReal x0;
		static patReal x1;
static patReal x2;
static patReal x3;

		//patString algoName;

};
#endif
