#!/bin/csh

source Q2_setup_and_submit.csh

setenv TARGET C12
setenv GENIE_TUNE G18_10a_00_000
setenv BEAM_E 4029MeV

# Define the function to loop over directories and list their contents
function Submit_jobs {
    foreach dir ( \
        Q2_0_02 Q2_0_03 Q2_0_04 Q2_0_05 \
        Q2_0_06 Q2_0_07 Q2_0_08 Q2_0_09 \
        Q2_0_10 Q2_0_11 Q2_0_12 Q2_0_13 \
        Q2_0_14 Q2_0_15 Q2_0_16 Q2_0_17 \
        Q2_0_18 Q2_0_19 Q2_0_20 Q2_0_21 \
        Q2_0_22 Q2_0_23 Q2_0_24 Q2_0_25 \
        Q2_0_26 Q2_0_27 Q2_0_28 Q2_0_29 \
        Q2_0_30 Q2_0_31 Q2_0_32 Q2_0_33 \
        Q2_0_34 Q2_0_35 Q2_0_36 Q2_0_37 \
        Q2_0_38 Q2_0_39 Q2_0_40 )
        echo
        echo "- Submmiting ${TARGET}_${GENIE_TUNE}_${dir}_${BEAM_E} jobs ------------"
        Q2_setup_and_submit TARGET GENIE_TUNE dir BEAM_E
        echo
    end
}