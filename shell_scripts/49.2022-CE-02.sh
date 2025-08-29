#!/bin/bash

if [[ "${#}" -ne 1 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! ${1} =~ ^[A-Z0-9]{1,4}$ ]]; then
    echo "Wrong device name!"
    exit 1
fi
 
file="/proc/acpi/wakeup"
data=$(cat "$file" | tail -n +2 | sed "s/\t/ /g" | tr - s ' ' | sed 's/^ //; s/ $//')

if echo "$data" | cut -d ' ' -f1 | ! grep -Eq "${1}"; then
    echo "The process does not exist in the table!"
    exit 0
fi

state=$(echo "$data" | cut -d ' ' -f3 | grep -E "${1}")
if [[ state == "*enabled" ]]; then
    echo "${1}" > "${file}"
    echo "${1} disabled successfully!"
else
    echo "${1} is already disabled! Do not wory!"
fi

