#include "param.hpp"


void ParamBase::init(const FixedData & fD){
  // scalar parameter

  // initialize the size
  parsSize = initParsSize(fD);

  // initialize vector to zero
  pars = std::vector<double> (parsSize,0);
  beg = pars.begin();
  end = pars.end();

  // initialize the internal information
  initInternal(fD);
}


std::vector<double> ParamBase::getPar() const {
  return pars;
}


std::vector<double>::iterator
ParamBase::putPar(std::vector<double>::iterator newParIt){
  updateBefore();
  std::vector<double>::iterator it;
  for(it = beg; it != end; ++it, ++newParIt)
    *it = *newParIt;
  updateAfter();
  return newParIt;
}



