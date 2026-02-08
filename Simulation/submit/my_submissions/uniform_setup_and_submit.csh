#!/bin/csh

# A scri

# Setup environment
# ======================================================================================================
source ./scripts/set_env.csh
echo

# Script banner
# ======================================================================================================
echo ""
echo "${SYSTEM_COLOR}///////////////////////////////////////////////////////////////////////${COLOR_END}"
printf "%s%s%s\n" "${SYSTEM_COLOR}//${COLOR_END}        Setting and submitting uniform sample generation jobs      ${SYSTEM_COLOR}//${COLOR_END}"
echo "${SYSTEM_COLOR}///////////////////////////////////////////////////////////////////////${COLOR_END}"
echo ""

# Setup environment variables and paths for uniform sample generation and submission
# ======================================================================================================
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s\n" "${COLOR_START}= Setup environment variables and paths                               =${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

# unsetenv BEAM_E
# setenv BEAM_E 2070MeV
# # setenv BEAM_E 4029MeV
# # setenv BEAM_E 5986MeV
# echo "${COLOR_START}BEAM_E:${COLOR_END}              ${BEAM_E}"
# echo

unset TARGET_VARIATION
setenv TARGET_VARIATION rgm_fall2021_Ar
echo "${COLOR_START}TARGET_VARIATION:${COLOR_END}    ${TARGET_VARIATION}"
echo

unsetenv CLEAR_FAR_OUT
setenv CLEAR_FAR_OUT false
echo "${COLOR_START}CLEAR_FAR_OUT:${COLOR_END}       ${CLEAR_FAR_OUT}"
echo

unset CUSTOM_GEMC_VERSION
setenv CUSTOM_GEMC_VERSION true
echo "${COLOR_START}CUSTOM_GEMC_VERSION:${COLOR_END} ${CUSTOM_GEMC_VERSION}"
echo

unset GEMC_VERSION
setenv GEMC_VERSION dev
echo "${COLOR_START}GEMC_VERSION:${COLOR_END}        ${GEMC_VERSION}"
echo

unset NUM_OF_JOBS
# setenv NUM_OF_JOBS 10
# setenv NUM_OF_JOBS 100
# setenv NUM_OF_JOBS 2500
# setenv NUM_OF_JOBS 5000
setenv NUM_OF_JOBS 7500
echo "${COLOR_START}NUM_OF_JOBS:${COLOR_END}         ${NUM_OF_JOBS}"
echo

# Main Script
# ======================================================================================================
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}= " "Starting uniform generation and submission for BeamE =${COLOR_END} ${BEAM_E}" "      ${COLOR_START}=${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

# Set paths based on BEAM_E and TARGET_VARIATION for uniform sample generation and submission. These environment variables will be used in the uniform sample generation and submission scripts to ensure that the correct paths and configurations are used for each beam energy and target variation.
unsetenv OUTPATH_BASE
# setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_devGEMC
setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_devGEMC_${TARGET_VARIATION}
# setenv OUTPATH_BASE /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco_Samples/Uniform_e-p-n_samples/${BEAM_E}_ConstPn_devGEMC_${TARGET_VARIATION}
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

unsetenv CLAS12TAGS_DIR
setenv CLAS12TAGS_DIR /lustre24/expphy/volatile/clas12/asportes/Ar40_imp_GEMC/clas12Tags
echo "${COLOR_START}CLAS12TAGS_DIR:${COLOR_END} ${CLAS12TAGS_DIR}"

# Check if CLAS12TAGS_DIR is a directory
echo "${COLOR_START}--> Checking if ${COLOR_END}CLAS12TAGS_DIR${COLOR_START} is a directory...${COLOR_END}"
if ( ! -d "${CLAS12TAGS_DIR}" ) then
    printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following directory does not exist: ${CLAS12TAGS_DIR}"
    exit 1
else
    printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}CLAS12TAGS_DIR exists.${COLOR_END}"
    echo
endif

# Handle farm_out directory clearing
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}= Handling farm_out directory clearing and custom GEMC version        =${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

# Optionally clear the farm_out directory
if ("${CLEAR_FAR_OUT}" == "true") then
    echo "${COLOR_START}Clearing farm_out directory...${COLOR_END}"
    echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
    rm /u/scifarm/farm_out/asportes/*
    echo
else
    echo "CLEAR_FAR_OUT$ ${COLOR_START}is set to '${COLOR_END}false${COLOR_START}', skipping farm_out directory clearing...${COLOR_END}"
    echo
endif

# Handle custom GEMC version loading based on environment variables
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}= Handling custom GEMC version                                        =${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""

# Optionally use custom GEMC version and set GEMC_DATA_DIR to a custom path
if ("${CUSTOM_GEMC_VERSION}" == "true") then
    echo "${COLOR_START}Loading GEMC version ${COLOR_END}${GEMC_VERSION}${COLOR_START}...${COLOR_END}"
    echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
    module unload gemc
    module load gemc/${GEMC_VERSION}
    echo

    # Set GEMC data directory to a custom path. This is important to ensure that the correct geometry and configuration files are used for the simulations, especially if using a custom or development version of GEMC.
    unsetenv GEMC_DATA_DIR
    setenv GEMC_DATA_DIR ${CLAS12TAGS_DIR}
    echo "${COLOR_START}GEMC_DATA_DIR:${COLOR_END} ${GEMC_DATA_DIR}"

    # Check if GEMC_DATA_DIR is a directory
    echo "${COLOR_START}--> Checking if ${COLOR_END}GEMC_DATA_DIR${COLOR_START} is a directory...${COLOR_END}"
    if ( ! -d "${GEMC_DATA_DIR}" ) then
        printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following directory does not exist: ${GEMC_DATA_DIR}"
        exit 1
    else
        printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}GEMC_DATA_DIR exists.${COLOR_END}"
        echo
    endif
else
    echo "CUSTOM_GEMC_VERSION$ ${COLOR_START}is set to '${COLOR_END}false${COLOR_START}', skipping custom GEMC version loading...${COLOR_END}"
    echo
endif

# Loop over particle types
# ------------------------------------------------------------------------------------------------------
echo ""
echo "${COLOR_START}=======================================================================${COLOR_END}"
printf "%s%s%s\n" "${COLOR_START}= Looping over particle types                                         =${COLOR_END}"
echo "${COLOR_START}=======================================================================${COLOR_END}"
echo ""
foreach BEAM_E ( 2070MeV 4029MeV 5986MeV )
    # foreach OUTPATH_PARTICLE ( 1e )
    foreach OUTPATH_PARTICLE ( en )
    # foreach OUTPATH_PARTICLE ( 1e ep en )
        echo "${COLOR_START}Processing particle type:${COLOR_END} ${OUTPATH_PARTICLE}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

        # Setup other environment variables based on BEAM_E and particle type
        # ---------------------------------------------------------------------------

        echo "${COLOR_START}OUTPATH_PARTICLE:${COLOR_END} ${OUTPATH_PARTICLE}"
        echo

        # Setup environment variables based on BEAM_E and particle type
        # ---------------------------------------------------------------------------
        echo "${COLOR_START}Setting environment variables based on BEAM_E and particle type ${OUTPATH_PARTICLE}${COLOR_END}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

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
            echo "${COLOR_ERROR_START}Error:${COLOR_END} unknown torus field configuration: ${BEAM_E}"
            exit 1
        endif

        # Set paths based on particle type
        # --------------------------------------------------------------------------------------------------
        echo "${COLOR_START}Setting paths based on particle type ${OUTPATH_PARTICLE}${COLOR_END}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

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

        # Determine the correct submit script path based on BEAM_E
        unsetenv REQUIREMENTS_PATH
        setenv REQUIREMENTS_PATH ./Generation_files_${BEAM_E_ROUNDED}/
        echo "${COLOR_START}REQUIREMENTS_PATH:${COLOR_END} ${REQUIREMENTS_PATH}"
        echo

        # Check if REQUIREMENTS_PATH is a directory
        echo "${COLOR_START}--> Checking if ${COLOR_END}REQUIREMENTS_PATH${COLOR_START} is a directory...${COLOR_END}"
        if ( ! -d "${REQUIREMENTS_PATH}" ) then
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following directory does not exist: ${REQUIREMENTS_PATH}"
            exit 1
        else
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}REQUIREMENTS_PATH exists.${COLOR_END}"
            echo
        endif

        # Set GCARD_FILE and YAML_FILE paths based on BEAM_E and TARGET_VARIATION. These will be used in the uniform sample generation and submission scripts to ensure that the correct configurations are used for each beam energy and target variation.
        # --------------------------------------------------------------------------------------------------
        echo "${COLOR_START}Setting GCARD_FILE and YAML_FILE files based on BEAM_E and TARGET_VARIATION ${OUTPATH_PARTICLE}${COLOR_END}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

        # Setting GCARD_FILE
        unsetenv GCARD_FILE
        setenv GCARD_FILE ${REQUIREMENTS_PATH}/${TARGET_VARIATION}_${BEAM_E_ROUNDED}.gcard
        echo "${COLOR_START}GCARD_FILE:${COLOR_END} ${GCARD_FILE}"

        # Check if GCARD_FILE is a file
        echo "${COLOR_START}--> Checking if ${COLOR_END}GCARD_FILE${COLOR_START} is a file...${COLOR_END}"
        if ( ! -f "${GCARD_FILE}" ) then
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following file does not exist: ${GCARD_FILE}"
            exit 1
        else
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}GCARD_FILE exists.${COLOR_END}"
            echo
        endif

        # Setting YAML_FILE
        unsetenv YAML_FILE
        if ("${BEAM_E}" == "2070MeV") then
            setenv YAML_FILE ${REQUIREMENTS_PATH}/rgm_fall2021-cv.yaml
        else if ("${BEAM_E}" == "4029MeV") then
            setenv YAML_FILE ${REQUIREMENTS_PATH}/rgm_fall2021-ai_4Gev.yaml
        else if ("${BEAM_E}" == "5986MeV") then
            setenv YAML_FILE ${REQUIREMENTS_PATH}/rgm_fall2021-ai_6Gev.yaml
        endif
        echo "${COLOR_START}YAML_FILE:${COLOR_END} ${YAML_FILE}"

        # Check if YAML_FILE is a file
        echo "${COLOR_START}--> Checking if ${COLOR_END}YAML_FILE${COLOR_START} is a file...${COLOR_END}"
        if ( ! -f "${YAML_FILE}" ) then
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following file does not exist: ${YAML_FILE}"
            exit 1
        else
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}YAML_FILE exists.${COLOR_END}"
            echo
        endif

        # Setup output directory structure
        # ---------------------------------------------------------------------------
        echo "${COLOR_START}Setting output directory structure ${OUTPATH_PARTICLE}${COLOR_END}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

        echo "${COLOR_START}Removing old directory structure for MC simulation here...\033[0m"
        rm -rf ${OUTPATH}/mchipo
        rm -rf ${OUTPATH}/reconhipo
        rm -rf ${OUTPATH}/rootfiles
        echo

        echo "${COLOR_START}Setting up directory structure for MC simulation here...\033[0m"
        mkdir ${OUTPATH}/mchipo ${OUTPATH}/reconhipo ${OUTPATH}/rootfiles
        echo

        # Submitting sbatch job
        # ---------------------------------------------------------------------------
        echo "${COLOR_START}Submitting sbatch job for BeamE = ${COLOR_END}${BEAM_E}"
        echo "${COLOR_START}-----------------------------------------------------------------------${COLOR_END}"
        echo

        unsetenv SLURM_JOB_NAME
        setenv SLURM_JOB_NAME Uniform_${OUTPATH_PARTICLE}_sample_${BEAM_E}
        # setenv SLURM_JOB_NAME Uniform_${OUTPATH_PARTICLE}_ConstPn_sample_${BEAM_E}
        echo "${COLOR_START}SLURM_JOB_NAME:${COLOR_END} ${SLURM_JOB_NAME}"
        echo ""

        unsetenv ARRAY
        setenv ARRAY 1-${NUM_OF_JOBS}
        echo "${COLOR_START}ARRAY:${COLOR_END} ${ARRAY}"
        echo ""

        unsetenv SUBMIT_SCRIPT_FILE
        setenv SUBMIT_SCRIPT_FILE ./scripts/job_submission_scripts/submit_GEMC_uniform.sh
        echo "${COLOR_START}SUBMIT_SCRIPT_FILE:${COLOR_END} ${SUBMIT_SCRIPT_FILE}"

        # Check if SUBMIT_SCRIPT_FILE is a file
        echo "${COLOR_START}--> Checking if ${COLOR_END}SUBMIT_SCRIPT_FILE${COLOR_START} is a file...${COLOR_END}"
        if ( ! -f "${SUBMIT_SCRIPT_FILE}" ) then
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_ERROR_START}Error:${COLOR_END}" " the following file does not exist: ${SUBMIT_SCRIPT_FILE}"
            exit 1
        else
            printf "${COLOR_START}-->${COLOR_END} %s%s%s\n" "${COLOR_GOOD_START}SUBMIT_SCRIPT_FILE exists.${COLOR_END}"
            echo
        endif

        sbatch --job-name="${SLURM_JOB_NAME}" --array=${ARRAY} ${SUBMIT_SCRIPT_FILE}
        echo
    end # end foreach OUTPATH_PARTICLE ( 1e ep en )
end # end foreach BEAM_E ( 2070MeV 4029MeV 5986MeV )
