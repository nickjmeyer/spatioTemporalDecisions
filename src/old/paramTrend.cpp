#include "paramTrend.hpp"


unsigned int ParamTrend::initParsSize(const FixedData & fD){
  return 1U; // unsigned int literal
}


std::vector<std::string> ParamTrend::initNames(){
  return {"trend"};
}


void ParamTrend::initInternal(const FixedData & fD){
  prevTime = 0;
}


void ParamTrend::updateBefore(){
}


void ParamTrend::updateAfter(){
}


void ParamTrend::setFill(std::vector<double> & probs,
			 const SimData & sD,
			 const TrtData & tD,
			 const FixedData & fD,
			 const DynamicData & dD){
  double time = fD.forecastFlat ? std::min(sD.time,fD.trtStart-1) : sD.time;
  double val = *beg*double(time);
  prevTime = (unsigned int)(time);
  std::for_each(probs.begin(),probs.end(),
		[&val](double & x){
		  x += val;
		});
}


void ParamTrend::modFill(std::vector<double> & probs,
			 const SimData & sD,
			 const TrtData & tD,
			 const FixedData & fD,
			 const DynamicData & dD){
  double time = fD.forecastFlat ? std::min(sD.time,fD.trtStart-1) : sD.time;
  double val = *beg*(double(time) - double(prevTime));
  prevTime = (unsigned int)(time);
  std::for_each(probs.begin(),probs.end(),
		[&val](double & x){
		  x += val;
		});
}


std::vector<double> ParamTrend::partial(const int notNode,
					const int infNode,
					const SimData & sD,
					const TrtData & tD,
					const FixedData & fD,
					const DynamicData & dD){
  double time = fD.forecastFlat ? std::min(sD.time,fD.trtStart-1) : sD.time;
  return std::vector<double>(parsSize,time);
}
