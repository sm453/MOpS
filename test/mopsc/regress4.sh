#!/bin/bash

#Choose the windows or linux names for the executable
uname -s | grep --ignore-case CYGWIN 
if(($?==0))
then
    windows=1
else
    windows=0
fi

if((windows==1))
then
	program="../../bin/debug/mops.exe"
else 
	program="../../bin/debug/mops" 
fi

echo "Weighted particle simulation of constant coagulation kernel"
$program -flamepp -p -gp ./regress4/regress4.inp -rr ./regress4/regress4.inx -s ./regress4/regress4.xml -c ./regress4/chem.inp -t ./regress4/therm.dat
echo "Finished simulation"
echo "========================"

# Array of particle numbers - these should be the count of particles of sizes 1-5 in the psl file
# Put a negative values at the start so that the number of particles of size 1 comes at index 1
# These numbers are for a seed of 123 in the Mersenne Twister random number generator
# Analytic solution is 3344 2866 1842 1053 564
testValues=(-1 3349 2853 1839 1061 564)

# Grep seems to require the file in unix format, even under cygwin
if((windows==1))
then
    dos2unix "regression4a-psl(3s).csv"
fi

i=1
while ((i <= 5))
do
  count=`grep ",$i$" "regression4a-psl(3s).csv" | wc -l`
  #echo "$i $count"
  if((count != testValues[i])) 
    then
      # Regression test has failed; print explanatory message and exit with non zero
      # value showing the size class of the first difference
      echo "Found $count particles of size $i, when ${testValues[i]} expected"
      echo "**************************"
      echo "****** TEST FAILURE ******"
      echo "**************************"
      exit $i
  fi
  ((i+=1))
done

if((i==6))
then
  # All tests passed
  echo "All tests passed"
  rm regression4a*
  exit 0
else
  exit 1
fi
