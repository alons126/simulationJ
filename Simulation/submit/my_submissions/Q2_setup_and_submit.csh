#!/bin/csh

# Function for setting up and submitting jobs
# Accepts: TARGET, GENIE_TUNE, Q2_CUT, BEAM_E, clear_farm_out (optional)

set TARGET = $1
set GENIE_TUNE = $2
set Q2_CUT = $3
set BEAM_E = $4
set clear_farm_out = $5

# Set a base path for JOB_OUT_PATH before using it
setenv BASE_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples

# Construct the full JOB_OUT_PATH
setenv JOB_OUT_PATH ${BASE_PATH}/${GENIE_TUNE}/Q2_th_test_samples/${BEAM_E}/${Q2_CUT}

# Determine the correct submit script path based on BEAM_E
if ("${BEAM_E}" == "5986MeV") then
    setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_6GeV/
else if ("${BEAM_E}" == "4029MeV") then
    setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_4GeV/
else if ("${BEAM_E}" == "2070MeV") then
    setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_2GeV/
endif

echo
echo "Pulling updates..."
git pull
echo

# Optionally clear the farm_out directory
if ("${clear_farm_out}" == "true") then
    echo
    echo "Clearing farm_out directory..."
    rm /u/scifarm/farm_out/asportes/*
    echo
endif

echo
echo "Removing old directory structure for MC simulation..."
rm -rf ${JOB_OUT_PATH}/mchipo ${JOB_OUT_PATH}/reconhipo ${JOB_OUT_PATH}/rootfiles
echo

echo
echo "Setting up directory structure for MC simulation..."
mkdir -p ${JOB_OUT_PATH}/mchipo ${JOB_OUT_PATH}/reconhipo ${JOB_OUT_PATH}/rootfiles
echo

echo
echo "Submitting sbatch jobs for ${TARGET} at BeamE = ${BEAM_E} with a ${Q2_CUT} cut..."
sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_Q2.sh
echo