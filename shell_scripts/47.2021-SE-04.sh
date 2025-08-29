#!/bin/bash

user=$(whoami)
if [[ "$user" != "oracle" && "$user" != "grid" ]]; then
    echo "Only oracle and grid have access to this script!"
    exit 1
fi

if [[ ${#} -ne 1 ]]; then
    echo "You must provide time"
    exit 2
fi

if [[ ! ${1} =~ ^[1-9][0-9]*$ ]]; then
    echo "Provided argument must be integer!"
    exit 3
fi

num=${1}
if [[ $num -lt 120 ]]; then
    num=120
fi

diag_dest="/u01/app/${user}"

cmd="${ORACLE_HOME}/bin/adrci"

while read -r homeDir; do
    inter=$(echo $homeDir | cut -d '/' -f 2)
    if [[ ! "$inter" =~ ^(crs|tnslsnr|kfod|asm|rdbms)$ ]]; then
        continue
    fi
    $cmd exec="SET BASE ${diag_dest}; SET HOMEPATH ${homeDir}; PURGE -AGE ${num}"

done< <($cmd exec="SET BASE ${diag_dest}; SHOW HOMES;" | tail -n +2)
