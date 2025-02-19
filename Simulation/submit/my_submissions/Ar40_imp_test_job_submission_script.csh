#!/bin/csh

module unload gemc
module load gemc/5.10

unsetenv GEMC_DATA_DIR
setenv GEMC_DATA_DIR /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/clas12Tags/GEMC_5_10
echo $GEMC_DATA_DIR
echo

cd /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/detectors
git pull
git clean -f
./targets.pl config.dat
echo

cd /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/clas12Tags/GEMC_5_10/experiments/clas12/targets
cp /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/detectors/clas12/targets/target__geometry_RGM_lAr.txt ./
cp /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/detectors/clas12/targets/target__geometry_RGM_2_C_v2_S.txt ./
cp /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/detectors/clas12/targets/target__geometry_RGM_2_C_v2_L.txt ./

cd /u/home/asportes/clas12simulations/simulationJ/Simulation/submit/my_submissions/Ar40_imp_tests

rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/Ar40/G18_10a_00_000/4029MeV/Q2_0_19/mchipo/*
rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/Ar40/G18_10a_00_000/4029MeV/Q2_0_19/reconhipo/*
# sbatch submit_GEMC_RGM_lAr.sh
echo
rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_S/mchipo/*
rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_S/reconhipo/*
# sbatch submit_GEMC_RGM_2_C_v2_S.sh
echo
rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_L/mchipo/*
rm -rf /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_L/reconhipo/*
# sbatch submit_GEMC_RGM_2_C_v2_L.sh
echo
