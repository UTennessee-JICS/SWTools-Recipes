#!/bin/bash

############################## standard interface to /sw tools
# Input:
#   Environment variables
#     SW_BLDDIR    current directory (PWD) minus /autofs/na1_ stuff
#     SW_ENVFILE   file to be sourced which has alternate prog environment
#                     only to be used in special circumstances
#     SW_WORKDIR   work dir that local script can use
# Output:
#   Return code of 0=success or 1=failure   or 2=job submitted
#
# Notes:
#   If this script is called from swtest, then swtest requires 
#   SW_WORKDIR to be set.  Then swtest adds a unique path to what 
#   user gave swtest (action+timestamp+build) and provides this
#   script with a uniquely valued SW_WORKDIR.  swtest will
#   automatically remove this unique workspace when retest is done.
##################################################################

# exit 3 is a signal to the sw infrastructure that this template has not 
# been updated; please delete it when ready
#exit 3

if [ -z $SW_BLDDIR ]; then
  echo "Error: SW_BLDDIR not set!"
  exit 1
else
  cd $SW_BLDDIR
fi

if [ -z $SW_ENVFILE ]; then
  ### Set Environment (do not remove this line only change what is in between)
  . ${MODULESHOME}/init/ksh
  . ${SW_BLDDIR}/remodule
  ### End Environment (do not remove this line only change what is in between)
else
  . $SW_ENVFILE
fi

############################## app specific section
#  
set -o verbose
#clear out status file since re-testing
rm -f status 


#-- Use one of the following template: in-place test or test with batch job

#-- 1. in-place test

cd ${SW_WORKDIR}

cp ${SW_BLDDIR}/gromacs-test/* .

cat > gromacs.pbs << EOF
#!/bin/bash
#PBS -N gromacs-test
#PBS -j oe
#PBS -l nodes=1:ppn=16,walltime=10:00

#PBS -V

set -o verbose
cd \$PBS_O_WORKDIR

echo unverified > ${SW_BLDDIR}/status
chmod g+w ${SW_BLDDIR}/status


##echo "Testing"; which grompp

mpirun -np 1 ${SW_BLDDIR}/bin/gmx_mpi grompp -f grompp.mdp -r conf.g96 -p topol.top
if [ -f topol.tpr ]; then
  echo "Testing:"; 
  mpirun -np 16 ${SW_BLDDIR}/bin/gmx_mpi mdrun -s topol.tpr
fi

if [ -f md.log ]; then
  testpassed=\`grep "Finished mdrun" md.log | wc -l\`
  
  if [[ \$testpassed -ne 1 ]]; then
    echo test failed!
    echo unverified > ${SW_BLDDIR}/status
    EXITCODE=1
  else
    echo test passed!
    echo verified > ${SW_BLDDIR}/status
    EXITCODE=0
  fi
  
fi 

JOBID=\`echo \$PBS_JOBID | cut -d "." -f1 \`
chmod 775 ${SW_BLDDIR}/status

rm -f ${SW_BLDDIR}/.running
cat gromacs_test.o\$JOBID >> ${SW_BLDDIR}/test.log
chmod 664 ${SW_BLDDIR}/test.log

EOF

#submit job and touch .running file - marker to infrastructure that
qsub ${PACKAGE}.pbs > ${SW_BLDDIR}/.running
if [ $? -ne 0 ] ; then
  echo "Error submitting job"
  rm -f .running
  exit 1
else
  echo "Job submitted"
  cat ${SW_BLDDIR}/.running
  exit 2
fi

cd ../

############################### if this far, return 0
exit 0
