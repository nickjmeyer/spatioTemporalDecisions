#ifndef ANCHOR_MAN_HPP__
#define ANCHOR_MAN_HPP__



#include <vector>
#include <algorithm>
#include "utilities.hpp"
#include "data.hpp"
#include "system.hpp"
#include "model.hpp"
#include "m1SgdOptim.hpp"
#include "m2SaOptim.hpp"
#include "rankAgent.hpp"


class AnchorManTunePar : public TuneParam {
 public:
  AnchorManTunePar();

  std::vector<double> getPar() const;
  void putPar(const std::vector<double> & par);

  int numSamples;
  double cutoff;
  int freq;
};


template <class S, class A, class F, class M>
class AnchorMan : BaseOptim<S,A,M> {
 public:
  AnchorMan();

  M1SgdOptim<System<M,M>,A,M> m1Opt;
  M2SaOptim<System<M,M>,A,F,M> m2Opt;

  std::vector<double> m1W,m2W;

  virtual void optim(const S & system,
		     A & agent);

  int toSwitch(System<M,M> & system,
	       A & agent, const int T);
  
  AnchorManTunePar tp;

  int switched;

  std::string name;
};



#endif
