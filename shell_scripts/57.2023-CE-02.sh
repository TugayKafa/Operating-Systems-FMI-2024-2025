#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "You need to provide 3 arguments!"
    exit 1
fi

if [[ ! -f ${1} || ! -f ${2} ]]; then
    echo "First two arguments must be files!"
    exit 2
fi

from_first_point=$(cat "${1}" | grep -E '^"$f{3}":' | sort -t ':' -k 2nr)
from_second_point=$(cat "${2}" | grep -E "^"${3}":" | sort -t ':' -k 2nr)

if [[ -z "$from_first_point" && -z "$from_second_point" ]]; then
    echo 'Black hole: "${3} not found!'
elif [[ -z "$from_first_point" ]]; then
    echo 'Black hole is closser to second point - "$from_second_point"!'
elif [[ -z "$from_second_point" ]]; then
    echo 'Black hole is closser to first point - "$from_first_point"!'
else
    first_point_dist=$(echo "$from_first_point" | cut -d ':' -f2 | sed -E "s/  +//g" | cut -d ' ' -f1)
    second_point_dist=$(echo "$from_first_point" | cut -d ':' -f2 | sed -E "s/  +//g" | cut -d ' ' -f1)

    if [[ "$first_point_dist" -ge "$second_point_dist" ]]; then
        echo 'Black hole is closser to first point - "$from_first_point"!'    
    else
        echo 'Black hole is closser to second point - "$from_second_point"!'
    fi
fi
