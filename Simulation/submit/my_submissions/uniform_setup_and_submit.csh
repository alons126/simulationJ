#!/bin/csh

# A scri

# Setup environment
# ------------------------------------------------------------------------------------------------------
source ./scripts/set_env.csh
echo

# Script banner
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}///////////////////////////////////////////////////////////////////////${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}//${COLOR_END}        Setting and submitting uniform sample generation jobs      ${COLOR_START}//${COLOR_END}"
echo "${COLOR_START}///////////////////////////////////////////////////////////////////////${COLOR_END}"
echo ""

# Setup environment variables and paths for uniform sample generation and submission
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s\n" "${COLOR_START}= Setup environment variables and paths                               =${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

unsetenv BEAM_E
setenv BEAM_E 2070MeV
# setenv BEAM_E 4029MeV
# setenv BEAM_E 5986MeV
echo "${COLOR_START}BEAM_E:${COLOR_END}              ${BEAM_E}"
echo

unset TARGET_VARIATION
setenv TARGET_VARIATION rgm_fall2021_C_S
echo "${COLOR_START}TARGET_VARIATION:${COLOR_END}    ${TARGET_VARIATION}"
echo

unsetenv CLEAR_FAR_OUT
setenv CLEAR_FAR_OUT false
echo "${COLOR_START}CLEAR_FAR_OUT:${COLOR_END}       ${CLEAR_FAR_OUT}"
echo

unset CUSTOM_GEMC_VERSION
setenv CUSTOM_GEMC_VERSION 1 # 1 for true, 0 for false
echo "${COLOR_START}CUSTOM_GEMC_VERSION:${COLOR_END} ${CUSTOM_GEMC_VERSION}"
echo

unset GEMC_VERSION
setenv GEMC_VERSION dev
echo "${COLOR_START}GEMC_VERSION:${COLOR_END}        ${GEMC_VERSION}"
echo

unset NUM_OF_JOBS
setenv NUM_OF_JOBS 2500
echo "${COLOR_START}NUM_OF_JOBS:${COLOR_END}         ${NUM_OF_JOBS}"
echo

# Main Script
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}= " "Starting uniform generation and submission for BeamE =${COLOR_END} ${BEAM_E}" "      ${COLOR_START}=${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

# Set paths based on BEAM_E and TARGET_VARIATION for uniform sample generation and submission. These environment variables will be used in the uniform sample generation and submission scripts to ensure that the correct paths and configurations are used for each beam energy and target variation.
unsetenv OUTPATH_BASE
# setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_devGEMC
setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_devGEMC_rgm_fall2021_C_S
echo "${COLOR_START}OUTPATH_BASE: ${COLOR_END}${OUTPATH_BASE}"

# Check if OUTPATH_BASE is a directory
echo "${COLOR_START}--> Checking if ${COLOR_END}OUTPATH_BASE${COLOR_START} is a directory...${COLOR_END}"
if ( ! -d "${OUTPATH_BASE}" ) then
    printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following directory does not exist: ${OUTPATH_BASE}"
    exit 1
else
    printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}OUTPATH_BASE exists.${COLOR_END}"
    echo
endif

# Loop over particle types
# ---------------------------------------------------------------------------
echo
foreach OUTPATH_PARTICLE ( 1e ep en )
    echo "${COLOR_START}Processing particle type:${COLOR_END} ${OUTPATH_PARTICLE}"
    echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
    echo

    # Set paths based on particle type
    # ---------------------------------------------------------------------------
    unsetenv OUTPATH
    setenv OUTPATH ${OUTPATH_BASE}/OutPut_${OUTPATH_PARTICLE}
    echo "${COLOR_START}OUTPATH:${COLOR_END} ${OUTPATH}"

    # Check if OUTPATH is a directory
    echo "${COLOR_START}--> Checking if ${COLOR_END}OUTPATH${COLOR_START} is a directory...${COLOR_END}"
    if ( ! -d "${OUTPATH}" ) then
        printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following directory does not exist: ${OUTPATH}"
        exit 1
    else
        printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}OUTPATH exists.${COLOR_END}"
        echo
    endif

    # Setup other environment variables based on BEAM_E and particle type
    # ---------------------------------------------------------------------------

    # Determine the correct submit script path based on BEAM_E
    unsetenv BEAM_E_ROUNDED
    if ("${BEAM_E}" == "2070MeV") then
        setenv BEAM_E_ROUNDED 2GeV
    else if ("${BEAM_E}" == "4029MeV") then
        setenv BEAM_E_ROUNDED 4GeV
    else if ("${BEAM_E}" == "5986MeV") then
        setenv BEAM_E_ROUNDED 6GeV
    endif
    echo "${COLOR_START}BEAM_E_ROUNDED:${COLOR_END} ${BEAM_E_ROUNDED}"
    echo

    # Set torus field based on beam energy
    unsetenv TORUS_FIELD
    if ("${BEAM_E}" == "2070MeV") then
        setenv TORUS_FIELD 0.5
    else if ("${BEAM_E}" == "4029MeV" || "${BEAM_E}" == "5986MeV") then
        setenv TORUS_FIELD -1.0
    else
        echo "Unknown torus field configuration: ${BEAM_E}"
        exit 1
    endif

    # Determine the correct submit script path based on BEAM_E
    unsetenv SUBMIT_SCRIPT_PATH
    setenv SUBMIT_SCRIPT_PATH ./Generation_files_${BEAM_E_ROUNDED}/
    echo "${COLOR_START}SUBMIT_SCRIPT_PATH:${COLOR_END} ${SUBMIT_SCRIPT_PATH}"
    echo

    # Check if SUBMIT_SCRIPT_PATH is a directory
    if ( ! -d "${SUBMIT_SCRIPT_PATH}" ) then
        echo "${COLOR_ERROR_START}Error:${COLOR_END} the following directory does not exist: ${SUBMIT_SCRIPT_PATH}"
        exit 1
    endif

    # Setting GCARD_FILE
    unsetenv GCARD_FILE
    setenv GCARD_FILE ${SUBMIT_SCRIPT_PATH}/${TARGET_VARIATION}_${BEAM_E_ROUNDED}.gcard
    echo "${COLOR_START}GCARD_FILE:${COLOR_END} ${GCARD_FILE}"
    echo

    # Check if GCARD_FILE is a file
    if ( ! -f "${GCARD_FILE}" ) then
        echo "${COLOR_ERROR_START}Error:${COLOR_END} the following file does not exist: ${GCARD_FILE}"
        exit 1
    endif

    # Setting YAML_FILE
    unsetenv YAML_FILE
    if ("${BEAM_E}" == "2070MeV") then
        setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-cv.yaml
    else if ("${BEAM_E}" == "4029MeV") then
        setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-ai_4Gev.yaml
    else if ("${BEAM_E}" == "5986MeV") then
        setenv YAML_FILE ${SUBMIT_SCRIPT_PATH}/rgm_fall2021-ai_6Gev.yaml
    endif
    echo "${COLOR_START}YAML_FILE:${COLOR_END} ${YAML_FILE}"
    echo

    # Check if YAML_FILE is a file
    if ( ! -f "${YAML_FILE}" ) then
        echo "${COLOR_ERROR_START}Error:${COLOR_END} the following file does not exist: ${YAML_FILE}"
        exit 1
    endif

    echo "${COLOR_START}Pulling updates...\033[0m"
    git pull
    echo

    # Optionally clear the farm_out directory
    if ("${CLEAR_FAR_OUT}" == "true") then
        echo
        echo "${COLOR_START}Clearing farm_out directory...\033[0m"
        rm /u/scifarm/farm_out/asportes/*
        echo
    endif

    # Optionally use dev GEMC
    if ("${CUSTOM_GEMC_VERSION}" != "0") then
        echo
        banner "Loading dev GEMC version ----------------------------------------------"
        module unload gemc
        module load gemc/${GEMC_VERSION}
        echo
    endif

    # Setup output directory structure
    # ---------------------------------------------------------------------------
    echo "${COLOR_START}Removing old directory structure for MC simulation here...\033[0m"
    rm -rf ${OUTPATH}_${OUTPATH_PARTICLE}/mchipo
    rm -rf ${OUTPATH}_${OUTPATH_PARTICLE}/reconhipo
    rm -rf ${OUTPATH}_${OUTPATH_PARTICLE}/rootfiles
    echo

    echo "${COLOR_START}Setting up directory structure for MC simulation here...\033[0m"
    mkdir ${OUTPATH}_${OUTPATH_PARTICLE}/mchipo ${OUTPATH}_${OUTPATH_PARTICLE}/reconhipo ${OUTPATH}_${OUTPATH_PARTICLE}/rootfiles
    echo

    echo "${COLOR_START}Submitting en sbatch job for BeamE = \033[0m${BEAM_E}${COLOR_START}...\033[0m"
    unsetenv SLURM_JOB_NAME
    setenv SLURM_JOB_NAME Uniform_${OUTPATH_PARTICLE}_sample_${BEAM_E}
    echo "${COLOR_START}SLURM_JOB_NAME:${COLOR_END} ${SLURM_JOB_NAME}"
    echo ""

    unsetenv ARRAY
    setenv ARRAY 1-${NUM_OF_JOBS}
    echo "${COLOR_START}ARRAY:${COLOR_END} ${ARRAY}"
    echo ""

    # sbatch --job-name="${SLURM_JOB_NAME}" --array=${ARRAY} ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform_${OUTPATH_PARTICLE}.sh
    echo
end # end foreach OUTPATH_PARTICLE ( 1e ep en )
