#!/bin/bash

if [[ ${#} -eq 0 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

sequence=""
n=10
if [[ ${1} =~ "-n" ]]; then
    if [[ ${#} -lt 3 ]]; then
        echo "Wrong arguments!"
        exit 1
    fi

    if [[ ! ${2} =~ ^[1-9]+[0-9]*$ ]]; then
        echo "Second argument must be positive integer!"
        exit 2
    fi
    
    sequence=$(seq 3 "${#}")
    n="${2}"
else
    sequence=$(seq 1 "${#}")
fi

result=$(mktemp)

for i in $sequence; do
    cat "${!i}" | tail -n "$n" >> "$result"
done

cat "$result" | sort

rm "$result"
