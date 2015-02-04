#include "anchorMan.hpp"


std::vector<double> AnchorManTunePar::getPar() const{
  return std::vector<double>();
}


void AnchorManTunePar::putPar(const std::vector<double> & par){
  // do nothing;
}


AnchorManTunePar::AnchorManTunePar(){
  numSamples = 100;
  cutoff = .95;
  freq = 6;
}

template class AnchorMan<System<GravityModel,GravityParam,
				GravityModel,GravityParam>,
			 RankToyAgent<ToyFeatures2<GravityModel,GravityParam>,
				      GravityModel,GravityParam>,
			 FeaturesInt<ToyFeatures2<GravityModel,GravityParam>,
				     GravityModel,GravityParam>,
			 GravityModel,GravityParam>;

template class AnchorMan<System<GravityModel,GravityParam,
				RangeModel,RangeParam>,
			 RankToyAgent<ToyFeatures2<RangeModel,RangeParam>,
				      RangeModel,RangeParam>,
			 FeaturesInt<ToyFeatures2<RangeModel,RangeParam>,
				     RangeModel,RangeParam>,
			 RangeModel,RangeParam>;


template <class System, class Agent, class Features,
	  class Model, class ModelParam>
AnchorMan<System,Agent,Features,Model,ModelParam>::AnchorMan(){
  // set switched time to max
  switched = std::numeric_limits<int>::max();
}


template <class System, class Agent, class Features,
	  class Model, class ModelParam>
void AnchorMan<System,Agent,Features,Model,ModelParam>
::optim(System system, Agent & agent){
  std::vector<double> w = agent.tp.getPar();

  if(switched >= system.sD.time){ // haven't made switch yet
    switched = system.sD.time;
  
    m1Opt.optim(system,agent);
    m1W = agent.tp.getPar();

    agent.tp.putPar(w);
    m2Opt.optim(system,agent);
    m2W = agent.tp.getPar();
  
    if(toSwitch(system,agent))
      agent.tp.putPar(m2W);
    else{
      agent.tp.putPar(m1W);
      switched+=tp.freq;
    }
  }
  else // made switch, use m2
    m2Opt.optim(system,agent);
}


template <class System, class Agent, class Features,
	  class Model, class ModelParam>
int AnchorMan<System,Agent,Features,Model,ModelParam>
::toSwitch(System system, Agent & agent){
  std::vector<double> samples;
  samples.reserve(tp.numSamples);
  int i, T = system.sD.time;
  for(i = 0; i < tp.numSamples; i++)
    samples.push_back(sampleNull(system,agent,T));

  int ind = std::ceil(tp.cutoff*tp.numSamples + 0.5) - 1;

  double testStat = njm::l2norm(m1W,m2W);

  return (testStat > samples.at(ind) ? 1 : 0);
}


template <class System, class Agent, class Features,
	  class Model, class ModelParam>
double AnchorMan<System,Agent,Features,Model,ModelParam>
::sampleNull(System & system, Agent & agent,
	     const int numYears){

  system.reset();
  int t;
  for(t = 0; t < numYears; t++){
    if(t >= system.fD.trtStart &&
       (((t - system.fD.trtStart) % system.fD.period) == 0)){
      system.modelEst.fit(system.sD,system.tD,system.fD,system.dD,
			  system.paramEst);
      m1Opt.optim(system,agent);
    }

    if(t >= system.fD.trtStart)
      agent.applyTrt(system.sD,system.tD,system.fD,system.dD,
		     system.modelEst,system.paramEst);
    system.updateStatus();
    system.nextPoint();
  }

  agent.tp.weights.ones();
  m2Opt.optim(system,agent);
  return njm::l2norm(m1W,agent.tp.getPar());
}

