#!/bin/csh

echo
echo "------------------ GENIE to LUND converter ------------------"
echo
echo "----------------------- Job parameters ----------------------"
echo

# Set base directory
unsetenv NUM_OF_FILES
setenv NUM_OF_FILES 10
echo "NUM_OF_FILES = ${NUM_OF_FILES}"
echo ""

# Set base directory
unsetenv BASE_TL_SAMPLE_DIR
setenv BASE_TL_SAMPLE_DIR /w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples
echo "BASE_TL_SAMPLE_DIR = ${BASE_TL_SAMPLE_DIR}"
echo ""

# Set target nucleus
unsetenv TL_SAMPLE_TARGET
# setenv TL_SAMPLE_TARGET H1
# setenv TL_SAMPLE_TARGET D2
setenv TL_SAMPLE_TARGET C12
# setenv TL_SAMPLE_TARGET Ar40
echo "TL_SAMPLE_TARGET = ${TL_SAMPLE_TARGET}"
echo ""

# Set Z and A for selected target
unsetenv TL_SAMPLE_TARGET_A # Mass number = the total number of protons and neutrons in an atomic nucleus
unsetenv TL_SAMPLE_TARGET_Z # Atomic number = the number of protons, which determines the chemical element

if ("${TL_SAMPLE_TARGET}" == "H1") then
    setenv TL_SAMPLE_TARGET_A 1
    setenv TL_SAMPLE_TARGET_Z 1
else if ("${TL_SAMPLE_TARGET}" == "D2") then
    setenv TL_SAMPLE_TARGET_A 2
    setenv TL_SAMPLE_TARGET_Z 1
else if ("${TL_SAMPLE_TARGET}" == "C12") then
    setenv TL_SAMPLE_TARGET_A 12
    setenv TL_SAMPLE_TARGET_Z 6
else if ("${TL_SAMPLE_TARGET}" == "Ar40") then
    setenv TL_SAMPLE_TARGET_A 40
    setenv TL_SAMPLE_TARGET_Z 18
else
    echo "Unknown target: ${TL_SAMPLE_TARGET}"
    exit 1
endif

echo "TL_SAMPLE_TARGET_A = ${TL_SAMPLE_TARGET_A}"
echo "TL_SAMPLE_TARGET_Z = ${TL_SAMPLE_TARGET_Z}"
echo ""

# Set GENIE tune
unsetenv TL_GENIE_TUNE
setenv TL_GENIE_TUNE G18_10a_00_000
# setenv TL_GENIE_TUNE GEM21_11a_00_000
echo "TL_GENIE_TUNE = ${TL_GENIE_TUNE}"
echo ""

# Set beam energy
unsetenv TL_SAMPLE_ENERGY
setenv TL_SAMPLE_ENERGY 2070MeV
# setenv TL_SAMPLE_ENERGY 4029MeV
# setenv TL_SAMPLE_ENERGY 5986MeV
echo "TL_SAMPLE_ENERGY = ${TL_SAMPLE_ENERGY}"
echo ""

# Set Q2 cut based on energy
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

# Expand root files using shell globbing
set INPUT_FILES_DIR = ${BASE_TL_SAMPLE_DIR}/${TL_SAMPLE_TARGET}/${TL_GENIE_TUNE}/${TL_SAMPLE_ENERGY}_${TL_SAMPLE_Q2_CUT}/master-routine_validation_01-eScattering/*.root
echo "INPUT_FILES_DIR = ${INPUT_FILES_DIR}"
echo ""

# Execute ROOT macro
root -q -b './GENIE_to_LUND_converter/GENIE_to_LUND_converter.C("'$INPUT_FILES_DIR'", "'$NUM_OF_FILES'", "'$TL_SAMPLE_TARGET'", "'$TL_SAMPLE_TARGET_A'", '$TL_SAMPLE_TARGET_Z')'

echo
echo "----------------------- End of job --------------------------"
echo