#include "runM2Miss.hpp"


int main(int argc, char ** argv){
  njm::sett.set(argc,argv);

  typedef GravityTimeInfExpCavesModel MG;
  
  typedef MultiModel ME;

  typedef System<MG,ME> S;

  typedef ToyFeatures2Multi<ME> F;
  typedef FeaturesInt<F,ME> FI;
  typedef RankAgent<F,ME> RA;

  typedef M2QOptim<S,RA,FI,ME> SPO;

  typedef OptimRunner<S,RA,SPO> R_RA;


  S s;
  s.modelGen_r.setType(MLE);
  s.modelEst_r.setType(MLE);

  int numReps = 96;
  Starts starts(numReps,s.fD.numNodes);
 
  RA ra; // running at the good starting weights

  ra.tp.weights_r.zeros(ra.f.numFeatures);
  ra.tp.weights_r(2) = 1;

  SPO spo;

  R_RA r_ra;
  

  njm::message("Priority Score: "
	       + njm::toString(r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts),
			       ""));

  return 0;
}

