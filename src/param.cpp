#include "param.hpp"

ParamBase::ParamBase(){
}

ParamBase::ParamBase(const ParamBase & p){
  pars = p.pars;
  beg = pars.begin();
  end = pars.end();
  parsSize = p.parsSize;
  names = p.names;
  toScale = p.toScale;
}


void ParamBase::init(const FixedData & fD){
  // initialize the size
  parsSize = initParsSize(fD);

  // initialize vector to zero
  pars = std::vector<double> (parsSize,0);
  beg = pars.begin();
  end = pars.end();

  // names
  names = initNames();

  // toScale
  toScale = initToScale();

  // initialize the internal information
  initInternal(fD);
}


std::vector<bool> ParamBase::initToScale(){
  return std::vector<bool>(parsSize,true);
}


void ParamBase::save(const std::string & m) const {
  unsigned int i;
  for(i = 0; i < parsSize; ++i)
    njm::toFile(pars.at(i),
		njm::sett.srcExt("./Param"+m+"/"+names.at(i)+".txt"),
		std::ios_base::out);
}


void ParamBase::read(const std::string & m){
  unsigned int i;
  double val;
  for(i = 0; i < parsSize; ++i){
    njm::fromFile(val,
		  njm::sett.srcExt("./Param"+m+"/"+names.at(i)+".txt"));
    pars.at(i) = val;
  }
}


std::vector<double> ParamBase::getPar() const {
  return pars;
}


std::vector<double>
ParamBase::getPar(const std::vector<std::string> & name) const{
  unsigned int i,j;
  std::vector<double> res;
  for(i = 0; i < parsSize; ++i){
    for(j = 0; j < name.size(); ++j){
      if(name.at(j) == names.at(i))
	res.push_back(pars.at(i));
    }
  }
  return res;
}


std::vector<double>::const_iterator
ParamBase::putPar(std::vector<double>::const_iterator newParIt){
  updateBefore();
  std::vector<double>::iterator it;
  for(it = beg; it != end; ++it, ++newParIt)
    *it = *newParIt;
  updateAfter();
  return newParIt;
}


void ParamBase::setPar(const std::string & name, const double & val){
  unsigned int i;
  for(i = 0; i < parsSize; ++i){
    if(names.at(i) == name)
      pars.at(i) = val;
  }
}


void ParamBase::setPar(const std::vector<std::string> & name,
		       const double & val){
  unsigned int i;
  for(i = 0; i < name.size(); ++i){
    setPar(name.at(i),val);
  }
}


unsigned int ParamBase::size() const {
  return parsSize;
}


std::vector<double> ParamBase::partial2(const int notNode,
					const int infNode,
					const SimData & sD,
					const TrtData & tD,
					const FixedData & fD,
					const DynamicData & dD){
  return std::vector<double>(parsSize*parsSize,0);
}
  

void ParamBase::linScale(const double & scale){
  unsigned int i;
  for(i = 0; i < parsSize; ++i){
    if(toScale.at(i))
      pars.at(i)*=scale;
  }
}
