#!/bin/bash -x
#SBATCH --account=training2022
#SBATCH --nodes=1
#SBATCH --ntasks=2
#SBATCH --output=netCDF_hands_on.%j.out
#SBATCH --error=netCDF_hands_on.%j.err
#SBATCH --time=00:05:00

source ../../../intel.env

srun ./hands_on_par_read
