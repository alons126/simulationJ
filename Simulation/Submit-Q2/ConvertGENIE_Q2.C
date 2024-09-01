#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
//#include "../targets.h"
#include <fstream>
#include <iostream>

#include "GENIE_to_LUND.C"

using namespace std;

/* root -l -q ConvertGENIE_Q2.C */

void ConvertGENIE_Q2() {
  TString TARGET = "C12";
  TString GENIE_TUNE = "G18_10a_00_000";
  TString Q2_CUT = "0_03";
  TString BEAM_E = "2070MeV";

  TString TARGET_TYPE = "4-foil";
  int TARGET_A = 12;
  int TARGET_Z = 6;
  int NUM_OF_FILES = 5;

  TString TRUTH_SAMPLE_INPUT_DIR = "/w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/" + TARGET + "/" + GENIE_TUNE + "/Q2_th_test_samples/" + BEAM_E;
  TString TRUTH_SAMPLE_ROOT_FILE_PREFIX= TARGET + "_" + GENIE_TUNE + "_Q2_" + Q2_CUT + "_" + BEAM_E;
  TString TRUTH_SAMPLE_ROOT_FILE= TRUTH_SAMPLE_ROOT_FILE_PREFIX + ".root";

  TString RECO_SAMPLES_TOPDIR = "/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples";
  TString RECO_SAMPLES_SUBDIR = "master-routine_validation_01-eScattering";
  TString RECO_SAMPLES_LUNDDIR = "lundfiles";

  gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET);
  gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE);
  gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE + "/Q2_th_test_samples");

  TString RECO_SAMPLE_OUTPUT_DIR = RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE + "/Q2_th_test_samples/" + BEAM_E;

  gSystem->Exec("rm -rf " + RECO_SAMPLE_OUTPUT_DIR);
  gSystem->Exec("mkdir -p " + RECO_SAMPLE_OUTPUT_DIR);
  gSystem->Exec("mkdir -p " + RECO_SAMPLE_OUTPUT_DIR + "/lundfiles");
  gSystem->Exec("mkdir -p " + RECO_SAMPLE_OUTPUT_DIR + "/mchipo");
  gSystem->Exec("mkdir -p " + RECO_SAMPLE_OUTPUT_DIR + "/reconhipo");

  GENIE_to_LUND((TRUTH_SAMPLE_INPUT_DIR + "/" + RECO_SAMPLES_SUBDIR + "/" + TRUTH_SAMPLE_ROOT_FILE),
                (RECO_SAMPLE_OUTPUT_DIR + "/" + RECO_SAMPLES_LUNDDIR),
                TRUTH_SAMPLE_ROOT_FILE_PREFIX,
                NUM_OF_FILES,
                TARGET_TYPE,
                TARGET_A,
                TARGET_Z);

}