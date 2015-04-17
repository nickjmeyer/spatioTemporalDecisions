#include "paramIntercept.hpp"


unsigned int ParamIntercept::initParsSize(const FixedData & fD){
  return 1U; // unsigned int literal
}


void ParamIntercept::initInternal(const FixedData & fD){
  parsOld = *beg;
}


void ParamIntercept::updateBefore(){
  parsOld = *beg;
}


void ParamIntercept::updateAfter(){
}


void ParamIntercept::setFill(std::vector<double> & probs,
			     const SimData & sD,
			     const TrtData & tD,
			     const FixedData & fD,
			     const DynamicData & dD) const {
  double val = *beg;
  std::for_each(probs.begin(),probs.end(),
		[&val](double & x){
		  x += val;
		});
}


void ParamIntercept::updateFill(std::vector<double> & probs,
				const SimData & sD,
				const TrtData & tD,
				const FixedData & fD,
				const DynamicData & dD) const {
  double diff = *beg - parsOld;
  std::for_each(probs.begin(),probs.end(),
		[&diff](double & x){
		  x += diff;
		});
}
							   
