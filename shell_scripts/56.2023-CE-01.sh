#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "You need to provide 2 arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "First argument must be csv file!"
fi

name_of_constellation=$(cat "${1}" | cut -d ',' -f4,5 | sort | uniq -c | sort -nr | sed -E "s/  +//g" | grep ",${2}" | cut -d ',' -f1 | cut -d ' ' -f2 | head -n 1)

cat "${1}" | cut -d ',' -f1,4,7 | grep ",${name_of_constellation}," | sort -t ',' -k 3nr | cut -d ',' -f1 | head -n 1
