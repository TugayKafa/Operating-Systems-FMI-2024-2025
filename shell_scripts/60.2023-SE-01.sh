#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo 'You need to provide 2 arguments!'
    exit 1
fi

if [[ ! -f "${1}" ]]; then
    echo 'First argument must be a file!'
    exit 2
fi

if [[ ! -d "${2}" ]]; then
    echo 'Second argument must be a directory!'
    exit 3
fi

while read forbidden; do
    censore=$(echo "${forbidden}" | sed 's/./*/g')

    while read file; do
        sed -i -E "s/\b${forbidden}\b/${censore}/Ig" "${file}"
    done < <(find "${2}" -type f -name "*.txt")

done < <(cat "${1}")
