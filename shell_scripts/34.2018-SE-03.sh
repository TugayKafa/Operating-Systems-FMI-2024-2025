#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "You must have exactly 2 arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "Your arguments must be files!"
    exit 2
fi

if [[ ! -f ${2} ]]; then
    touch "${2}"
fi

uniqLines=$(cat "${1}" | cut -d ',' -f2- | sort | uniq | sed 's/  */ /g')

while read -r line; do
    id=$(cat "${1}" | sort -n | grep -F ",${line}" | head -n 1 | cut -d ',' -f1 )
    echo "$id,$line" >> "${2}"
done< <(echo "$uniqLines")
