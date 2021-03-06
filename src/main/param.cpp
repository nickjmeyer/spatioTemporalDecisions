#include "param.hpp"

ParamBase::ParamBase(){
}

ParamBase::ParamBase(const ParamBase & p){
    pars = p.pars;
    beg = pars.begin();
    end = pars.end();
    offset = p.offset;
    totNumPars = p.totNumPars;
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


void ParamBase::setOffset(const unsigned int offset) {
    this->offset = offset;
}


void ParamBase::setTotNumPars(const unsigned int totNumPars) {
    this->totNumPars = totNumPars;
}


unsigned int ParamBase::getOffset() const {
    return this->offset;
}


unsigned int ParamBase::getTotNumPars() const {
    return this->totNumPars;
}


std::vector<bool> ParamBase::initToScale(){
    return std::vector<bool>(parsSize,true);
}


void ParamBase::save(const boost::filesystem::path & path) const {
    unsigned int i;
    for(i = 0; i < parsSize; ++i)
        njm::toFile(pars.at(i),
                (path / (names.at(i) + ".txt")).native(),
                std::ios_base::out);
}


void ParamBase::read(const boost::filesystem::path & path){
    unsigned int i;
    double val;
    std::vector<double> vals;
    vals.reserve(parsSize);
    for(i = 0; i < parsSize; ++i){
        njm::fromFile(val,
                (path / (names.at(i) + ".txt")).native());
        vals.push_back(val);
    }
    putPar(vals.begin());
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


bool ParamBase::setPar(const std::string & name, const double & val){
    std::vector<std::string> nameV = {name};
    return setPar(nameV,val);
}


bool ParamBase::setPar(const std::vector<std::string> & name,
        const double & val){
    std::vector<double> vals = pars;
    unsigned int i,j;
    bool found = false, foundAll = true;
    for(j = 0; j < name.size(); ++j){
        found = false;
        for(i = 0; i < parsSize; ++i){
            if(names[i] == name[j]){
                vals[i] = val;
                found = true;
            }
        }
        foundAll &= found;
    }
    putPar(vals.begin());

    return foundAll;
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
    std::vector<double> vals = pars;
    unsigned int i;
    for(i = 0; i < parsSize; ++i){
        if(toScale.at(i))
            vals.at(i)*=scale;
    }
    putPar(vals.begin());
}
