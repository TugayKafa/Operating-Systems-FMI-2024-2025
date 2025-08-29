#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "First argument must be config file!"
    exit 2
fi

user=$(whoami)
date=$(date)
message=$(echo "${2} = ${3} # added at $date by $user")

res=$(mktemp)

found=0
while read line; do
    if echo "${line}" | sed 's/ //g' | sed 's/#.*//g' | grep -E -q "^${2}"; then
        echo "${line} # edited at $date by $user" >> "${res}"
        echo "$message" >> "${res}"
        found=1
    else
        echo "${line}" >> "${res}"
    fi
done < <(cat "${1}")

if [[ "${found}" -eq 0 ]]; then
    echo "$message" >> "${res}"
fi

cat "${res}" > "${1}"
rm "${res}"
