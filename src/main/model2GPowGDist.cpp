#include "model2GPowGDist.hpp"

static std::vector<ParamBase *> genPars(){
    std::vector<ParamBase *> pars;
    pars.push_back(new ParamIntercept);
    pars.push_back(new ParamBeta2);
    pars.push_back(new ParamGravPowGDist);
    pars.push_back(new ParamTrt);
    return pars;
}

Model2GPowGDist::Model2GPowGDist(const FixedData & fD)
    : ModelBase("2GPowGDist",genPars(),fD){
}


Model2GPowGDist::Model2GPowGDist()
    : ModelBase("2GPowGDist",genPars()){
}


Model2GPowGDist::Model2GPowGDist(const Model2GPowGDist & m){
    int i, parsSize = m.pars.size();
    pars.clear();
    for(i = 0; i < parsSize; ++i)
        pars.push_back(m.pars.at(i)->clone());

    name = m.name;
    numPars = m.numPars;
    set = m.set;
    probs = m.probs;
    expitInfProbs = m.expitInfProbs;
    expitRevProbs = m.expitRevProbs;
    quick = m.quick;
    pcPartial = m.pcPartial;
    meanHit = m.meanHit;
    varHit = m.varHit;
    ready = m.ready;
    // numInfected = m.numInfected;
    // numNotInfec = m.numNotInfec;
    fitType = m.fitType;
    fixSample = m.fixSample;
    setEdgeToEdge(m.getEdgeToEdge());
}


Model2GPowGDist &
Model2GPowGDist::operator=(const Model2GPowGDist & m){
    if(this != & m){
        this->Model2GPowGDist::~Model2GPowGDist();
        new (this) Model2GPowGDist(m);
    }
    return *this;
}
