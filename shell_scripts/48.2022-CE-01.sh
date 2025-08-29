#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

#if [[ ${1} =~ ^-?[0-9]+(\.[0-9]+)?$ ]]; then
 #   echo "First argument must be a numbers!"
  #  exit 2
#fi

if [[ ${#2} -ne 1 || ${#3} -ne 1 ]]; then
    echo "2nd and 3rd argument must be symbols!"
    exit 3
fi

num=${1}
prefix=${2}
unit=${3}

prefixFile="48.data/prefix.csv"
unitFile="48.data/base.csv"

if ! grep -Eq ",$prefix," "$prefixFile"; then
    echo "Prefix does not exist!"
    exit 4
fi

if ! grep -q ",$unit," "$unitFile"; then
    echo "Unit does not exist!"
    exit 5
fi

decimal=$(grep ",$prefix," "$prefixFile" | cut -d ',' -f3)
num=$(echo "$num * $decimal" | bc )
unit_name=$(grep ",$unit," "$unitFile" | cut -d ',' -f1)
unit_measure=$(grep ",$unit," "$unitFile" | cut -d ',' -f3)

echo "$num $unit ($unit_measure $unit_name)"
