#!/bin/csh

setenv TARGET C12
setenv GENIE_TUNE G18_10a_00_000
setenv Q2_CUT 0_03
setenv BEAM_E 2070MeV

setenv TARGET_TYPE 4-foil
setenv TARGET_A 12
setenv TARGET_Z 6
setenv NUM_OF_FILES 5

setenv TRUTH_SAMPLE_INPUT_DIR /w/hallb-scshelf2102/clas12/asportes/2N_Analysis_Truth_Samples/${TARGET}/${GENIE_TUNE}/Q2_th_test_samples/${BEAM_E}
setenv TRUTH_SAMPLE_ROOT_FILE_PREFIX ${TARGET}_${GENIE_TUNE}_Q2_${Q2_CUT}_${BEAM_E}
setenv TRUTH_SAMPLE_ROOT_FILE ${TRUTH_SAMPLE_ROOT_FILE_PREFIX}.root

setenv RECO_SAMPLES_TOPDIR /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples
setenv RECO_SAMPLES_SUBDIR master-routine_validation_01-eScattering
setenv RECO_SAMPLES_LUNDDIR lundfiles
mkdir -p ${RECO_SAMPLES_TOPDIR}/${TARGET}
mkdir -p ${RECO_SAMPLES_TOPDIR}/${TARGET}/${GENIE_TUNE}
mkdir -p ${RECO_SAMPLES_TOPDIR}/${TARGET}/${GENIE_TUNE}/Q2_th_test_samples
setenv RECO_SAMPLE_OUTPUT_DIR ${RECO_SAMPLES_TOPDIR}/${TARGET}/${GENIE_TUNE}/Q2_th_test_samples/${BEAM_E}
rm -rf ${RECO_SAMPLE_OUTPUT_DIR}
mkdir ${RECO_SAMPLE_OUTPUT_DIR}
mkdir ${RECO_SAMPLE_OUTPUT_DIR}/lundfiles
mkdir ${RECO_SAMPLE_OUTPUT_DIR}/mchipo
mkdir ${RECO_SAMPLE_OUTPUT_DIR}/reconhipo


root -l 'GENIE_to_LUND.C(gSystem->Getenv("TRUTH_SAMPLE_INPUT_DIR")/gSystem->Getenv("RECO_SAMPLES_SUBDIR")/gSystem->Getenv("TRUTH_SAMPLE_ROOT_FILE"),gSystem->Getenv("RECO_SAMPLE_OUTPUT_DIR")/gSystem->Getenv("RECO_SAMPLES_LUNDDIR"),gSystem->Getenv("TRUTH_SAMPLE_ROOT_FILE_PREFIX"),gSystem->Getenv("NUM_OF_FILES"),gSystem->Getenv("TARGET_TYPE"),gSystem->Getenv("TARGET_A"),gSystem->Getenv("TARGET_Z"))'
# root -l 'GENIE_to_LUND.C(${TRUTH_SAMPLE_INPUT_DIR}/master-routine_validation_01-eScattering/${TRUTH_SAMPLE_ROOT_FILE},${RECO_SAMPLE_OUTPUT_DIR}/lundfiles,${TRUTH_SAMPLE_ROOT_FILE_PREFIX},${NUM_OF_FILES},${TARGET_TYPE},${TARGET_A},${TARGET_Z})'

