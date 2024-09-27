#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "../targets.h"
#include <fstream>
#include <iostream>

#include "doubleToString.cpp"

using namespace std;

void GENIE_to_LUND(TString TARGET, TString GENIE_TUNE, TString BEAM_E,
                   TString inputFile = "", TString lundPath = "./lundfiles/", TString outputFile = "",
                   int nFiles = 800, string target = "liquid", int A = 1, int Z = 1,
                   double Q2_min = 0, double Q2_max = 1., double dQ2 = 0.02)
{
    bool PrintOut = true;
    bool CountQ2AndExit = false;

    std::string sample_target0 = TARGET.Data(), sample_genie_tune0 = GENIE_TUNE.Data(), sample_beamE0 = BEAM_E.Data();
    string sample_target1, sample_genie_tune1, sample_beamE1;
    string sample_target2, sample_genie_tune2, sample_beamE2;
    double Q2_ulim, Q2_llim;

    if (sample_target0 == "H1")
    {
        sample_target1 = "_H1";
        sample_target2 = "H1 ";
    }
    else if (sample_target0 == "D2")
    {
        sample_target1 = "_D2";
        sample_target2 = "D2 ";
    }
    else if (sample_target0 == "C12")
    {
        sample_target1 = "_C12";
        sample_target2 = "C12 ";
    }
    else if (sample_target0 == "Ar40")
    {
        sample_target1 = "_Ar40";
        sample_target2 = "Ar40 ";
    }

    if (sample_genie_tune0 == "G18_10a_00_000")
    {
        sample_genie_tune1 = "_G18";
        sample_genie_tune2 = "G18";
    }
    else if (sample_genie_tune0 == "GEM21_11a_00_000")
    {
        sample_genie_tune1 = "_SuSa";
        sample_genie_tune2 = "SuSa";
    }

    if (sample_beamE0 == "2070MeV")
    {
        sample_beamE1 = "_2GeV";
        sample_beamE2 = " @2GeV";
        Q2_ulim = 0.5;
        Q2_llim = 0.;
    }
    else if (sample_beamE0 == "4029MeV")
    {
        sample_beamE1 = "_4GeV";
        sample_beamE2 = " @4GeV";
        Q2_ulim = 1.;
        Q2_llim = 0.;
    }
    else if (sample_beamE0 == "5986MeV")
    {
        sample_beamE1 = "_6GeV";
        sample_beamE2 = " @6GeV";
        Q2_ulim = 2.;
        Q2_llim = 0.;
    }

    if (PrintOut)
    {
        cout << "\n";
        cout << "inputFile = " << inputFile << endl;
        cout << "lundPath = " << lundPath << endl;
        cout << "outputFile = " << outputFile << endl;
        cout << "\n";
    }

    double Q2_master = Q2_min;

    while (Q2_master <= Q2_max)
    {
        // TODO: change Q2 in lund file name according to Q2_master
        // TODO: apply the Q2 cut on the branch
        // TODO: fix the event fill proccess

        gDirectory->Clear();

        // #region My Custom Fold
        std::vector<TH1D *> histList;
        std::vector<TString> pageTitles;

        TString HistNamePrefix_1e_cut = "TL_Q2cut_" + doubleToString(Q2_master) + "_1e_cut" + sample_target1 + sample_genie_tune1 + sample_beamE1;
        TString HistTitlePrefix_1e_cut = "Q^{2} for (e,e') with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for (e,e')");
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
        TString HistTitlePrefix_1p = "Q^{2} for 1p with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1p");
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
        TString HistTitlePrefix_1n = "Q^{2} for 1n with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1n");
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
        TString HistTitlePrefix_1N = "Q^{2} for 1N with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1N");
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
        TString HistTitlePrefix_2p = "Q^{2} for 2p with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 2p");
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
        TString HistTitlePrefix_1n1p = "Q^{2} for 1n1p with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 1n1p");
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
        TString HistTitlePrefix_2N = "Q^{2} for 2N with Q^{2}_{TL cut} #geq " + doubleToString(Q2_master) + " GeV^{2}/c^{2} on " + sample_target2 + sample_genie_tune2 + sample_beamE2;
        pageTitles.push_back("Q^{2} plots for 2N");
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
        // #endregion

        cout << "\n";

        TString TempOutPutPath = "/Q2_" + doubleToString(Q2_master);

        cout << "TempOutPutPath = " << TempOutPutPath << endl;
        cout << "lundPath = " << lundPath << endl;

        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath);
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/lundfiles");
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/mchipo");
        gSystem->Exec("mkdir -p " + lundPath + TempOutPutPath + "/reconhipo");

        TString TempLundPath = lundPath + TempOutPutPath + "/lundfiles";
        cout << "TempLundPath =  " << TempLundPath << endl;

        // Read in target parameter files
        cout << "Converting file " << inputFile << endl;
        TFile *inFile = new TFile(inputFile);
        cout << "Making LUND file " << outputFile << endl;

        TTree *T = (TTree *)inFile->Get("gst");

        double RES_ID = 0.; // WAS targP = 0.; // polarization
        double beamP = 0.;  // polarization
        Int_t interactN = 1;
        int beamType = 11;

        double beamE = -99; // GeV

        Bool_t qel;
        Bool_t mec;
        Bool_t res;
        Bool_t dis;
        // will be coded into 1,2,3,4
        Int_t resid;
        // final state particles
        Int_t nf;
        Int_t pdgf[125];   //[nf]
        Double_t Ef[125];  //[nf]
        Double_t pxf[125]; //[nf]
        Double_t pyf[125]; //[nf]
        Double_t pzf[125]; //[nf]
        // electron info
        Double_t El;
        Double_t pxl;
        Double_t pyl;
        Double_t pzl;

        Double_t Q2;
        Int_t nfp;
        Int_t nfn;

        cout << "Making LUND file " << outputFile << endl;

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

        if (PrintOut)
        {
            cout << "\nQ2_master = " << Q2_master << endl;
        }

        TString formatstring, outstring;

        // Check the number of files is not more than what is in the file
        if (nFiles > nEvents / 10000)
        {
            nFiles = nEvents / 10000;
        }

        int iFiles = 1;

        int MaxEventsPerFile = 10000;
        // int j = 0;
        int start = 0;

        // nFiles = 5;

        if (PrintOut)
        {
            cout << "nFiles = " << nFiles << endl;
            cout << "Number of events = " << nEvents << endl;
            cout << "MaxEventsPerFile = " << MaxEventsPerFile << endl;
        }

        cout << "\n";

        int Q2_above_cut_counter = 0;
        int Q2_above_cut_counter_debug = 0;
        int matched = 0;
        vector<int> Q2_above_cut_ind;

        for (int k = 0; k < nEvents; k++)
        {
            T->GetEntry(k);
            if (Q2 >= Q2_master)
            {
                ++Q2_above_cut_counter;
                Q2_above_cut_ind.push_back(k);
            }
        }

        if (PrintOut)
        {
            cout << "Q2_above_cut_counter = " << Q2_above_cut_counter << "\n";
            cout << "Q2_above_cut_ind.size() = " << Q2_above_cut_ind.size() << "\n";

            if (CountQ2AndExit)
            {
                exit(0);
            }
        }

        while (iFiles <= nFiles)
        {
            int j = 0;

            if (PrintOut)
            {
                cout << "-----------------------------------------------------------------\n";
                cout << "iFiles = " << iFiles << "\n";
                cout << "j = " << j << "\n";
            }

            TString outfilename = Form("%s/%s_%d.txt", TempLundPath.Data(), outputFile.Data(), iFiles);

            ofstream outfile;
            outfile.open(outfilename);
            // int start = (iFiles - 1) * 10000;
            // int end = iFiles * 10000;

            int FilledEvents = 0;

            while (FilledEvents < MaxEventsPerFile)
            {
                T->GetEntry(j + start);

                if (Q2 >= Q2_master)
                {
                    for (int q = 0; q < Q2_above_cut_ind.size(); q++)
                    {
                        if ((Q2 >= Q2_master) && ((j + start) == Q2_above_cut_ind.at(q)))
                        {
                            ++matched;
                        }
                    }

                    if (nf == 1)
                    {
                        Q2_1N_TL_all_int->Fill(Q2);

                        if (qel)
                        {
                            Q2_1N_TL_QE_only->Fill(Q2);
                        }
                        else if (mec)
                        {
                            Q2_1N_TL_MEC_only->Fill(Q2);
                        }
                        else if (res)
                        {
                            Q2_1N_TL_RES_only->Fill(Q2);
                        }
                        else if (dis)
                        {
                            Q2_1N_TL_DIS_only->Fill(Q2);
                        }

                        if (nfp == 1)
                        {
                            Q2_1p_TL_all_int->Fill(Q2);

                            if (qel)
                            {
                                Q2_1p_TL_QE_only->Fill(Q2);
                            }
                            else if (mec)
                            {
                                Q2_1p_TL_MEC_only->Fill(Q2);
                            }
                            else if (res)
                            {
                                Q2_1p_TL_RES_only->Fill(Q2);
                            }
                            else if (dis)
                            {
                                Q2_1p_TL_DIS_only->Fill(Q2);
                            }
                        }
                        else if (nfn == 1)
                        {
                            Q2_1n_TL_all_int->Fill(Q2);

                            if (qel)
                            {
                                Q2_1n_TL_QE_only->Fill(Q2);
                            }
                            else if (mec)
                            {
                                Q2_1n_TL_MEC_only->Fill(Q2);
                            }
                            else if (res)
                            {
                                Q2_1n_TL_RES_only->Fill(Q2);
                            }
                            else if (dis)
                            {
                                Q2_1n_TL_DIS_only->Fill(Q2);
                            }
                        }
                    }
                    else if (nf == 2)
                    {
                        Q2_2N_TL_all_int->Fill(Q2);

                        if (qel)
                        {
                            Q2_2N_TL_QE_only->Fill(Q2);
                        }
                        else if (mec)
                        {
                            Q2_2N_TL_MEC_only->Fill(Q2);
                        }
                        else if (res)
                        {
                            Q2_2N_TL_RES_only->Fill(Q2);
                        }
                        else if (dis)
                        {
                            Q2_2N_TL_DIS_only->Fill(Q2);
                        }

                        if (nfp == 2)
                        {
                            Q2_2p_TL_all_int->Fill(Q2);

                            if (qel)
                            {
                                Q2_2p_TL_QE_only->Fill(Q2);
                            }
                            else if (mec)
                            {
                                Q2_2p_TL_MEC_only->Fill(Q2);
                            }
                            else if (res)
                            {
                                Q2_2p_TL_RES_only->Fill(Q2);
                            }
                            else if (dis)
                            {
                                Q2_2p_TL_DIS_only->Fill(Q2);
                            }
                        }
                        else if (nfn == 1 && nfp == 1)
                        {
                            Q2_1n1p_TL_all_int->Fill(Q2);

                            if (qel)
                            {
                                Q2_1n1p_TL_QE_only->Fill(Q2);
                            }
                            else if (mec)
                            {
                                Q2_1n1p_TL_MEC_only->Fill(Q2);
                            }
                            else if (res)
                            {
                                Q2_1n1p_TL_RES_only->Fill(Q2);
                            }
                            else if (dis)
                            {
                                Q2_1n1p_TL_DIS_only->Fill(Q2);
                            }
                        }
                    }
                    else
                    {
                        Q2_1e_cut_TL_all_int->Fill(Q2);

                        if (qel)
                        {
                            Q2_1e_cut_TL_QE_only->Fill(Q2);
                        }
                        else if (mec)
                        {
                            Q2_1e_cut_TL_MEC_only->Fill(Q2);
                        }
                        else if (res)
                        {
                            Q2_1e_cut_TL_RES_only->Fill(Q2);
                        }
                        else if (dis)
                        {
                            Q2_1e_cut_TL_DIS_only->Fill(Q2);
                        }
                    }

                    // Stores reaction mechanism qel = 1, mec = 2, rec = 3, dis=4
                    double code = 0.;

                    if (qel)
                    {
                        code = 1.;
                    }
                    else if (mec)
                    {
                        code = 2.;
                    }
                    else if (res)
                    {
                        code = 3.;
                    }
                    else if (dis)
                    {
                        code = 4.;
                    }

                    if (code < .01)
                    {
                        continue;
                    }

                    RES_ID = double(resid);

                    int nf_mod = 1;
                    for (int iPart = 0; iPart < nf; iPart++)
                    {
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
                    outstring = Form(formatstring, nf_mod, A, Z, RES_ID /*targP*/, beamP, beamType, beamE, interactN, j, code);
                    outfile << outstring;

                    auto vtx = randomVertex(target); // get vertex of event

                    int part_num = 0;
                    // electron
                    outfile << addParticle(1, 1, 11, TVector3(pxl, pyl, pzl), mass_e, vtx);
                    part_num++;

                    for (int iPart = 0; iPart < nf; iPart++)
                    {
                        if (pdgf[iPart] == 2212)
                        { // p
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_p, vtx);
                        }
                        else if (pdgf[iPart] == 2112)
                        { // n
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_n, vtx);
                        }
                        else if (pdgf[iPart] == 211)
                        { // pi+
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                        }
                        else if (pdgf[iPart] == -211)
                        { // pi-
                            part_num++;
                            outfile << addParticle(part_num, 1, pdgf[iPart], TVector3(pxf[iPart], pyf[iPart], pzf[iPart]), mass_pi, vtx);
                        }
                    }

                    ++FilledEvents;
                    ++Q2_above_cut_counter_debug;

                    if (PrintOut)
                    {
                        cout << "\niFiles = " << iFiles << "\n";
                        cout << "Q2 = " << Q2 << "\n";
                        cout << "FilledEvents = " << FilledEvents << "\n";
                        cout << "j = " << j << "\n";
                        cout << "start = " << start << "\n";
                        cout << "\n";
                    }
                }

                ++j;

                if ((j + start) > nEvents)
                {
                    break;
                }
            }

            outfile.close();

            start = j;

            if (PrintOut)
            {
                cout << "start = " << start << "\n";
            }

            ++iFiles;

            // else
            // {
            //     cout << "Not enough events to fill " << nFiles << " files with Q2 >= " << doubleToString(Q2_master) << "\n";
            //     cout << "Saved " << iFiles << " instead.\n";
            //     break;
            // }
        }

        std::string TempFilePath0 = TempLundPath.Data();
        std::string TempFilePath = TempFilePath0 + "/";
        std::string pdfFileName = TempFilePath + "Q2_" + doubleToString(Q2_master) + ".pdf";
        const char *pdfFile = pdfFileName.c_str();

        // Create a canvas
        TCanvas *canvas = new TCanvas("canvas", "Canvas for saving histograms", 800, 600);
        canvas->cd()->SetGrid();
        canvas->cd()->SetBottomMargin(0.14), canvas->cd()->SetLeftMargin(0.18), canvas->cd()->SetRightMargin(0.12);

        // Start the multi-page PDF
        canvas->Print(Form("%s[", pdfFile)); // Open the PDF file

        // Loop through the list of histograms
        for (int i = 0; i < histList.size(); i++)
        {
            canvas->cd(); // Select the canvas
            canvas->Clear();

            std::string pageTitleTemp = pageTitles.at(i).Data();

            if (pageTitleTemp != "")
            {
                cout << "pageTitleTemp = " << pageTitleTemp << "\n";

                TLatex text;
                text.SetTextSize(0.05);
                text.DrawLatex(0.2, 0.9, pageTitles.at(i));
                canvas->Print(pdfFile); // Save the current canvas (histogram) to the PDF
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
            histList.at(i)->Draw(); // Draw the histogram on the canvas
            canvas->Print(pdfFile); // Save the current canvas (histogram) to the PDF
        }

        // End the multi-page PDF
        canvas->Print(Form("%s]", pdfFile)); // Close the PDF file

        delete canvas;

        cout << "\nQ2_above_cut_counter = " << Q2_above_cut_counter << "\n";
        cout << "Q2_above_cut_counter_debug = " << Q2_above_cut_counter_debug << "\n\n";
        cout << "matched = " << matched << "\n\n";

        Q2_master = Q2_master + dQ2;
    }
}
