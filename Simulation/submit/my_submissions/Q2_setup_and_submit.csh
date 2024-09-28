#!/bin/csh

# # Example of calling the function with a beam energy and enabling farm_out clearing
# uniform_setup_and_submit "5986MeV" true # Won’t clear farm_out.
# uniform_setup_and_submit "5986MeV"      # Will clear farm_out.

function Q2_setup_and_submit (TARGET, GENIE_TUNE, Q2_CUT, BEAM_E, clear_farm_out = false) {
    # Set paths based on BEAM_E
    setenv JOB_OUT_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/${JOB_OUT_PATH}/${GENIE_TUNE}/Q2_th_test_samples/${BEAM_E}/${Q2_CUT}

    # Determine the correct submit script path based on BEAM_E
    if (${BEAM_E} == "5986MeV") then
        setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_6GeV/
    else if (${BEAM_E} == "4029MeV") then
        setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_4GeV/
    else if (${BEAM_E} == "2070MeV") then
        setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_2GeV/
    endif

    echo
    echo "Pulling updates..."
    git pull
    echo

    # Optionally clear the farm_out directory
    if (${clear_farm_out}) then
        echo
        echo "Clearing farm_out directory..."
        rm /u/scifarm/farm_out/asportes/*
        echo
    endif

    echo
    echo "Removing old directory structure for MC simulation here..."
    rm -rf ${JOB_OUT_PATH}/mchipo
    rm -rf ${JOB_OUT_PATH}/reconhipo
    rm -rf ${JOB_OUT_PATH}/rootfiles
    echo

    echo
    echo "Setting up directory structure for MC simulation here..."
    mkdir ${JOB_OUT_PATH}/mchipo ${JOB_OUT_PATH}/reconhipo ${JOB_OUT_PATH}/rootfiles
    echo

    echo
    echo "Submitting sbatch jobs for ${TARGET} at BeamE = ${BEAM_E} with a ${Q2_CUT} cut..."
    sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_Q2.sh
    echo
}