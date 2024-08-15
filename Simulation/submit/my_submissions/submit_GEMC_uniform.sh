#!/bin/bash                                                                                                          
#SBATCH --nodes=1                                                                                                    
#SBATCH --ntasks=1                                                                                                   
#SBATCH --mem-per-cpu=2000                                                                                            
#SBATCH --account=clas12                                                                                             
#SBATCH --job-name=mc_rgm_gcf                                                                                              
#SBATCH --partition=production                                                               
#SBATCH --time=20:00:00                                                                                               
#SBATCH --output=/farm_out/%u/%x-%j-%N.out                                                                           
#SBATCH --error=//farm_out/%u/%x-%j-%N.err                                                                           
#SBATCH --array=1-10 #Number of files 1-N                                                                                                

NEVENTS=10000
#-1.0 for inbending(6,4 GeV) 0.5 for outbending (2 Gev)
TORUS=-1.0 
#Change file prefix for your simulation                                                                                                                          
FILE_PREFIX=Uniform_sample_598636MeV

#set output file path location, don't forget to set up dir using setupdir.sh
OUTPATH=/lustre19/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/598636MeV/

#choose the Gcard for your target type
GCARD=/u/home/asportes/clas12simulations/simulationJ/NeutronVeto/Simulation/rgm.gcard
##GCARD=./gcards/rgm_calcium_tmp.gcard
#Reconstruction yaml file
YAML=/u/home/asportes/clas12simulations/simulationJ/NeutronVeto/Simulation/rgm_mc.yaml

#------DONT NEED TO TOUCH UNDER HERE UNLESS YOU NEED TOO------
LUNDOUT=${OUTPATH}/lundfiles/
MCOUT=${OUTPATH}/mchipo/
RECONOUT=${OUTPATH}/reconhipo/

##rm -rf ${OUTPATH}/mchipo/* ## remove old files in resubmission (my addition)
##rm -rf ${OUTPATH}/reconhipo/* ## remove old files in resubmission (my addition)
##rm -rf MCOUT ## remove old files in resubmission (my addition)
##rm -rf RECONOUT ## remove old files in resubmission (my addition)
#mkdir mchipo reconhipo rootfiles

source /u/home/asportes/clas12simulations/simulationJ/Simulation/environment_gemc.csh
#source /u/home/asportes/clas12simulations/simulationJ/environment.csh
#source ../../environment_gemc.csh
#source /u/home/asportes/clas12simulations/simulationJ/Simulation/environment_gemc.sh
#source ../environment_gemc.sh ## oritignal

#SUBMIT GEMC MC
gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, ${LUNDOUT}/lund_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.txt" -OUTPUT="hipo, ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD

#RECONSTRUCTION
recon-util -y $YAML -n $NEVENTS -i ${MCOUT}/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o ${RECONOUT}/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo
