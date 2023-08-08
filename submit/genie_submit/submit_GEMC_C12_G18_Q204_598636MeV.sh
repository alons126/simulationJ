#!/bin/bash                                                                                                          
#SBATCH --nodes=1                                                                                                    
#SBATCH --ntasks=1                                                                                                   
#SBATCH --mem-per-cpu=2000                                                                                            
#SBATCH --account=clas12                                                                                             
#SBATCH --job-name=C12_G18_02a_00_000_Q204_598636MeV_T1
#SBATCH --partition=production                                                               
#SBATCH --time=20:00:00                                                                                               
#SBATCH --output=/farm_out/%u/%x-%j-%N.out                                                                           
#SBATCH --error=//farm_out/%u/%x-%j-%N.err                                                                           
#SBATCH --array=1-2400

NEVENTS=10000
TORUS=-1.0
FILE_PREFIX=C12_G18_02a_00_000_Q204_598636MeV #Change file prefix for your simulation

GCARD=/u/home/asportes/clas12simulations/simulationJ/submit/rgm.gcard
YAML=/u/home/asportes/clas12simulations/simulationJ/submit/rgm_mc.yaml
# GCARD=rgm.gcard
# YAML=rgm_mc.yaml

source /etc/profile.d/modules.sh
source /group/clas12/packages/setup.sh
module load clas12/dev # new
module switch gemc/5.1 # new
module load sqlite/dev
#module load clas12/pro
# I had to switch to the software versions Justin uses to avoid a seg fault

gemc -USE_GUI=0  -SCALE_FIELD="TorusSymmetric, $TORUS" -SCALE_FIELD="clas12-newSolenoid, -1.0" -N=10000 -INPUT_GEN_FILE="lund, /lustre19/expphy/volatile/clas12/asportes/simulationFiles/C12_G18_02a_00_000_Q204_598636MeV/lundfiles/${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}.dat" -OUTPUT="hipo, /lustre19/expphy/volatile/clas12/asportes/simulationFiles/C12_G18_02a_00_000_Q204_598636MeV/mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus$TORUS.hipo" $GCARD
echo FINISHED GEMC
recon-util -y $YAML -i /lustre19/expphy/volatile/clas12/asportes/simulationFiles/C12_G18_02a_00_000_Q204_598636MeV/mchipo/mc_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo -o /lustre19/expphy/volatile/clas12/asportes/simulationFiles/C12_G18_02a_00_000_Q204_598636MeV/reconhipo/recon_${FILE_PREFIX}_${SLURM_ARRAY_TASK_ID}_torus${TORUS}.hipo
