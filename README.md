# SimulationJ
A repository based on the Run Group M repository for generating GEMC samples.

## Files to use

- **Convert GENIE to LUND files**:
```
Simulation/submit/my_submissions/GENIE_to_LUND_converter/GENIE_to_LUND_converter.C
```
- **Script for submitting Slurm jobs for passing GENIE samples through GEMC**:
```
Simulation/submit/my_submissions/submit_GENIE_sample.sh
```
- **Script for setting ifarm jobs and submitting them (uniform samples)**:
```
Simulation/submit/my_submissions/uniform_setup_and_submit.csh
```


# Run Group M 
A repository for Run Group M. 

# Setting up your environment on the JLab Farm

```
module use /scigroup/cvmfs/hallb/clas12/sw/modulefiles
module purge
module load sqlite/dev
module load clas12
module switch coatjava/10.0.2
```

alternatively 

```
source environment.csh
```

# Installing 

```
mkdir build
cd build
cmake /path/to/rgm/repo/
make
```