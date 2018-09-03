#!/bin/bash
# Slurm job submission script for MOpS.
# It is being used by a wrapper script.
# DO NOT EXECUTE THIS DIRECTLY ON THE COMMAND LINE!
# DO NOT SUBMIT THIS DIRECTLY TO SLURM!

#SBATCH -p sandybridge                       # partition (queue)
#SBATCH --mem 32768                          # real memory required per node [MB]
#SBATCH --time 7-0:0                         # wall-time limit [D-HH:MM]
#SBATCH --output slurm.%u.%N.%j.stdout.txt   # (%u,%N,%j)=(user, node, job allocation #)  
#SBATCH --error slurm.%u.%N.%j.errout.txt    #
#SBATCH --mail-type=ALL                      # notifications for job done & fail

TheFolder="Job$SLURM_JOBID"
mkdir $TheFolder

echo "Copying bin folder"
cp -rf $1 $TheFolder
echo 
echo "Changing to folder: $(hostname)$TheFolder/$1"
cd $TheFolder/$1
echo

echo "Launching MOpS ($2)..."
echo
for t in '1' '2' '3' '4' '5' '6' '7' '8' '9' '10' '11' '12'
do
	dirname=swa$t
        mkdir $dirname
        cd $dirname
	srun --exclusive --mem=4000 --ntasks=1 $2 --diags -p -w --strang --ensemble -t ../therm.dat -r ../mops-hm-s1.xml -c ../chem.inp -s ../sweep-fo-detailed-w3-$t.xml &
        cd ..
done
for t in '1' '2' '3' '4' '5'
do
	dirname=dsa$t
        mkdir $dirname
        cd $dirname
	srun --exclusive --mem=4000 --ntasks=1 $2 --diags -p -w --strang --ensemble -t ../therm.dat -r ../mops-hm-s1.xml -c ../chem.inp -s ../sweep-fo-detailed-$t.xml &
        cd ..
done
wait
cd ../../


echo
echo 'Slurm job diagnostics:'
sacct --job $SLURM_JOBID --format "JobName,Submit,Elapsed,AveCPU,CPUTime,UserCPU,TotalCPU,NodeList,NTasks,AveDiskRead,AveDiskWrite"