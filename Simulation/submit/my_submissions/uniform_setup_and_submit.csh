#!/bin/csh

# A scri

# Setup
# ---------------------------------------------------------------------------

source ./scripts/set_env.csh

unsetenv BEAM_E
setenv BEAM_E 2070MeV
# setenv BEAM_E 4029MeV
# setenv BEAM_E 5986MeV
echo "${COLOR_START}BEAM_E:${COLOR_END} ${BEAM_E}"
echo

unset TARGET_VARIATION
setenv TARGET_VARIATION rgm_fall2021_C_S
echo "TARGET_VARIATION: ${TARGET_VARIATION}"
echo

unsetenv CLEAR_FAR_OUT
setenv CLEAR_FAR_OUT false
echo "${COLOR_START}CLEAR_FAR_OUT:${COLOR_END} ${CLEAR_FAR_OUT}"
echo

unset CUSTOM_GEMC_VERSION
setenv CUSTOM_GEMC_VERSION 1 # 1 for true, 0 for false
echo "CUSTOM_GEMC_VERSION: ${CUSTOM_GEMC_VERSION}"
echo

unset GEMC_VERSION
setenv GEMC_VERSION dev
echo "GEMC_VERSION: ${GEMC_VERSION}"
echo

unset NUM_OF_JOBS
setenv NUM_OF_JOBS 2500
echo "NUM_OF_JOBS: ${NUM_OF_JOBS}"
echo

# Main Script
# ---------------------------------------------------------------------------
banner "Starting uniform generation and submission for BeamE = ${BEAM_E}"

# Set paths based on BEAM_E
unsetenv OUTPATH_BASE
setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_devGEMC
echo "${COLOR_START}OUTPATH_BASE: ${COLOR_END}${OUTPATH_BASE}"
echo

# Check if OUTPATH_BASE is a directory
Check_if_dir_exist "${OUTPATH_BASE}"

# 
# Loop over particle types
# ---------------------------------------------------------------------------
foreach OUTPATH_PARTICLE ( 1e ep en )
    echo "${COLOR_START}Processing particle type:${COLOR_END} ${OUTPATH_PARTICLE}"
    echo

    # Set paths based on particle type
    # ---------------------------------------------------------------------------
    unsetenv OUTPATH
    setenv OUTPATH ${OUTPATH_BASE}/OutPut_${OUTPATH_PARTICLE}
    echo "${COLOR_START}OUTPATH:${COLOR_END} ${OUTPATH}"
    echo

    # Check if OUTPATH_<PARTICLE> is a directory
    Check_if_dir_exist "${OUTPATH}"

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
    Check_if_dir_exist "${SUBMIT_SCRIPT_PATH}"

    # Setting GCARD_FILE
    unsetenv GCARD_FILE
    setenv GCARD_FILE ${SUBMIT_SCRIPT_PATH}/${TARGET_VARIATION}_${BEAM_E_ROUNDED}
    echo "${COLOR_START}GCARD_FILE:${COLOR_END} ${GCARD_FILE}"
    echo

    # Check if GCARD_FILE is a file
    Check_if_file_exist "${GCARD_FILE}"

    # Git operations
    # ---------------------------------------------------------------------------
    source ./scripts/update_script.csh

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
    Check_if_file_exist "${YAML_FILE}"

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
