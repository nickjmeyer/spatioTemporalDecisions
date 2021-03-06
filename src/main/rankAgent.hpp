#ifndef RANK_AGENT_HPP
#define RANK_AGENT_HPP


#include <armadillo>
#include <vector>
#include <queue>
#include <limits>
#include "system.hpp"
#include "timer.hpp"
#include "agent.hpp"
#include "dataDepth.hpp"
#include "tuneParam.hpp"
#include "features.hpp"
#include "toyFeatures5.hpp"
#include "wnsFeatures3.hpp"
#include "calcCentrality.hpp"


class RankTuneParam : public TuneParam {
public:
    std::vector<double> getPar() const;
    void putPar(const std::vector<double> & par);

    arma::colvec weights_r;
    arma::colvec weights;

    double jitterScale;

    bool shuffle;

    bool oneChunk;
};


template <class F, class M>
class RankAgent : public BaseAgent<M> {
public:
    RankAgent();

    void reset();

    virtual void applyTrt(const SimData & sD,
            TrtData & tD,
            const FixedData & fD,
            const DynamicData & dD,
            M & m);

    virtual double calcJitter();

    F f;

    arma::colvec infRanks;
    arma::colvec notRanks;

    int numAct;
    int numPre;

    RankTuneParam tp;

    std::string name;

    void setEdgeToEdge(const bool edgeToEdge);
    bool getEdgeToEdge() const;
};



#endif
