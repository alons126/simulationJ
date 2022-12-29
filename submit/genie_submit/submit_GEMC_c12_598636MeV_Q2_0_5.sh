#!/bin/bash                                                                                                          
#SBATCH --nodes=1                                                                                                    
#SBATCH --ntasks=1                                                                                                   
#SBATCH --mem-per-cpu=2000                                                                                            
#SBATCH --account=clas12                                                                                             
#SBATCH --job-name=mc_c12_598636MeV_Q2_0_5_test_1
#SBATCH --partition=production                                                               
#SBATCH --time=30:00:00                                                                                               
#SBATCH --output=/farm_out/%u/%x-%j-%N.out
#SBATCH --error=//farm_out/%u/%x-%j-%N.err                                                                           
#SBATCH --array=1-1000

NEVENTS=10000
TORUS=-1.0
FILE_PREFIX=c12_598636MeV_Q2_0_5 #Change file prefix for your simulation

GCARD=/u/home/asportes/clas12simulations/simulationJ/submit/rgm.gcard
YAML=/u/home/asportes/clas12simulations/simulationJ/submit/rgm_mc.yaml
# GCARD=./rgm.gcard
# YAML=./rgm_mc.yaml

source /etc/profile.d/modules.sh
source /group/clas12/packages/setup.sh
module load sqlite/dev
module load clas12/dev

gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, /lustre19/expphy/volatile/clas12/asportes/simulationFiles/598636MeV_Q2_0_5_test_1/lundfiles/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.dat" -OUTPUT="hipo, /lustre19/expphy/volatile/clas12/asportes/simulationFiles/598636MeV_Q2_0_5_test_1/mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD

recon-util -y $YAML -i /lustre19/expphy/volatile/clas12/asportes/simulationFiles/598636MeV_Q2_0_5_test_1/mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o /lustre19/expphy/volatile/clas12/asportes/simulationFiles/598636MeV_Q2_0_5_test_1//reconhipo/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo
# gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=$NEVENTS -INPUT_GEN_FILE="lund, ../lundfiles/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.dat" -OUTPUT="hipo, ../mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD
# recon-util -y $YAML -i ../mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o ../reconhipo/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo
