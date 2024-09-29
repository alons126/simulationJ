#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
// #include "../targets.h"
#include <fstream>
#include <iostream>

#include "GENIE_to_LUND_Q2.C"

using namespace std;

/* root -l -q ConvertGENIE_Q2.C */

void ConvertGENIE_Q2()
{
    TString TARGET = "C12";

    // TString GENIE_TUNE = "GEM21_11a_00_000";
    TString GENIE_TUNE = "G18_10a_00_000";

    TString Q2_CUT;

    TString BEAM_E = "4029MeV"; // 4029MeV_def_Q2_th
    // TString BEAM_E = "5986MeV"; // 5986MeV_Q2_0_4_th

    int NUM_OF_FILES = 10;

    string TARGET_TYPE;

    int TARGET_A = 12;
    int TARGET_Z = 6;

    TString TRUTH_SAMPLE_INPUT_DIR;

    if (GENIE_TUNE.Data() == "G18_10a_00_000")
    {
        if (BEAM_E.Data() == "4029MeV")
        {
            TRUTH_SAMPLE_INPUT_DIR = "/w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/" + TARGET + "/" + GENIE_TUNE +
                                     "/Q2_th_test_samples/small_Q2_test_samples/4029MeV_def_Q2_th";
            TARGET_TYPE = "1-foil";
            Q2_CUT = "def_Q2_th";
        }
        else if (BEAM_E.Data() == "5986MeV")
        {
            TRUTH_SAMPLE_INPUT_DIR = "/w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/" + TARGET + "/" + GENIE_TUNE +
                                     "/Q2_th_test_samples/small_Q2_test_samples/5986MeV_Q2_0_4_th";
            TARGET_TYPE = "4-foil";
            Q2_CUT = "Q2_0_40";
        }
    }
    else if (GENIE_TUNE.Data() == "GEM21_11a_00_000")
    {
        if (BEAM_E.Data() == "4029MeV")
        {
            TRUTH_SAMPLE_INPUT_DIR = "/w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/" + TARGET + "/" + GENIE_TUNE +
                                     "/Q2_th_test_samples/small_Q2_test_samples/4029MeV_def_Q2_th";
            TARGET_TYPE = "1-foil";
            Q2_CUT = "def_Q2_th";
        }
        else if (BEAM_E.Data() == "5986MeV")
        {
            TRUTH_SAMPLE_INPUT_DIR = "/w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/" + TARGET + "/" + GENIE_TUNE +
                                     "/Q2_th_test_samples/small_Q2_test_samples/5986MeV_Q2_0_4_th";
            TARGET_TYPE = "4-foil";
            Q2_CUT = "Q2_0_40";
        }
    }

    // TString TRUTH_SAMPLE_ROOT_FILE_PREFIX = TARGET + "_" + GENIE_TUNE + "_Q2_" + Q2_CUT + "_" + BEAM_E;
    TString TRUTH_SAMPLE_ROOT_FILE_PREFIX = TARGET + "_" + GENIE_TUNE + "_" + Q2_CUT + "_" + BEAM_E;
    // TString TRUTH_SAMPLE_ROOT_FILE = "e_on_1000060120_2070MeV_0.gst.root";
    TString TRUTH_SAMPLE_ROOT_FILE = TRUTH_SAMPLE_ROOT_FILE_PREFIX + ".root";

    // TString RECO_SAMPLES_TOPDIR = "/Users/alon/Downloads";
    TString RECO_SAMPLES_TOPDIR = "/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples";
    TString RECO_SAMPLES_SUBDIR = "master-routine_validation_01-eScattering";
    TString RECO_SAMPLES_LUNDDIR = "lundfiles";

    gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET);
    gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE);
    gSystem->Exec("mkdir -p " + RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE + "/Q2_th_test_samples");

    TString RECO_SAMPLE_OUTPUT_DIR = RECO_SAMPLES_TOPDIR + "/" + TARGET + "/" + GENIE_TUNE + "/Q2_th_test_samples/" + BEAM_E;

    gSystem->Exec("rm -rf " + RECO_SAMPLE_OUTPUT_DIR);

    std::string beam_e = BEAM_E.Data();

    if (beam_e == "4029MeV")
    {
        GENIE_to_LUND_Q2(TARGET, GENIE_TUNE, BEAM_E,
                         (TRUTH_SAMPLE_INPUT_DIR + "/" + RECO_SAMPLES_SUBDIR + "/" + TRUTH_SAMPLE_ROOT_FILE),
                         RECO_SAMPLE_OUTPUT_DIR,
                         TRUTH_SAMPLE_ROOT_FILE_PREFIX,
                         NUM_OF_FILES,
                         TARGET_TYPE,
                         TARGET_A,
                         TARGET_Z,
                         0.02,  // start (4029MeV_def_Q2_th)
                         0.02,  // finish (4029MeV_def_Q2_th)
                                //  0.4,   // finish (4029MeV_def_Q2_th)
                         0.01); // delta (4029MeV_def_Q2_th)
    }
    else if (beam_e == "5986MeV")
    {
        GENIE_to_LUND_Q2(TARGET, GENIE_TUNE, BEAM_E,
                         (TRUTH_SAMPLE_INPUT_DIR + "/" + RECO_SAMPLES_SUBDIR + "/" + TRUTH_SAMPLE_ROOT_FILE),
                         RECO_SAMPLE_OUTPUT_DIR,
                         TRUTH_SAMPLE_ROOT_FILE_PREFIX,
                         NUM_OF_FILES,
                         TARGET_TYPE,
                         TARGET_A,
                         TARGET_Z,
                         0.4,   // start (5986MeV_Q2_0_4_th)
                         0.8,   // finish (5986MeV_Q2_0_4_th)
                         0.01); // delta (5986MeV_Q2_0_4_th)
    }
}