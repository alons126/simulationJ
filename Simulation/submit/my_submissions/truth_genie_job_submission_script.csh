#!/bin/csh

# Loop over target nuclei
foreach FC_STATUSES ( 0 1  )
# foreach SAMPLE_TARGET_NUCLEI ( H1 D2 C12 Ar40 )

# Loop over target nuclei
foreach SAMPLE_TARGET_NUCLEI ( C12  )
# foreach SAMPLE_TARGET_NUCLEI ( H1 D2 C12 Ar40 )

# Loop over GENIE tunes
foreach GENIE_TUNES ( G18_10a_00_000 GEM21_11a_00_000 )

# Loop over beam energies
foreach BEAM_ENERGIES ( 2070MeV )
# foreach TEMP_BEAM_E ( 4029MeV )
# foreach TEMP_BEAM_E ( 2070MeV 4029MeV 5986MeV )

# Job parameters
# ============================================================================

# Set target nucleus
unset SAMPLE_TARGET_NUCLEUS
setenv SAMPLE_TARGET_NUCLEUS ${SAMPLE_TARGET_NUCLEI}
echo "SAMPLE_TARGET_NUCLEUS = ${SAMPLE_TARGET_NUCLEUS}"
echo ""

# Set GENIE tune
unset GENIE_TUNE
setenv GENIE_TUNE ${GENIE_TUNES}
echo "GENIE_TUNE:\t\t${GENIE_TUNE}"
echo ""

# Set beam energy
unset BEAM_E
setenv BEAM_E ${BEAM_ENERGIES}
echo "BEAM_E:\t\t\t${BEAM_E}"
echo ""

unset PRINT_OUT_COLOR

if ("${BEAM_E}" == "2070MeV") then
    setenv PRINT_OUT_COLOR '\033[31m'
else if ("${BEAM_E}" == "4029MeV") then
    setenv PRINT_OUT_COLOR '\033[32m'
else if ("${BEAM_E}" == "5986MeV") then
    setenv PRINT_OUT_COLOR '\033[33m'
else
    echo "Unknown beam energy: ${BEAM_E}"
    exit 1
endif

# Set Q² cut based on energy
unset Q2_CUT
if ("${BEAM_E}" == "2070MeV") then
    setenv Q2_CUT Q2_0_02
else if ("${BEAM_E}" == "4029MeV") then
    setenv Q2_CUT Q2_0_25
else if ("${BEAM_E}" == "5986MeV") then
    setenv Q2_CUT Q2_0_40
else
    echo "Unknown beam energy: ${BEAM_E}"
    exit 1
endif

echo "Q2_CUT = ${Q2_CUT}"
echo ""

# Set fiducial cuts status
setenv FC_STATUS_ENABLED ${FC_STATUSES}
echo "FC_STATUS_ENABLED = ${FC_STATUS_ENABLED}"
echo ""

unset FC_STATUS
if ("${FC_STATUS_ENABLED}" == "1") then
    setenv FC_STATUS _wFC
else
setenv FC_STATUS ""
endif

echo "FC_STATUS = ${FC_STATUS}"
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

unset JOB_OUT_PATH
setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/${SAMPLE_TARGET_NUCLEUS}/${GENIE_TUNE}/${BEAM_E}_${Q2_CUT}${FC_STATUS}
echo "${PRINT_OUT_COLOR}JOB_OUT_PATH:\033[0m ${JOB_OUT_PATH}"

# Check if JOB_OUT_PATH is a directory
if (! -d "${JOB_OUT_PATH}") then
    echo "Error: Directory specified by JOB_OUT_PATH does not exist: ${JOB_OUT_PATH}"
    exit 1
endif

# Setting SUBMIT_SCRIPT_PATH for 2 GeV
# ============================================================================

unset RUNNING_DIR
setenv RUNNING_DIR `pwd`
echo "${PRINT_OUT_COLOR}RUNNING_DIR::\033[0m ${RUNNING_DIR}"
echo

# Check if RUNNING_DIR is a directory
if (! -d "${RUNNING_DIR}") then
    echo "Error: Directory specified by RUNNING_DIR does not exist: ${RUNNING_DIR}"
    exit 1
endif

if ("${BEAM_E}" == "2070MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 2 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_2GeV/
else if ("${BEAM_E}" == "4029MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 4 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_4GeV/
else if ("${BEAM_E}" == "5986MeV") then
    echo "${PRINT_OUT_COLOR}- Setting SUBMIT_SCRIPT_PATH for 6 GeV --------------------------------\033[0m"
    setenv SUBMIT_SCRIPT_PATH ${RUNNING_DIR}/Uniform_sample_6GeV/
endif

echo "${PRINT_OUT_COLOR}SUBMIT_SCRIPT_PATH:\033[0m ${SUBMIT_SCRIPT_PATH}"
echo

# Check if SUBMIT_SCRIPT_PATH is a directory
if (! -d "${SUBMIT_SCRIPT_PATH}") then
    echo "Error: Directory specified by SUBMIT_SCRIPT_PATH does not exist: ${SUBMIT_SCRIPT_PATH}"
    exit 1
endif

# Re-pulling repository
# ============================================================================
echo "${PRINT_OUT_COLOR}- Re-pulling repository -----------------------------------------------\033[0m"
echo
echo "${PRINT_OUT_COLOR}Pulling updates...\033[0m"

# This command is used to reset the current branch to the latest commit in the remote repository. The
# --hard option is used to discard any local changes, and the git pull command is used to fetch and merge
# the latest changes from the remote repository.

# Display the latest commit in the current branch. The -1 option limits the output to one commit, and the
# --oneline option formats the output to show only the commit hash and the commit message in a single line.
# This command is useful for quickly checking the latest commit in the current branch without displaying
# the full commit history.

echo "HEAD:"
git log -1 --oneline
echo ""

# Clean the working tree by recursively removing files that are not under version control, starting from
# the current directory. The -f option is used to force the removal of files, and the -d option is used
# to remove untracked directories. The -x option is used to remove files that are ignored by git.

# This command is useful for cleaning up the working tree and removing any untracked files or directories
# that may have been created during development, like generated cut files and acceptance and weight files.

git clean -fxd # removes untracked files and directories
echo ""

echo "\033[35mPulling updates...\033[0m"
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
rm -rf ${JOB_OUT_PATH}/mchipo ; rm -rf ${JOB_OUT_PATH}/reconhipo
echo

echo
echo "${PRINT_OUT_COLOR}- Setting up directory structure for MC simulation here ---------------\033[0m"
mkdir ${JOB_OUT_PATH}/mchipo ${JOB_OUT_PATH}/reconhipo
echo

# Submitting jobs
# ============================================================================

echo
echo "${PRINT_OUT_COLOR}- Submitting jobs -----------------------------------------------------\033[0m"
echo

echo "${PRINT_OUT_COLOR}Submitting GENIE sbatch job...\033[0m"
sbatch ${SUBMIT_SCRIPT_PATH}/submit_GENIE_sample.sh
echo

end # End of loop over beam energies
end # End of loop over GENIE tunes
end # End of loop over target nuclei
end # End of loop over fiducial cuts status