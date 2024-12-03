#!/bin/csh

unset BEAM_E
# setenv BEAM_E 5986MeV
setenv BEAM_E 4029MeV
# setenv BEAM_E 2070MeV
unset JOB_OUT_PATH
setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/${BEAM_E}
unset JOB_OUT_PATH_1E
setenv JOB_OUT_PATH_1E ${JOB_OUT_PATH}/OutPut_Tester_e_1e
unset SUBMIT_SCRIPT_PATH
# setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/
setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_4GeV/
# setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_2GeV/

# echo
# echo "Pulling updates..."
# git pull
# echo

# echo
# echo "Clearing farm_out directory..."
# rm /u/scifarm/farm_out/asportes/*
# echo

echo
echo "Removing old directory structure for MC simulation here..."
rm -rf ${JOB_OUT_PATH_1E}/mchipo
rm -rf ${JOB_OUT_PATH_1E}/reconhipo
rm -rf ${JOB_OUT_PATH_1E}/rootfiles
echo

echo
echo "Setting up directory structure for MC simulation here..."
mkdir ${JOB_OUT_PATH_1E}/mchipo ${JOB_OUT_PATH_1E}/reconhipo ${JOB_OUT_PATH_1E}/rootfiles
echo

echo
echo "Submitting 1e sbatch job..."
sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform_1e.sh
echo