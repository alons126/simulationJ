#!/bin/bash

# Set environment variables
TARGET="C12"
echo -e "TARGET:\t${TARGET}"
GENIE_TUNE="G18_10a_00_000"
echo -e "GENIE_TUNE:\t${GENIE_TUNE}"
BEAM_E="4029MeV"
echo -e "BEAM_E:\t${BEAM_E}"
CLEAR_FARM_OUT="false"
echo -e "CLEAR_FARM_OUT:\t${CLEAR_FARM_OUT}"

# Set a base path for JOB_OUT_PATH before using it
BASE_PATH="/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples"

# Pull updates
echo
echo "Pulling updates..."
git pull
echo

# Define Q2_CUT loop values
Q2_CUT_LIST=(
    Q2_0_02 Q2_0_03 Q2_0_04 Q2_0_05
    Q2_0_06 Q2_0_07 Q2_0_08 Q2_0_09
    Q2_0_10 Q2_0_11 Q2_0_12 Q2_0_13
    Q2_0_14 Q2_0_15 Q2_0_16 Q2_0_17
    Q2_0_18 Q2_0_19 Q2_0_20 Q2_0_21
    Q2_0_22 Q2_0_23 Q2_0_24 Q2_0_25
    Q2_0_26 Q2_0_27 Q2_0_28 Q2_0_29
    Q2_0_30 Q2_0_31 Q2_0_32 Q2_0_33
    Q2_0_34 Q2_0_35 Q2_0_36 Q2_0_37
    Q2_0_38 Q2_0_39 Q2_0_40
)

for Q2_CUT in "${Q2_CUT_LIST[@]}"; do
    # Construct the full JOB_OUT_PATH
    JOB_OUT_PATH="${BASE_PATH}/${GENIE_TUNE}/Q2_th_test_samples/${BEAM_E}/${Q2_CUT}"

    # Determine the correct submit script path based on BEAM_E
    SUBMIT_SCRIPT_PATH="./${TARGET}_Q2_sample_${BEAM_E}/"

    # Optionally clear the farm_out directory
    if [[ "${CLEAR_FARM_OUT}" == "true" ]]; then
        echo
        echo "Clearing farm_out directory..."
        rm /u/scifarm/farm_out/asportes/*
        echo
    fi

    echo
    echo "Removing old directory structure for MC simulation..."
    rm -rf "${JOB_OUT_PATH}/mchipo" "${JOB_OUT_PATH}/reconhipo" "${JOB_OUT_PATH}/rootfiles"
    echo

    echo
    echo "Setting up directory structure for MC simulation..."
    mkdir -p "${JOB_OUT_PATH}/mchipo" "${JOB_OUT_PATH}/reconhipo" "${JOB_OUT_PATH}/rootfiles"
    echo

    echo
    echo "Submitting sbatch jobs for ${TARGET} at BeamE = ${BEAM_E} with a ${Q2_CUT} cut..."
    sbatch "${SUBMIT_SCRIPT_PATH}/submit_GEMC_Q2.sh"
    echo
done