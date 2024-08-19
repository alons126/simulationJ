#!/bin/csh

setenv JOB_OUT_PATH /lustre19/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/598636MeV/
setenv SUBMIT_SCRIPT_PATH ./Uniform_sample_6GeV/

echo
echo "Pulling updates..."
git pull
echo

echo
echo "Clearing farm_out directory..."
rm /u/scifarm/farm_out/asportes/*
echo

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
echo "Submitting sbatch job..."
sbatch ${SUBMIT_SCRIPT_PATH}/submit_GEMC_uniform.sh
echo
