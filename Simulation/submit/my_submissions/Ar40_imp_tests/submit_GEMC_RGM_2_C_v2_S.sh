#!/bin/bash                                                                                                          
#SBATCH --nodes=1                                                                                                    
#SBATCH --ntasks=1                                                                                                   
#SBATCH --mem-per-cpu=2000                                                                                            
#SBATCH --account=clas12                                                                                             
#SBATCH --job-name=RGM_2_C_v2_S_sample_4GeV_test_9                                                                                             
#SBATCH --partition=production                                                               
#SBATCH --time=20:00:00                                                                                               
#SBATCH --output=/farm_out/%u/%x-%j-%N.out                                                                           
#SBATCH --error=//farm_out/%u/%x-%j-%N.err                                                                           
#SBATCH --array=1-10 #Number of files 1-N                                                                                                 

echo "GEMC_DATA_DIR = $GEMC_DATA_DIR"
echo

NEVENTS=10000
#-1.0 for inbending(6,4 GeV) 0.5 for outbending (2 Gev)
TORUS=-1.0 
#Change file prefix for your simulation                                                                                                                          
FILE_PREFIX=C12_G18_10a_00_000_def_Q2_th_4029MeV_lundfile
echo "FILE_PREFIX = ${FILE_PREFIX}"
echo

#set output file path location, don't forget to set up dir using setupdir.sh
OUTPATH=/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_S
# OUTPATH=/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/4029MeV/def_Q2_th_S_NoShift
# OUTPATH=/lustre24/expphy/volatile/clas12/asportes/2N_Analysis_Reco/2N_Analysis_Reco_Samples/C12/G18_10a_00_000/5986MeV/Q2_0_4_th_NoShift
echo "OUTPATH = ${OUTPATH}"
SUBMIT_SCRIPT_DIR=/u/home/asportes/clas12simulations/simulationJ/Simulation/submit/my_submissions/Ar40_imp_tests
echo "SUBMIT_SCRIPT_DIR = ${SUBMIT_SCRIPT_DIR}"
echo

#choose the Gcard for your target type
GCARD=${SUBMIT_SCRIPT_DIR}/rgm_fall2021_RGM_2_C_v2_S.gcard
echo "GCARD = ${GCARD}"
#Reconstruction yaml file
YAML=${SUBMIT_SCRIPT_DIR}/rgm_fall2021-ai_4Gev.yaml
echo "YAML = ${YAML}"
echo

#------DONT NEED TO TOUCH UNDER HERE UNLESS YOU NEED TOO------
LUNDOUT=${OUTPATH}/lundfiles
MCOUT=${OUTPATH}/mchipo
RECONOUT=${OUTPATH}/reconhipo
rm -rf ${RECONOUT}/*

#SUBMIT GEMC MC
echo
echo
echo "============================================================================================="
echo "RUNNING GEMC"
echo "============================================================================================="
echo
echo
gemc -USE_GUI=0  -SCALE_FIELD="binary_torus, $TORUS" -SCALE_FIELD="binary_solenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, ${LUNDOUT}/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.txt" -OUTPUT="hipo, ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD
# gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, ${LUNDOUT}/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.txt" -OUTPUT="hipo, ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD

#RECONSTRUCTION
echo
echo
echo "============================================================================================="
echo "RUNNING COTJAVA"
echo "============================================================================================="
echo
echo
recon-util -y $YAML -n $NEVENTS -i ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o ${RECONOUT}/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo
