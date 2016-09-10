#include <gflags/gflags.h>
#include "runM1MlesWnsMiss.hpp"

DEFINE_string(srcDir,"","Path to source directory");
DEFINE_bool(edgeToEdge,false,"Edge to edge transmission");

int main(int argc, char ** argv){
  gflags::ParseCommandLineFlags(&argc,&argv,true);
  njm::sett.setup(std::string(argv[0]),FLAGS_srcDir);

  // typedef ModelTimeExpCavesGDistTrendPowCon MG;

  typedef Model2GravityGDist MG;

  typedef ModelGDist ME;

  typedef System<MG,ME> S;

  typedef MyopicAgent<ME> MA;

  typedef WnsFeatures3<ME> F;
  typedef RankAgent<F,ME> RA;

  typedef M1SpOptim<S,RA,ME> SPO;

  typedef FitOnlyRunner<S,MA> R_MA;
  typedef OptimRunner<S,RA,SPO> R_RA;


  // S s;
  S s("obsData.txt");
  s.setEdgeToEdge(FLAGS_edgeToEdge);
  s.modelGen_r.setType(MLES);
  s.modelEst_r.setType(MLES);

  int numReps = 100;
  Starts starts("startingLocations.txt");

  MA ma;
  RA ra;

  ra.tp.jitterScale = -1;
  ra.setEdgeToEdge(FLAGS_edgeToEdge);

  SPO spo;
  spo.tp.fixSample = 1;

  R_MA r_ma;
  R_RA r_ra;


  RunStats rs;

  rs = r_ma.run(s,ma,numReps,s.fD.finalT,starts);
  njm::message("         Myopic: "
	       + njm::toString(rs.smean(),"")
	       + "  (" + njm::toString(rs.seMean(),"") + ")");

  rs = r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts);
  njm::message("  Policy Search: "
	       + njm::toString(rs.smean(),"")
	       + "  (" + njm::toString(rs.seMean(),"") + ")");

  return 0;
}
