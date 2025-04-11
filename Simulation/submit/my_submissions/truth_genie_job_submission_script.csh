#!/bin/csh

setenv GENIE_TUNE
# setenv GENIE_TUNE G18_10a_00_000
setenv GENIE_TUNE GEM21_11a_00_000
echo "GENIE_TUNE:\t\t${GENIE_TUNE}"

# Set target nucleus
unset TL_SAMPLE_TARGET_NUCLEUS
# setenv TL_SAMPLE_TARGET_NUCLEUS H1
# setenv TL_SAMPLE_TARGET_NUCLEUS D2
setenv TL_SAMPLE_TARGET_NUCLEUS C12
# setenv TL_SAMPLE_TARGET_NUCLEUS Ar40
echo "TL_SAMPLE_TARGET_NUCLEUS = ${TL_SAMPLE_TARGET_NUCLEUS}"
echo ""

foreach TEMP_BEAM_E ( 2070MeV )
# foreach TEMP_BEAM_E ( 4029MeV )
# foreach TEMP_BEAM_E ( 2070MeV 4029MeV 5986MeV )

# Job parameters
# ============================================================================

unset BEAM_E
setenv BEAM_E ${TEMP_BEAM_E}
echo "BEAM_E:\t\t\t${BEAM_E}"

unset PRINT_OUT_COLOR

if ("${BEAM_E}" == "2070MeV") then
    setenv PRINT_OUT_COLOR '\033[31m'
else if ("${BEAM_E}" == "4029MeV") then
    setenv PRINT_OUT_COLOR '\033[32m'
else if ("${BEAM_E}" == "5986MeV") then
    setenv PRINT_OUT_COLOR '\033[33m'
endif

# Set Q² cut based on energy
unsetenv TL_SAMPLE_Q2_CUT
if ("${TL_SAMPLE_ENERGY}" == "2070MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_02
else if ("${TL_SAMPLE_ENERGY}" == "4029MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_25
else if ("${TL_SAMPLE_ENERGY}" == "5986MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_40
else
    echo "Unknown beam energy: ${TL_SAMPLE_ENERGY}"
    exit 1
endif

echo "TL_SAMPLE_Q2_CUT = ${TL_SAMPLE_Q2_CUT}"
echo ""

echo
echo "${PRINT_OUT_COLOR}- Job parameters ------------------------------------------------------\033[0m"
echo

echo "${PRINT_OUT_COLOR}BEAM_E:\033[0m ${BEAM_E}"
echo

unset CLEAR_FARM_OUT
setenv CLEAR_FARM_OUT 0 ## 1 for true
echo "${PRINT_OUT_COLOR}CLEAR_FARM_OUT:\033[0m ${CLEAR_FARM_OUT}"

unset CANCEL_PREVIOUS_JOBS
setenv CANCEL_PREVIOUS_JOBS 0 ## 1 for true
echo "${PRINT_OUT_COLOR}CANCEL_PREVIOUS_JOBS:\033[0m ${CANCEL_PREVIOUS_JOBS}"

unset USE_GEMC_5_10
setenv USE_GEMC_5_10 1 ## 1 for true
echo "${PRINT_OUT_COLOR}USE_GEMC_5_10:\033[0m ${USE_GEMC_5_10}"
echo

# Set Q² cut based on energy
unsetenv TL_SAMPLE_Q2_CUT
if ("${TL_SAMPLE_ENERGY}" == "2070MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_02
else if ("${TL_SAMPLE_ENERGY}" == "4029MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_25
else if ("${TL_SAMPLE_ENERGY}" == "5986MeV") then
    setenv TL_SAMPLE_Q2_CUT Q2_0_40
else
    echo "Unknown beam energy: ${TL_SAMPLE_ENERGY}"
    exit 1
endif

echo "TL_SAMPLE_Q2_CUT = ${TL_SAMPLE_Q2_CUT}"
echo ""

unset JOB_OUT_PATH
setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/${BEAM_E}_${TL_SAMPLE_Q2_CUT}
# setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/${BEAM_E}_${TL_SAMPLE_Q2_CUT}_wFC
# setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/C12/GEM21_11a_00_000/${BEAM_E}_${TL_SAMPLE_Q2_CUT}
# setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/C12/GEM21_11a_00_000/${BEAM_E}_${TL_SAMPLE_Q2_CUT}_wFC
echo "${PRINT_OUT_COLOR}JOB_OUT_PATH:\033[0m ${JOB_OUT_PATH}"

# Setting SUBMIT_SCRIPT_PATH for 2 GeV
# ============================================================================

unset RUNNING_DIR
setenv RUNNING_DIR `pwd`
echo "${PRINT_OUT_COLOR}RUNNING_DIR::\033[0m ${RUNNING_DIR}"
echo

if ("${BEAM_E}" == "2070MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 2 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_2GeV/
    # setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_2GeV/
    echo
else if ("${BEAM_E}" == "4029MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 4 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_4GeV/
    # setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_4GeV/
    echo
else if ("${BEAM_E}" == "5986MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 6 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_6GeV/
    # setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/
    echo
endif

echo "${PRINT_OUT_COLOR}SUBMIT_SCRIPT_PATH:\033[0m ${SUBMIT_SCRIPT_PATH}"
echo

# Re-pulling repository
# ============================================================================
echo "${PRINT_OUT_COLOR}- Re-pulling repository -----------------------------------------------\033[0m"
echo
echo "${PRINT_OUT_COLOR}Pulling updates...\033[0m"
git reset --hard
git pull
echo

# Clearing farm_out directory
# ============================================================================

# Optionally clear the farm_out directory
if ("${BEAM_E}" == "2070MeV") then
    if ("${CLEAR_FARM_OUT}" == "1") then
        echo
        echo "${PRINT_OUT_COLOR}- Clearing farm_out directory -----------------------------------------\033[0m"
        cd /u/scifarm/farm_out/asportes/
        rm *
        cd -
        echo
    endif
endif

# Canceling previous jobs
# ============================================================================

# Optionally cancel previous jobs
if ("${BEAM_E}" == "2070MeV") then
    if ("${CANCEL_PREVIOUS_JOBS}" == "1") then
        echo
        echo "${PRINT_OUT_COLOR}- Canceling previous jobs ---------------------------------------------\033[0m"
        scancel --user=asportes
        echo
    endif
endif

# Use GEMC 5.10
# ============================================================================

# Optionally use GEMC 5.10
if ("${USE_GEMC_5_10}" == "1") then
    echo
    echo "${PRINT_OUT_COLOR}- Reverting to GEMC 5.10 ----------------------------------------------\033[0m"
    module unload gemc
    module load gemc/5.10
    echo
endif

echo "${PRINT_OUT_COLOR}GEMC_DATA_DIR:\033[0m ${GEMC_DATA_DIR}"
echo

# Removing old directory structure for MC simulation here
# ============================================================================

echo
echo "${PRINT_OUT_COLOR}- Removing old directory structure for MC simulation here -------------\033[0m"
rm -rf ${JOB_OUT_PATH}/mchipo
rm -rf ${JOB_OUT_PATH}/reconhipo
rm -rf ${JOB_OUT_PATH}/rootfiles
echo

echo
echo "${PRINT_OUT_COLOR}- Setting up directory structure for MC simulation here ---------------\033[0m"
mkdir ${JOB_OUT_PATH}/mchipo ${JOB_OUT_PATH}/reconhipo ${JOB_OUT_PATH}/rootfiles
echo

# Submitting jobs
# ============================================================================

echo
echo "${PRINT_OUT_COLOR}- Submitting jobs -----------------------------------------------------\033[0m"
echo

echo "${PRINT_OUT_COLOR}Submitting GENIE sbatch job...\033[0m"
sbatch ${SUBMIT_SCRIPT_PATH}/submit_GENIE_sample.sh
echo

end