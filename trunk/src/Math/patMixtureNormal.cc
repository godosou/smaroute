#include "patMixtureNormal.h"
#include "patErrMiscError.h"
#include <boost/math/distributions/normal.hpp> // for normal_distribution
#include "patDisplay.h"
#include "patError.h"
#include <sstream>
#include <vector>
using boost::math::normal;

patMixtureNormal::patMixtureNormal(int& components, 
	vector<double>& w,   
	vector<double>& mu,
	vector<double>& sigma, 
	patError *& err):
	m_components(components),m_w(w),m_mu(mu),m_sigma(sigma){

	bool flag = false;		
	double total_w=0.0;

	if(m_mu.size()==m_components  && m_sigma.size()==m_components && m_w.size()==m_components){
		for(vector<double>::const_iterator iter = m_w.begin();iter!=m_w.end();++iter){
			total_w+=*iter;
		}

		if (total_w< 1.0000001 && total_w > 0.999999){
			flag= true;
		}
	}

	if(flag !=true){
		stringstream str;
		str << "Wrong distribution" <<total_w;
		err = new patErrMiscError(str.str());
		WARNING(err->describe());
	}
}

double patMixtureNormal::pdf(const double& value) const{

	double proba = 0.0;
	for (int i = 0; i < m_components;++i) {

	boost::math::normal s(m_mu[i],m_sigma[i]);
		proba += m_w[i]
				* boost::math::pdf(s,value ) ;
	}
	return proba;
}
