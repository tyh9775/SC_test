#!/bin/bash
#------ slurm option --------#
#SBATCH --partition=mpc
#SBATCH --account=RB230064
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=8G
#SBATCH --time=03:00:00
#SBATCH -o "/data/RB230064/teajun/slurm_log/spring/single_dens/slurm_%A.log"
#------- Program execution -------#

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
# echo "SLURM_CPUS_PER_TASK=${SLURM_CPUS_PER_TASK}"
#source /data/RB230064/curtis/env_Submit.sh
source /data/RB230064/common/spiritroot/Submit/SpiRITROOT_WMU/env_Submit.sh
#source /data/RB230064/common/spiritroot/Hokusai/SpiRITROOT_WMU/env_Hokusai.sh
srun --cpus-per-task=${SLURM_CPUS_PER_TASK} root -l -q -b "run_reco_single_edens.C($1, $2, $3,\"reco.2024\", \"$4\", $5, $6, $7, $8, $9, ${10}, ${11})"



#try 2500 events per split



