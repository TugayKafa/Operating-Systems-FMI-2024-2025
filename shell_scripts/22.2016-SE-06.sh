#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -f $1 ]]; then
    echo "You must provide file"
    exit 2
fi

result=""
rowNumber=1

while read -r row; do
    formated="${rowNumber} ${row}"
    result="${result} ${formated}\n"
    rowNumber=$(( rowNumber + 1 ))
done< <(cat ${1} | cut -d '-' -f2-3)

printf "%b" "$result" | sort -k2 > result

