#include <gflags/gflags.h>
#include <glog/logging.h>
#include "runM1MlesMiss.hpp"

using namespace google;
using namespace gflags;


DEFINE_string(srcDir,"","Path to source directory");
DEFINE_bool(edgeToEdge,false,"Edge to edge transmission");
DEFINE_bool(dryRun,false,"Do not execute main");

int main(int argc, char ** argv){
    InitGoogleLogging(argv[0]);
    ParseCommandLineFlags(&argc,&argv,true);
    if(!FLAGS_dryRun) {
        njm::sett.setup(std::string(argv[0]),FLAGS_srcDir);

        njm::toFile(FLAGS_edgeToEdge,
                njm::sett.datExt("edgeToEdge_flag_",".txt"));

        if(FLAGS_edgeToEdge) {

            // typedef ModelTimeExpCavesGPowGDistTrendPowCon MG;

            typedef Model2EdgeToEdge MG;

            typedef ModelIntercept ME;

            typedef System<MG,ME> S;

            typedef MyopicAgent<ME> MA;

            typedef ToyFeatures5<ME> F;
            typedef RankAgent<F,ME> RA;

            typedef M1SpOptim<S,RA,ME> SPO;

            typedef FitOnlyRunner<S,MA> R_MA;
            typedef OptimRunner<S,RA,SPO> R_RA;



            S s;
            s.setEdgeToEdge(FLAGS_edgeToEdge);
            s.modelGen_r.setType(MLES);
            s.modelEst_r.setType(MLES);

            int numReps = 100;
            Starts starts(numReps,s.fD.numNodes);

            MA ma;
            RA ra;

            ra.tp.jitterScale = -1;
            ra.setEdgeToEdge(FLAGS_edgeToEdge);

            ma.setEdgeToEdge(FLAGS_edgeToEdge);

            SPO spo;
            spo.tp.fixSample = 1;

            R_MA r_ma;
            R_RA r_ra;


            RunStats rs;

            rs = r_ma.run(s,ma,numReps,s.fD.finalT,starts);
            njm::message("         Myopic: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("myopic, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            rs = r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts);
            njm::message("  Policy Search: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("ps, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);
        } else {

            // typedef ModelTimeExpCavesGPowGDistTrendPowCon MG;

            typedef Model2GravityEDist MG;

            typedef ModelEDist ME;

            typedef System<MG,ME> S;

            typedef MyopicAgent<ME> MA;

            typedef ToyFeatures5<ME> F;
            typedef RankAgent<F,ME> RA;

            typedef M1SpOptim<S,RA,ME> SPO;

            typedef FitOnlyRunner<S,MA> R_MA;
            typedef OptimRunner<S,RA,SPO> R_RA;



            S s;
            s.setEdgeToEdge(FLAGS_edgeToEdge);
            s.modelGen_r.setType(MLES);
            s.modelEst_r.setType(MLES);

            int numReps = 100;
            Starts starts(numReps,s.fD.numNodes);

            MA ma;
            RA ra;

            ra.tp.jitterScale = -1;
            ra.setEdgeToEdge(FLAGS_edgeToEdge);

            ma.setEdgeToEdge(FLAGS_edgeToEdge);

            SPO spo;
            spo.tp.fixSample = 1;

            R_MA r_ma;
            R_RA r_ra;


            RunStats rs;

            rs = r_ma.run(s,ma,numReps,s.fD.finalT,starts);
            njm::message("         Myopic: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("myopic, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);

            rs = r_ra.run(s,ra,spo,numReps,s.fD.finalT,starts);
            njm::message("  Policy Search: "
                    + njm::toString(rs.sMean(),"")
                    + "  (" + njm::toString(rs.seMean(),"") + ")");
            njm::toFile("ps, "+ njm::toString(rs.sMean(),"") +
                    ", " + njm::toString(rs.seMean(),"") + "\n",
                    njm::sett.datExt("results_",".txt"),
                    std::ios_base::app);
        }
    }
    return 0;
}
