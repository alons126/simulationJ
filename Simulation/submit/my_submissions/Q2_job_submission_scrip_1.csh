#!/bin/csh

setenv TARGET C12
echo "TARGET:\t\t\t${TARGET}"
setenv GENIE_TUNE G18_10a_00_000
echo "GENIE_TUNE:\t\t${GENIE_TUNE}"
setenv BEAM_E 4029MeV
echo "BEAM_E:\t\t\t${BEAM_E}"
setenv CLEAR_FARM_OUT "true"
echo "CLEAR_FARM_OUT:\t\t${CLEAR_FARM_OUT}"
echo

# # Set a base path for JOB_OUT_PATH before using it
# setenv BASE_PATH /lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples
# echo "BASE_PATH:\t\t${BASE_PATH}"
# echo

# Determine the correct submit script path based on BEAM_E
setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_${BEAM_E}/
echo "SUBMIT_SCRIPT_PATH:\t${SUBMIT_SCRIPT_PATH}"
echo

# Determine the correct submit script path based on BEAM_E
setenv SUBMIT_SCRIPT_PATH ./${TARGET}_Q2_sample_${BEAM_E}/

# echo
# echo "Pulling updates..."
# git pull
# echo

# Optionally clear the farm_out directory
if ("${CLEAR_FARM_OUT}" == "true") then
    echo
    echo "Clearing farm_out directory..."
    rm /u/scifarm/farm_out/asportes/*
    echo
endif

# Loop over directories and submit jobs
foreach Q2_CUT ( \
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
    echo "#!/bin/bash"                                                                                                          
    echo "#SBATCH --nodes=1"                                                                                                    
    echo "#SBATCH --ntasks=1"                                                                                                   
    echo "#SBATCH --mem-per-cpu=2000"                                                                                            
    echo "#SBATCH --account=clas12"                                                                                             
    echo "#SBATCH --job-name=C12_Q2_sample_4GeV"                                                                                           
    echo "#SBATCH --partition=production"                                                               
    echo "#SBATCH --time=20:00:00"                                                                                               
    echo "#SBATCH --output=/farm_out/%u/%x-%j-%N.out"                                                                           
    echo "#SBATCH --error=//farm_out/%u/%x-%j-%N.err"                                                                           
    echo "#SBATCH --array=1-10 #Number of files 1-N"                                                                                                
    echo 
    echo "NEVENTS=10000"
    echo "#-1.0 for inbending(6,4 GeV) 0.5 for outbending (2 Gev)"
    echo "TORUS=-1.0" 
    echo "#Change file prefix for your simulation"                                                                                                                          
    echo "FILE_PREFIX=${TARGET}_${GENIE_TUNE}_${Q2_CUT}_${BEAM_E}"
    echo 
    echo "#set output file path location, don't forget to set up dir using setupdir.sh"
    echo "OUTPATH=${JOB_OUT_PATH}"
    echo "SUBMIT_SCRIPT_DIR=/u/home/asportes/clas12simulations/simulationJ/Simulation/submit/my_submissions/${TARGET}_Q2_sample_${BEAM_E}"
    echo 
    echo "#choose the Gcard for your target type"
    echo "GCARD=${SUBMIT_SCRIPT_DIR}/rgm_fall2021_C.gcard"
    echo "#Reconstruction yaml file"
    echo "YAML=${SUBMIT_SCRIPT_DIR}/rgm_fall2021-ai_4Gev.yaml"
    echo 
    echo "#------DONT NEED TO TOUCH UNDER HERE UNLESS YOU NEED TOO------"
    echo "LUNDOUT=${OUTPATH}/lundfiles"
    echo "MCOUT=${OUTPATH}/mchipo"
    echo "RECONOUT=${OUTPATH}/reconhipo"
    echo 
    echo "#SUBMIT GEMC MC"
    echo "gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, ${LUNDOUT}/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.txt" -OUTPUT="hipo, ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD"
    echo 
    echo "#RECONSTRUCTION"
    echo "recon-util -y $YAML -n $NEVENTS -i ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o ${RECONOUT}/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo"
end