#!/bin/csh

# setenv BEAM_E 5986MeV
# setenv BEAM_E 4029MeV
setenv BEAM_E "2070MeV"
echo ${BEAM_E}
setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/${BEAM_E}
setenv JOB_OUT_PATH_1E ${JOB_OUT_PATH}/OutPut_1e_torus-1_test
setenv JOB_OUT_PATH_EP ${JOB_OUT_PATH}/OutPut_ep_torus-1_test
setenv JOB_OUT_PATH_EN ${JOB_OUT_PATH}/OutPut_en_torus-1_test
# setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/
# setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_4GeV/
setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_2GeV/

echo
echo "Pulling updates..."
git pull
echo

echo
echo "Clearing farm_out directory..."
rm /u/scifarm/farm_out/asportes/*
echo

echo
echo "Removing old directory structure for MC simulation here..."
rm -rf ${JOB_OUT_PATH_1E}/mchipo
rm -rf ${JOB_OUT_PATH_1E}/reconhipo
rm -rf ${JOB_OUT_PATH_1E}/rootfiles

rm -rf ${JOB_OUT_PATH_EP}/mchipo
rm -rf ${JOB_OUT_PATH_EP}/reconhipo
rm -rf ${JOB_OUT_PATH_EP}/rootfiles

rm -rf ${JOB_OUT_PATH_EN}/mchipo
rm -rf ${JOB_OUT_PATH_EN}/reconhipo
rm -rf ${JOB_OUT_PATH_EN}/rootfiles
echo

echo
echo "Setting up directory structure for MC simulation here..."
mkdir ${JOB_OUT_PATH_1E}/mchipo ${JOB_OUT_PATH_1E}/reconhipo ${JOB_OUT_PATH_1E}/rootfiles
mkdir ${JOB_OUT_PATH_EP}/mchipo ${JOB_OUT_PATH_EP}/reconhipo ${JOB_OUT_PATH_EP}/rootfiles
mkdir ${JOB_OUT_PATH_EN}/mchipo ${JOB_OUT_PATH_EN}/reconhipo ${JOB_OUT_PATH_EN}/rootfiles
echo

# echo
# echo "Submitting 1e sbatch job..."
# sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform_1e.sh
# echo

# echo "Submitting ep sbatch job..."
# sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform_ep.sh
# echo

# echo "Submitting en sbatch job..."
# sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform_en.sh
# echo
