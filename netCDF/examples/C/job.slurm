#!/bin/bash -x
#SBATCH --account=training2022
#SBATCH --reservation=parallel-io-day1
#SBATCH --nodes=1
#SBATCH --ntasks=2
#SBATCH --output=netCDF_example.%j.out
#SBATCH --error=netCDF_example.%j.err
#SBATCH --time=00:05:00

source ../../intel.env

srun ./example_par_write
