#!/bin/bash

if [[ ${#} -lt 1 || ${#} -gt 2 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "First argument must be directory!"
    exit 2
fi

isNumber() {
     [[ $1 =~ ^[1-9]+[0-9]*$ ]]
}


isExist=0
if [[ ${#} -eq 2 ]]; then
    if isNumber "${2}"; then
        isExist=1
    else
        echo "Second argument must be positive number!"
        exit 3
    fi
fi

if [[ $isExist -eq 1 ]]; then
    find ${1} -links +$(( ${2} - 1 ))
else
    find -L ${1} -type l
fi


