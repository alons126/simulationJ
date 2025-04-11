#!/bin/csh

# Loop over fiducial cuts statuses
foreach FC_STATUSES ( 0 1 )

# Loop over target nuclei
foreach SAMPLE_TARGET_NUCLEI ( C12 )
# foreach SAMPLE_TARGET_NUCLEI ( H1 D2 C12 Ar40 )

# Loop over GENIE tunes
foreach GENIE_TUNES ( G18_10a_00_000 GEM21_11a_00_000 )

# Loop over beam energies
foreach BEAM_ENERGIES ( 2070MeV )
foreach BEAM_ENERGIES ( 2070MeV 4029MeV )
# foreach BEAM_ENERGIES ( 2070MeV 4029MeV 5986MeV )

# Job parameters
# ============================================================================

# Set target nucleus
unsetenv SAMPLE_TARGET_NUCLEUS
setenv SAMPLE_TARGET_NUCLEUS ${SAMPLE_TARGET_NUCLEI}
echo "SAMPLE_TARGET_NUCLEUS: ${SAMPLE_TARGET_NUCLEUS}"
echo ""

# Set GENIE tune
unsetenv GENIE_TUNE
setenv GENIE_TUNE ${GENIE_TUNES}
echo "GENIE_TUNE: ${GENIE_TUNE}"
echo ""

# Set beam energy
unsetenv BEAM_E
setenv BEAM_E ${BEAM_ENERGIES}
echo "BEAM_E: ${BEAM_E}"
echo ""


# Set fiducial cuts status
setenv FC_STATUS_ENABLED ${FC_STATUSES}
echo "FC_STATUS_ENABLED: ${FC_STATUS_ENABLED}"
echo ""

unsetenv FC_STATUS
if ("${FC_STATUS_ENABLED}" == "1") then
    setenv FC_STATUS _wFC
else
    setenv FC_STATUS ""
endif
echo "FC_STATUS: ${FC_STATUS}"
echo ""

unsetenv PRINT_OUT_COLOR
if ("${BEAM_E}" == "2070MeV" && "${FC_STATUS_ENABLED}" == "0") then
    setenv PRINT_OUT_COLOR '\033[31m'
else if ("${BEAM_E}" == "2070MeV" && "${FC_STATUS_ENABLED}" == "1") then
    setenv PRINT_OUT_COLOR '\033[92m'
else if ("${BEAM_E}" == "4029MeV" && "${FC_STATUS_ENABLED}" == "0") then
    setenv PRINT_OUT_COLOR '\033[33m'
else if ("${BEAM_E}" == "4029MeV" && "${FC_STATUS_ENABLED}" == "1") then
    setenv PRINT_OUT_COLOR '\033[94m'
else if ("${BEAM_E}" == "5986MeV" && "${FC_STATUS_ENABLED}" == "0") then
    setenv PRINT_OUT_COLOR '\033[35m'
else if ("${BEAM_E}" == "5986MeV" && "${FC_STATUS_ENABLED}" == "1") then
    setenv PRINT_OUT_COLOR '\033[96m'
else
    echo "Unknown beam energy: ${BEAM_E}"
    exit 1
endif

# Set Q² cut based on energy
unsetenv Q2_CUT
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
echo "Q2_CUT: ${Q2_CUT}"
echo ""

echo
echo "${PRINT_OUT_COLOR}- Job parameters ------------------------------------------------------\033[0m"
echo "${PRINT_OUT_COLOR}BEAM_E:\033[0m ${BEAM_E}"
echo ""

unsetenv TORUS_FIELD

if ("${BEAM_E}" == "2070MeV") then
    setenv TORUS_FIELD 0.5
else if ("${BEAM_E}" == "4029MeV" || "${BEAM_E}" == "5986MeV") then
    setenv TORUS_FIELD -1.0
else
    echo "Unknown torus field configuration: ${BEAM_E}"
    exit 1
endif

echo "${PRINT_OUT_COLOR}TORUS_FIELD:\033[0m ${TORUS_FIELD}"

unsetenv CLEAR_FARM_OUT
setenv CLEAR_FARM_OUT 0 ## 1 for true
echo "${PRINT_OUT_COLOR}CLEAR_FARM_OUT:\033[0m ${CLEAR_FARM_OUT}"

unsetenv CANCEL_PREVIOUS_JOBS
setenv CANCEL_PREVIOUS_JOBS 0 ## 1 for true
echo "${PRINT_OUT_COLOR}CANCEL_PREVIOUS_JOBS:\033[0m ${CANCEL_PREVIOUS_JOBS}"

unsetenv USE_GEMC_5_10
setenv USE_GEMC_5_10 1 ## 1 for true
echo "${PRINT_OUT_COLOR}USE_GEMC_5_10:\033[0m ${USE_GEMC_5_10}"
echo ""

unsetenv OUTPATH
setenv OUTPATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/${SAMPLE_TARGET_NUCLEUS}/${GENIE_TUNE}/${BEAM_E}_${Q2_CUT}${FC_STATUS}
echo "${PRINT_OUT_COLOR}OUTPATH:\033[0m ${OUTPATH}"

# Check if OUTPATH is a directory
if (! -d "${OUTPATH}") then
    echo "Error: Directory specified by OUTPATH does not exist: ${OUTPATH}"
    exit 1
endif

# Setting SUBMIT_SCRIPT_PATH based on beam energy
unsetenv RUNNING_DIR
setenv RUNNING_DIR `pwd`
echo "${PRINT_OUT_COLOR}RUNNING_DIR::\033[0m ${RUNNING_DIR}"
echo ""

if (! -d "${RUNNING_DIR}") then
    echo "Error: Directory specified by RUNNING_DIR does not exist: ${RUNNING_DIR}"
    exit 1
endif

unsetenv SUBMIT_SCRIPT_PATH
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
echo ""

if (! -d "${SUBMIT_SCRIPT_PATH}") then
    echo "Error: Directory specified by SUBMIT_SCRIPT_PATH does not exist: ${SUBMIT_SCRIPT_PATH}"
    exit 1
endif

unsetenv GCARD_FILE
setenv GCARD_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021_C.gcard
echo "${PRINT_OUT_COLOR}GCARD_FILE:\033[0m ${GCARD_FILE}"
echo

if (! -f "${GCARD_FILE}") then
    echo "Error: File specified by GCARD_FILE does not exist: ${GCARD_FILE}"
    exit 1
endif

unsetenv YAML_FILE
if ("${BEAM_E}" == "2070MeV") then
    setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-cv.yaml
else if ("${BEAM_E}" == "4029MeV") then
    setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-ai_4Gev.yaml
else if ("${BEAM_E}" == "5986MeV") then
    setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-ai_6Gev.yaml
endif
echo "${PRINT_OUT_COLOR}YAML_FILE:\033[0m ${YAML_FILE}"
echo

if (! -f "${YAML_FILE}") then
    echo "Error: File specified by YAML_FILE does not exist: ${YAML_FILE}"
    exit 1
endif

# Re-pulling repository
echo "${PRINT_OUT_COLOR}- Re-pulling repository -----------------------------------------------\033[0m"
echo "${PRINT_OUT_COLOR}Pulling updates...\033[0m"
git reset --hard
git clean -fxd
echo "${PRINT_OUT_COLOR}Pulling updates...\033[0m"
git pull
echo "HEAD:"
git log -1 --oneline
echo ""

# Optionally clear the farm_out directory
if ("${CLEAR_FARM_OUT}" == "1") then
    echo "${PRINT_OUT_COLOR}- Clearing farm_out directory -----------------------------------------\033[0m"
    cd /u/scifarm/farm_out/asportes/
    rm -f *
    cd -
    echo
endif

# Optionally cancel previous jobs
if ("${CANCEL_PREVIOUS_JOBS}" == "1") then
    echo "${PRINT_OUT_COLOR}- Canceling previous jobs ---------------------------------------------\033[0m"
    scancel --user=asportes
    echo
endif

# Use GEMC 5.10
if ("${USE_GEMC_5_10}" == "1") then
    echo "${PRINT_OUT_COLOR}- Reverting to GEMC 5.10 ----------------------------------------------\033[0m"
    module unload gemc
    module load gemc/5.10
    echo
endif

echo "${PRINT_OUT_COLOR}GEMC_DATA_DIR:\033[0m ${GEMC_DATA_DIR}"
echo

# Remove old output dirs
echo "${PRINT_OUT_COLOR}- Removing old directory structure for MC simulation -------------------\033[0m"
rm -rf ${OUTPATH}/mchipo ${OUTPATH}/reconhipo
echo

# Create new output dirs
echo "${PRINT_OUT_COLOR}- Setting up directory structure for MC simulation ---------------------\033[0m"
mkdir -p ${OUTPATH}/mchipo ${OUTPATH}/reconhipo
echo

# Submitting job
echo "${PRINT_OUT_COLOR}- Submitting jobs -----------------------------------------------------\033[0m"
echo "${PRINT_OUT_COLOR}Submitting GENIE sbatch job...\033[0m"

unsetenv SLURM_JOB_NAME
setenv SLURM_JOB_NAME ${SAMPLE_TARGET_NUCLEUS}_${GENIE_TUNE}_${BEAM_E}_${Q2_CUT}${FC_STATUS}
echo "${PRINT_OUT_COLOR}SLURM_JOB_NAME:\033[0m ${SLURM_JOB_NAME}"
# sbatch --job-name="${SLURM_JOB_NAME}" ${SUBMIT_SCRIPT_PATH}/submit_GENIE_sample.sh || exit 1
echo

end  # End of loop over beam energies
end  # End of loop over GENIE tunes
end  # End of loop over target nuclei
end  # End of loop over fiducial cuts statuses