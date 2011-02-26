#!/bin/bash
# Make all the libraries in src.
# Written by Laurence McGlashan lrm29@cam.ac.uk

usage() 
{
    while [ "$#" -ge 1 ]; do echo "$1"; shift; done
    cat<<USAGE
usage: ${0##*/} [OPTION]
  
options:
  -cleanall       Clean all
  -cleansource    Clean source code
  -debug          Compile in debug mode
  -profile        Compile in profile mode
  -help           This usage

USAGE
    exit 1
}

debug=0
profile=0
cleanall=0
cleansource=0

# parse options
while [ "$#" -gt 0 ]
do
   case "$1" in
   -h | -help)
      usage
      ;;
   -debug)
      debug=1
      shift 1   
      ;;
   -profile)
      profile=1
      shift 1   
      ;;
   -cleanall)
      cleanall=1
      shift 1
      ;;
   -cleansource)
      cleansource=1
      shift 1
      ;;
   --)
      shift
      break
      ;;
   -*)
      usage "invalid option '$1'"
      ;;
   *)
      break
      ;;
   esac
done

echo $0

if [ $0 != "./etc/makeAllLibs.sh" ]; then 
    echo "Error: Must run this script from mops-c-Git/"
    exit 1
fi

odesolvers=(cvode cvodes radau kinsol ida)
io=(camxml comostrings)
folders=(sprogc geometry camflow sweepc mopsc brush)

if [ $cleanall -eq 1 ]; then
    echo "Cleaning all libraries."
    for element in ${odesolvers[@]}
    do
        make --directory=src/odesolvers/$element -s --file="Makefile" cleanlibs
        echo "$element cleaned."
    done
    for element in ${io[@]}
    do
        make --directory=src/io/$element -s --file="Makefile" cleanlibs
        echo "$element cleaned."
    done
    for element in ${folders[@]}
    do
        make --directory=src/$element -s --file="Makefile" cleanlibs
        echo "$element cleaned."
    done
    echo "Libraries cleaned successfully."
    exit
fi

if [ $cleansource -eq 1 ]; then
    echo "Cleaning all library source."
    for element in ${odesolvers[@]}
    do
        make --directory=src/odesolvers/$element -s --file="Makefile" clean
        echo "$element cleaned."
    done
    for element in ${io[@]}
    do
        make --directory=src/io/$element -s --file="Makefile" clean
        echo "$element cleaned."
    done
    for element in ${folders[@]}
    do
        make --directory=src/$element -s --file="Makefile" clean
        echo "$element cleaned."
    done
    echo "Librarie source cleaned successfully."
    exit
fi

echo "Make everything with debug set to $debug and profile set to $profile."

echo "Make ODE solver libraries:"
for element in ${odesolvers[@]}
do
    make --directory=src/odesolvers/$element -s --file="Makefile" clean all debug=$debug profile=$profile
    if [ $? -gt 0 ] ; then
        echo make $element failed
        exit
    else
        echo make $element success
    fi
done

echo "Make IO libraries:"
for element in ${io[@]}
do
    make --directory=src/io/$element -s --file="Makefile" clean all debug=$debug profile=$profile
    if [ $? -gt 0 ] ; then
        echo make $element failed
        exit
    else
        echo make $element success
    fi
done

echo "Make the rest:"
for element in ${folders[@]}
do
    make --directory=src/$element -s --file="Makefile" clean all debug=$debug profile=$profile
    if [ $? -gt 0 ] ; then
        echo make $element failed
        exit
    else
        echo make $element success
    fi
done

echo "makeAllLibs successful."
