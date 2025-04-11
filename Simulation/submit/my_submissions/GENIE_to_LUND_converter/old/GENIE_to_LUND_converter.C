#include <algorithm>  // for std::find
#include <fstream>
#include <iostream>

#include "../../../targets.h"
#include "/w/hallb-scshelf2102/clas12/asportes/2N-Analyzer/framework/classes/AMaps/AMap.cpp"
#include "TFile.h"
#include "TRandom3.h"
#include "TString.h"
#include "TTree.h"
#include "doubleToString.cpp"

using namespace std;

/* root -l -q GENIE_to_LUND_converter.C */

bool isInVector(int value, const std::vector<int> &vec) {
    // Check if the value is in the vector using std::find
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

void GENIE_to_LUND_converter(TString TARGET, TString GENIE_TUNE, TString BEAM_E, TString inputFile = "", TString lundPath = "./lundfiles/", TString outputFile = "", int nFiles = 800,
                             string target = "liquid", int A = 1, int Z = 1) {
    bool PrintOut = false;
    bool StepByStepPrintOut = false;

    std::string sample_target0 = TARGET.Data(), sample_genie_tune0 = GENIE_TUNE.Data(), sample_beamE0 = BEAM_E.Data();
    string sample_target1, sample_genie_tune1, sample_beamE1;
    string sample_target2, sample_genie_tune2, sample_beamE2;
    double beam_e;

    if (sample_target0 == "H1") {
        sample_target1 = "_H1";
        sample_target2 = "H1 ";
    } else if (sample_target0 == "D2") {
        sample_target1 = "_D2";
        sample_target2 = "D2 ";
    } else if (sample_target0 == "C12") {
        sample_target1 = "_C12";
        sample_target2 = "C12 ";
    } else if (sample_target0 == "Ar40") {
        sample_target1 = "_Ar40";
        sample_target2 = "Ar40 ";
    }

    if (sample_genie_tune0 == "G18_10a_00_000") {
        sample_genie_tune1 = "_G18";
        sample_genie_tune2 = "G18";
    } else if (sample_genie_tune0 == "GEM21_11a_00_000") {
        sample_genie_tune1 = "_SuSa";
        sample_genie_tune2 = "SuSa";
    }

    if (sample_beamE0 == "2070MeV") {
        sample_beamE1 = "_2GeV";
        sample_beamE2 = " @2GeV";
        beam_e = 2.07052;
    } else if (sample_beamE0 == "4029MeV") {
        sample_beamE1 = "_4GeV";
        sample_beamE2 = " @4GeV";
        beam_e = 4.02962;
    } else if (sample_beamE0 == "5986MeV") {
        sample_beamE1 = "_6GeV";
        sample_beamE2 = " @6GeV";
        beam_e = 5.98636;
    }

    if (PrintOut) {
        cout << "\033[33m\n\033[0m";
        cout << "\033[33minputFile = " << inputFile << "\n\033[0m";
        cout << "\033[33mlundPath = " << lundPath << "\n\033[0m";
        cout << "\033[33moutputFile = " << outputFile << "\n\033[0m";
        cout << "\033[33m\n\033[0m";
    }

    bool ContinueLooping = true;

    while (ContinueLooping) {
        cout << "\033[33m\n==============================================================\n\033[0m";
        cout << "\033[33mGenerating LUND files\n\033[0m";
        cout << "\033[33m==============================================================\n\033[0m" << endl;

#pragma region  // histograms

        // #region My Custom Fold
        std::vector<TH1D *> histList;
        std::vector<TString> pageTitles;

        TString HistNamePrefix_1e_cut = "TL_Q2cut_" + doubleToString(Q2_master) + "_1e_cut" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1e_cut =
            "Q^{2} for (e,e') with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for (e,e') with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_1e_cut_TL_all_int = new TH1D(HistNamePrefix_1e_cut, HistTitlePrefix_1e_cut + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1e_cut_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_1e_cut_TL_QE_only = new TH1D(HistNamePrefix_1e_cut, HistTitlePrefix_1e_cut + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1e_cut_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_1e_cut_TL_MEC_only = new TH1D(HistNamePrefix_1e_cut, HistTitlePrefix_1e_cut + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1e_cut_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_1e_cut_TL_RES_only = new TH1D(HistNamePrefix_1e_cut, HistTitlePrefix_1e_cut + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1e_cut_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_1e_cut_TL_DIS_only = new TH1D(HistNamePrefix_1e_cut, HistTitlePrefix_1e_cut + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1e_cut_TL_DIS_only);

        TString HistNamePrefix_1p = "TL_Q2cut_" + doubleToString(Q2_master) + "_1p" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1p =
            "Q^{2} for 1p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_1p_TL_all_int = new TH1D(HistNamePrefix_1p, HistTitlePrefix_1p + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1p_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_1p_TL_QE_only = new TH1D(HistNamePrefix_1p, HistTitlePrefix_1p + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1p_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_1p_TL_MEC_only = new TH1D(HistNamePrefix_1p, HistTitlePrefix_1p + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1p_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_1p_TL_RES_only = new TH1D(HistNamePrefix_1p, HistTitlePrefix_1p + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1p_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_1p_TL_DIS_only = new TH1D(HistNamePrefix_1p, HistTitlePrefix_1p + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1p_TL_DIS_only);

        TString HistNamePrefix_1n = "TL_Q2cut_" + doubleToString(Q2_master) + "_1n" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1n =
            "Q^{2} for 1n with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1n with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_1n_TL_all_int = new TH1D(HistNamePrefix_1n, HistTitlePrefix_1n + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_1n_TL_QE_only = new TH1D(HistNamePrefix_1n, HistTitlePrefix_1n + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_1n_TL_MEC_only = new TH1D(HistNamePrefix_1n, HistTitlePrefix_1n + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_1n_TL_RES_only = new TH1D(HistNamePrefix_1n, HistTitlePrefix_1n + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_1n_TL_DIS_only = new TH1D(HistNamePrefix_1n, HistTitlePrefix_1n + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n_TL_DIS_only);

        TString HistNamePrefix_1N = "TL_Q2cut_" + doubleToString(Q2_master) + "_1N" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1N =
            "Q^{2} for 1N with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1N with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_1N_TL_all_int = new TH1D(HistNamePrefix_1N, HistTitlePrefix_1N + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1N_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_1N_TL_QE_only = new TH1D(HistNamePrefix_1N, HistTitlePrefix_1N + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1N_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_1N_TL_MEC_only = new TH1D(HistNamePrefix_1N, HistTitlePrefix_1N + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1N_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_1N_TL_RES_only = new TH1D(HistNamePrefix_1N, HistTitlePrefix_1N + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1N_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_1N_TL_DIS_only = new TH1D(HistNamePrefix_1N, HistTitlePrefix_1N + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1N_TL_DIS_only);

        TString HistNamePrefix_2p = "TL_Q2cut_" + doubleToString(Q2_master) + "_2p" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_2p =
            "Q^{2} for 2p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 2p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_2p_TL_all_int = new TH1D(HistNamePrefix_2p, HistTitlePrefix_2p + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2p_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_2p_TL_QE_only = new TH1D(HistNamePrefix_2p, HistTitlePrefix_2p + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2p_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_2p_TL_MEC_only = new TH1D(HistNamePrefix_2p, HistTitlePrefix_2p + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2p_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_2p_TL_RES_only = new TH1D(HistNamePrefix_2p, HistTitlePrefix_2p + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2p_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_2p_TL_DIS_only = new TH1D(HistNamePrefix_2p, HistTitlePrefix_2p + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2p_TL_DIS_only);

        TString HistNamePrefix_1n1p = "TL_Q2cut_" + doubleToString(Q2_master) + "_1n1p" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1n1p =
            "Q^{2} for 1n1p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1n1p with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_1n1p_TL_all_int = new TH1D(HistNamePrefix_1n1p, HistTitlePrefix_1n1p + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n1p_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_1n1p_TL_QE_only = new TH1D(HistNamePrefix_1n1p, HistTitlePrefix_1n1p + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n1p_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_1n1p_TL_MEC_only = new TH1D(HistNamePrefix_1n1p, HistTitlePrefix_1n1p + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n1p_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_1n1p_TL_RES_only = new TH1D(HistNamePrefix_1n1p, HistTitlePrefix_1n1p + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n1p_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_1n1p_TL_DIS_only = new TH1D(HistNamePrefix_1n1p, HistTitlePrefix_1n1p + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_1n1p_TL_DIS_only);

        TString HistNamePrefix_2N = "TL_Q2cut_" + doubleToString(Q2_master) + "_2N" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_2N =
            "Q^{2} for 2N with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}] on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 2N with Q^{2}_{TL cut} #geq " + doubleToStringWithPercision(Q2_master) + " [GeV^{2}/c^{2}]");
        TH1D *Q2_2N_TL_all_int = new TH1D(HistNamePrefix_2N, HistTitlePrefix_2N + " (All Int.);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2N_TL_all_int);
        pageTitles.push_back("");
        TH1D *Q2_2N_TL_QE_only = new TH1D(HistNamePrefix_2N, HistTitlePrefix_2N + " (QE Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2N_TL_QE_only);
        pageTitles.push_back("");
        TH1D *Q2_2N_TL_MEC_only = new TH1D(HistNamePrefix_2N, HistTitlePrefix_2N + " (MEC Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2N_TL_MEC_only);
        pageTitles.push_back("");
        TH1D *Q2_2N_TL_RES_only = new TH1D(HistNamePrefix_2N, HistTitlePrefix_2N + " (RES Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2N_TL_RES_only);
        pageTitles.push_back("");
        TH1D *Q2_2N_TL_DIS_only = new TH1D(HistNamePrefix_2N, HistTitlePrefix_2N + " (DIS Only);Q^{2} [GeV^{2}/c^{2}]", 100, Q2_llim, Q2_ulim);
        histList.push_back(Q2_2N_TL_DIS_only);

#pragma endregion

        cout << "\033[33m\n\033[0m";

        TString TempOutPutPath = "/Q2_" + doubleToString(Q2_master);

        if (PrintOut) {
            cout << "\033[33mTempOutPutPath = " << TempOutPutPath << "\n\033[0m";
            cout << "\033[33mlundPath = " << lundPath << "\n\n\033[0m";
        }

        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath);
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/lundfiles");
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/mchipo");
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/reconhipo");

        TString TempLundPath = lundPath + TempOutPutPath + "/lundfiles";

        cout << "\033[33mLund out path for Q2 = " << doubleToString(Q2_master) << " cut:\033[0m" << endl;
        cout << TempLundPath << "\n\n";

        // Read in target parameter files
        cout << "\033[33mConverting file:\033[0m" << endl;
        cout << inputFile << "\n\n";

        TFile *inFile = new TFile(inputFile);

        TString Q2_cut_TString = doubleToString(Q2_master);
        outputFile = TARGET + "_" + GENIE_TUNE + "_" + BEAM_E;

        cout << "\033[33mMaking LUND files with prefix:\033[0m" << endl;
        cout << outputFile << "\n\n";

        TTree *T = (TTree *)inFile->Get("gst");

        double RES_ID = 0.;  // WAS targP = 0.; // polarization
        double beamP = 0.;   // polarization
        Int_t interactN = 1;
        int beamType = 11;

        double beamE = beam_e;  // GeV

        Bool_t qel;
        Bool_t mec;
        Bool_t res;
        Bool_t dis;
        // will be coded into 1,2,3,4

        Int_t resid;

        // Final state particles
        Int_t nf;
        Int_t pdgf[125];    //[nf]
        Double_t Ef[125];   //[nf]
        Double_t pxf[125];  //[nf]
        Double_t pyf[125];  //[nf]
        Double_t pzf[125];  //[nf]

        // Electron info
        Double_t El;
        Double_t pxl;
        Double_t pyl;
        Double_t pzl;

        // My additions:
        Double_t Q2;
        Int_t nfp;
        Int_t nfn;

        T->SetBranchAddress("qel", &qel);
        T->SetBranchAddress("mec", &mec);
        T->SetBranchAddress("res", &res);
        T->SetBranchAddress("dis", &dis);
        // Added by J. L. Barrow
        T->SetBranchAddress("resid", &resid);

        T->SetBranchAddress("pdgf", pdgf);
        T->SetBranchAddress("nf", &nf);
        T->SetBranchAddress("Ef", Ef);
        T->SetBranchAddress("pxf", pxf);
        T->SetBranchAddress("pyf", pyf);
        T->SetBranchAddress("pzf", pzf);

        T->SetBranchAddress("El", &El);
        T->SetBranchAddress("pxl", &pxl);
        T->SetBranchAddress("pyl", &pyl);
        T->SetBranchAddress("pzl", &pzl);

        T->SetBranchAddress("Q2", &Q2);
        T->SetBranchAddress("nf", &nf);
        T->SetBranchAddress("nfp", &nfp);
        T->SetBranchAddress("nfn", &nfn);

        int nEvents = T->GetEntries();
        cout << "\033[33mNumber of events (nEvents):\033[0m " << nEvents << "\n\n";

        if (PrintOut) { cout << "\033[33m\nQ2_master = " << Q2_master << "\n\033[0m"; }

        TString formatstring, outstring;

        // Check the number of files is not more than what is in the file
        if (nFiles > nEvents / 10000) { nFiles = nEvents / 10000; }

        int iFiles = 1;

        int MaxEventsPerFile = 10000;
        int j = 0;

        cout << "\033[33mNumber of lund files (nFiles):\033[0m " << nFiles << "\n";
        cout << "\033[33mMax number of Events/file:\033[0m " << MaxEventsPerFile << "\n";

        if (PrintOut) {
            cout << "\033[33mnFiles = " << nFiles << "\n\033[0m";
            cout << "\033[33mNumber of events = " << nEvents << "\n\033[0m";
            cout << "\033[33mMaxEventsPerFile = " << MaxEventsPerFile << "\n\033[0m";
        }

        cout << "\033[33m\n\033[0m";

        int Q2_above_cut_counter = 0;
        int Q2_above_cut_counter_debug = 0;
        int matched = 0;
        int totalFilledEvents = 0;
        vector<int> Q2_above_cut_ind;
        vector<int> Filled_events_ind;

        cout << "\033[33mCounting events with Q2 >= " << doubleToStringWithPercision(Q2_master) << "...\n\n\033[0m";
        for (int k = 0; k < nEvents; k++) {
            T->GetEntry(k);
            if (Q2 >= Q2_master) {
                ++Q2_above_cut_counter;
                Q2_above_cut_ind.push_back(k);
            }
        }

        cout << "\033[33mGenerating lund files...\n\n\033[0m";
        while (iFiles <= nFiles) {
            int FilledEvents = 0;

            if (PrintOut) {
                cout << "\033[33m\n-----------------------------------------------------------------\n\033[0m";
                cout << "\033[33miFiles = " << iFiles << "\n\033[0m";
                cout << "\033[33mj = " << j << "\n\n\033[0m";
            }

            TString outfilename = Form("%s/%s_%d.txt", TempLundPath.Data(), outputFile.Data(), iFiles);

            cout << "\033[33moutfilename:\033[0m " << outfilename << "\n";

            ofstream outfile;
            outfile.open(outfilename);

            if (!outfile.is_open()) { cout << "\033[33moutfile file cannot be created! Exiting...\033[0m", exit(0); }

            while ((FilledEvents < MaxEventsPerFile) && (totalFilledEvents < Q2_above_cut_ind.size())) {
                T->GetEntry(j);

                if (StepByStepPrintOut) { cout << "\033[33mLooping over events...\n\033[0m"; }

                if (Q2 >= Q2_master && isInVector(j, Q2_above_cut_ind)) {
                    if (isInVector(j, Filled_events_ind)) {
                        cout << "\033[33m\nExited!!!!\n\033[0m";
                        cout << "\033[33m\niFiles = " << iFiles << "\n\033[0m";
                        cout << "\033[33mQ2 = " << Q2 << "\n\033[0m";
                        cout << "\033[33mFilledEvents = " << FilledEvents << "\n\033[0m";
                        cout << "\033[33mtotalFilledEvents = " << totalFilledEvents << "\n\033[0m";
                        cout << "\033[33mj = " << j << "\n\033[0m";
                        cout << "\033[33m\n\033[0m";
                        exit(0);
                    }

                    Q2_1e_cut_TL_all_int->Fill(Q2);

                    if (qel) {
                        Q2_1e_cut_TL_QE_only->Fill(Q2);
                    } else if (mec) {
                        Q2_1e_cut_TL_MEC_only->Fill(Q2);
                    } else if (res) {
                        Q2_1e_cut_TL_RES_only->Fill(Q2);
                    } else if (dis) {
                        Q2_1e_cut_TL_DIS_only->Fill(Q2);
                    }

                    if (nf == 1) {
                        Q2_1N_TL_all_int->Fill(Q2);

                        if (qel) {
                            Q2_1N_TL_QE_only->Fill(Q2);
                        } else if (mec) {
                            Q2_1N_TL_MEC_only->Fill(Q2);
                        } else if (res) {
                            Q2_1N_TL_RES_only->Fill(Q2);
                        } else if (dis) {
                            Q2_1N_TL_DIS_only->Fill(Q2);
                        }

                        if (nfp == 1) {
                            Q2_1p_TL_all_int->Fill(Q2);

                            if (qel) {
                                Q2_1p_TL_QE_only->Fill(Q2);
                            } else if (mec) {
                                Q2_1p_TL_MEC_only->Fill(Q2);
                            } else if (res) {
                                Q2_1p_TL_RES_only->Fill(Q2);
                            } else if (dis) {
                                Q2_1p_TL_DIS_only->Fill(Q2);
                            }
                        } else if (nfn == 1) {
                            Q2_1n_TL_all_int->Fill(Q2);

                            if (qel) {
                                Q2_1n_TL_QE_only->Fill(Q2);
                            } else if (mec) {
                                Q2_1n_TL_MEC_only->Fill(Q2);
                            } else if (res) {
                                Q2_1n_TL_RES_only->Fill(Q2);
                            } else if (dis) {
                                Q2_1n_TL_DIS_only->Fill(Q2);
                            }
                        }
                    } else if (nf == 2) {
                        Q2_2N_TL_all_int->Fill(Q2);

                        if (qel) {
                            Q2_2N_TL_QE_only->Fill(Q2);
                        } else if (mec) {
                            Q2_2N_TL_MEC_only->Fill(Q2);
                        } else if (res) {
                            Q2_2N_TL_RES_only->Fill(Q2);
                        } else if (dis) {
                            Q2_2N_TL_DIS_only->Fill(Q2);
                        }

                        if (nfp == 2) {
                            Q2_2p_TL_all_int->Fill(Q2);

                            if (qel) {
                                Q2_2p_TL_QE_only->Fill(Q2);
                            } else if (mec) {
                                Q2_2p_TL_MEC_only->Fill(Q2);
                            } else if (res) {
                                Q2_2p_TL_RES_only->Fill(Q2);
                            } else if (dis) {
                                Q2_2p_TL_DIS_only->Fill(Q2);
                            }
                        } else if (nfn == 1 && nfp == 1) {
                            Q2_1n1p_TL_all_int->Fill(Q2);

                            if (qel) {
                                Q2_1n1p_TL_QE_only->Fill(Q2);
                            } else if (mec) {
                                Q2_1n1p_TL_MEC_only->Fill(Q2);
                            } else if (res) {
                                Q2_1n1p_TL_RES_only->Fill(Q2);
                            } else if (dis) {
                                Q2_1n1p_TL_DIS_only->Fill(Q2);
                            }
                        }
                    }

                    // Stores reaction mechanism qel = 1, mec = 2, rec = 3, dis=4
                    double code = 0.;

                    if (qel) {
                        code = 1.;
                    } else if (mec) {
                        code = 2.;
                    } else if (res) {
                        code = 3.;
                    } else if (dis) {
                        code = 4.;
                    }

                    if (code < .01) { continue; }

                    RES_ID = double(resid);

                    int nf_mod = 1;
                    for (int iPart = 0; iPart < nf; iPart++) {
                        if (pdgf[iPart] == 2212)
                            nf_mod++;
                        else if (pdgf[iPart] == 2112)
                            nf_mod++;
                        else if (pdgf[iPart] == 211)
                            nf_mod++;
                        else if (pdgf[iPart] == -211)
                            nf_mod++;
                    }

                    // LUND header for the event:
                    formatstring = "%i \t %i \t %i \t %f \t %f \t %i \t %f \t %i \t %d \t %.2f \n";
                    outstring = Form(formatstring, nf_mod, A, Z, RES_ID /*targP*/, beamP, beamType, beamE, interactN, FilledEvents, code);
                    // outstring = Form(formatstring, nf_mod, A, Z, RES_ID /*targP*/, beamP, beamType, beamE, interactN, j, code);
                    outfile << outstring;

                    auto vtx = randomVertex(target);  // get vertex of event

                    int part_num = 0;
                    // electron
                    outfile << addParticle(1, 1, 11, TVector3(pxl, pyl, pzl), mass_e, vtx);
                    part_num++;

                    for (int iPart = 0; iPart < nf; iPart++) {
                        if (pdgf[iPart] == 2212) {  // p
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_p, vtx);
                        } else if (pdgf[iPart] == 2112) {  // n
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_n, vtx);
                        } else if (pdgf[iPart] == 211) {  // pi+
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                        } else if (pdgf[iPart] == -211) {  // pi-
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                        }
                    }

                    ++matched;
                    ++FilledEvents;
                    ++totalFilledEvents;
                    ++Q2_above_cut_counter_debug;
                    Filled_events_ind.push_back(j);

                    if (PrintOut) {
                        cout << "\033[33m\niFiles = " << iFiles << "\n\033[0m";
                        cout << "\033[33mQ2 = " << Q2 << "\n\033[0m";
                        cout << "\033[33mFilledEvents = " << FilledEvents << "\n\033[0m";
                        cout << "\033[33mtotalFilledEvents = " << totalFilledEvents << "\n\033[0m";
                        cout << "\033[33mj = " << j << "\n\033[0m";
                        cout << "\033[33m\n\033[0m";
                    }

                    if ((j >= nEvents)) { break; }
                }

                ++j;

                if (j > nEvents) {
                    cout << "\033[33mj is larger than nEvents! Breaking...\n\033[0m";
                    break;
                }
            }

            if (StepByStepPrintOut) { cout << "\033[33mLeft event loop!\n\033[0m"; }

            outfile.close();

            if (totalFilledEvents >= Q2_above_cut_ind.size()) {
                break;
            } else {
                ++iFiles;
            }
        }

        cout << "\033[33m\n\nSaving debugging histograms...\n\n\033[0m\033[0m";

        std::string TempFilePath0 = (lundPath + TempOutPutPath).Data();
        std::string TempFilePath = TempFilePath0 + "/";
        std::string pdfFileName = TempFilePath + sample_target0 + "_" + sample_genie_tune0 + "_Q2_" + doubleToString(Q2_master) + "_" + sample_beamE0 + "_plots.pdf";
        const char *pdfFile = pdfFileName.c_str();

        TList *plotsList = new TList();
        string listName = TempFilePath + sample_target0 + "_" + sample_genie_tune0 + "_Q2_" + doubleToString(Q2_master) + "_" + sample_beamE0 + "_plots.root";
        const char *TListName = listName.c_str();

        // Create a canvas
        TCanvas *canvas = new TCanvas("canvas", "Canvas for saving histograms", 800, 600);
        canvas->cd()->SetGrid();
        canvas->cd()->SetBottomMargin(0.14), canvas->cd()->SetLeftMargin(0.18), canvas->cd()->SetRightMargin(0.12);

        // offset the multi-page PDF
        canvas->Print(Form("%s[", pdfFile));  // Open the PDF file

        // Loop through the list of histograms
        for (int i = 0; i < histList.size(); i++) {
            canvas->cd();  // Select the canvas
            canvas->Clear();

            std::string pageTitleTemp = pageTitles.at(i).Data();

            if (pageTitleTemp != "") {
                // cout << "\033[33mpageTitleTemp = " << pageTitleTemp << "\n\033[0m";

                TLatex text;
                text.SetTextAlign(22);
                text.SetTextSize(0.05);
                text.DrawLatexNDC(0.5, 0.5, pageTitles.at(i));
                canvas->Update();
                canvas->Print(pdfFile);  // Save the current canvas (histogram) to the PDF
            }

            canvas->Clear();

            canvas->cd();
            histList.at(i)->GetXaxis()->SetTitleSize(0.06);
            histList.at(i)->GetXaxis()->SetLabelSize(0.0425);
            histList.at(i)->GetXaxis()->CenterTitle(true);
            histList.at(i)->GetYaxis()->SetTitle("Number of events");
            histList.at(i)->GetYaxis()->SetTitleSize(0.06);
            histList.at(i)->GetYaxis()->SetLabelSize(0.0425);
            histList.at(i)->GetYaxis()->CenterTitle(true);
            histList.at(i)->SetLineWidth(2);
            histList.at(i)->SetLineStyle(0);
            histList.at(i)->SetLineColor(kBlue);
            histList.at(i)->Draw();  // Draw the histogram on the canvas
            canvas->Print(pdfFile);  // Save the current canvas (histogram) to the PDF
            plotsList->Add(histList.at(i));
        }

        // End the multi-page PDF
        canvas->Print(Form("%s]", pdfFile));  // Close the PDF file

        TFile *plotsOutRootFile = new TFile(TListName, "recreate");
        plotsOutRootFile->cd();
        plotsList->Write();
        plotsOutRootFile->Write();
        plotsOutRootFile->Close();

        delete canvas;
        delete plotsList;
        delete plotsOutRootFile;

        for (auto &h : histList) { delete h; }

        histList.clear();

        cout << "\033[33m\n- Summary for Q2 = " << doubleToString(Q2_master) << " cut ----------------------------------\n\033[0m";
        cout << "\033[33mCounted #(events) above cut:\033[0m " << Q2_above_cut_counter << "\n\033[0m";
        cout << "\033[33m#(filled events) above cut:\033[0m " << Q2_above_cut_counter_debug << "\n\n\033[0m";

        Q2_master = Q2_master + dQ2;
    }
}
