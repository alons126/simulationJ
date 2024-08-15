#!/bin/bash                                                                                                          

OUTPATH0=/lustre19/expphy/volatile/clas12/asportes/2N_Analysis_Reco/Uniform_e-p-n_samples/598636MeV/

echo
echo "Clearing farm_out directory..."
rm /u/scifarm/farm_out/asportes/*
echo

echo
echo "Removing old directory structure for MC simulation here..."
rm -rf ${OUTPATH0}/mchipo
rm -rf ${OUTPATH0}/reconhipo
rm -rf ${OUTPATH0}/rootfiles
echo

echo
echo "Setting up directory structure for MC simulation here..."
mkdir ${OUTPATH0}/mchipo ${OUTPATH0}/reconhipo ${OUTPATH0}/rootfiles
echo

echo
echo "Submitting sbatch job..."
sbatch submit_GEMC_uniform.sh
echo