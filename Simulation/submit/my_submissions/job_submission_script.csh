#!/bin/csh

# Job parameters
# ============================================================================
echo
echo "- Job parameters ------------------------------------------------------"
echo

# unset BEAM_E
setenv BEAM_E 5986MeV
# setenv BEAM_E 4029MeV
# setenv BEAM_E 2070MeV
echo "BEAM_E = ${BEAM_E}"
echo

unset CLEAR_FARM_OUT
setenv CLEAR_FARM_OUT 1 ## 1 for true
echo "CLEAR_FARM_OUT = ${CLEAR_FARM_OUT}"
unset CANCEL_PREVIOUS_JOBS
setenv CANCEL_PREVIOUS_JOBS 1 ## 1 for true
echo "CANCEL_PREVIOUS_JOBS = ${CANCEL_PREVIOUS_JOBS}"
echo

setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/${BEAM_E}-BeamE-test
# setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/${BEAM_E}
echo "JOB_OUT_PATH = ${JOB_OUT_PATH}"
setenv JOB_OUT_PATH_1E ${JOB_OUT_PATH}/OutPut_1e
setenv JOB_OUT_PATH_EP ${JOB_OUT_PATH}/OutPut_ep
setenv JOB_OUT_PATH_EN ${JOB_OUT_PATH}/OutPut_en
echo "JOB_OUT_PATH_1E = ${JOB_OUT_PATH_1E}"
echo "JOB_OUT_PATH_EP = ${JOB_OUT_PATH_EP}"
echo "JOB_OUT_PATH_EN = ${JOB_OUT_PATH_EN}"
echo

# # setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/
# # setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_4GeV/
# setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_2GeV/
# echo "SUBMIT_SCRIPT_PATH = ${SUBMIT_SCRIPT_PATH}"

# Setting SUBMIT_SCRIPT_PATH for 2 GeV
# ============================================================================
if ("${BEAM_E}" == "2070MeV") then
    echo "- Setting SUBMIT_SCRIPT_PATH for 2 GeV --------------------------------"
    setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_2GeV/
    echo
else if ("${BEAM_E}" == "4029MeV") then
    echo "- Setting SUBMIT_SCRIPT_PATH for 4 GeV --------------------------------"
    setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_4GeV/
    echo
else if ("${BEAM_E}" == "5986MeV") then
    echo "- Setting SUBMIT_SCRIPT_PATH for 6 GeV --------------------------------"
    setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/
    echo
endif

echo "SUBMIT_SCRIPT_PATH = ${SUBMIT_SCRIPT_PATH}"
echo

# Re-pulling repository
# ============================================================================
echo "- Re-pulling repository -----------------------------------------------"
echo
echo "Pulling updates..."
git pull
echo

# Clearing farm_out directory
# ============================================================================

# Optionally clear the farm_out directory
if ("${CLEAR_FARM_OUT}" == "1") then
    echo
    echo "- Clearing farm_out directory -----------------------------------------"
    cd /u/scifarm/farm_out/asportes/
    rm *
    cd -
    echo
endif

# Canceling previous jobs
# ============================================================================

# Optionally cancel previous jobs
if ("${CANCEL_PREVIOUS_JOBS}" == "1") then
    echo
    echo "- Canceling previous jobs ---------------------------------------------"
    scancel --account=asportes
    echo
endif

# Removing old directory structure for MC simulation here
# ============================================================================

echo
echo "- Removing old directory structure for MC simulation here -------------"
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
echo "- Setting up directory structure for MC simulation here ---------------"
mkdir ${JOB_OUT_PATH_1E}/mchipo ${JOB_OUT_PATH_1E}/reconhipo ${JOB_OUT_PATH_1E}/rootfiles
mkdir ${JOB_OUT_PATH_EP}/mchipo ${JOB_OUT_PATH_EP}/reconhipo ${JOB_OUT_PATH_EP}/rootfiles
mkdir ${JOB_OUT_PATH_EN}/mchipo ${JOB_OUT_PATH_EN}/reconhipo ${JOB_OUT_PATH_EN}/rootfiles
echo

# Submitting jobs
# ============================================================================

# echo
# echo "- Submitting jobs -----------------------------------------------------"
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
