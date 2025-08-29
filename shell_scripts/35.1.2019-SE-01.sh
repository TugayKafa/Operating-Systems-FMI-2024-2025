#!/bin/bash

numbers=()

while read line; do
    if [[ "$line" =~ ^-?[1-9]+[0-9]*$ ]]; then
        numbers+=("$line")
    fi
done

if [[ ${#numbers[@]} -eq 0 ]]; then
    exit 0
fi

max=0
for n in "${numbers[@]}"; do
    abs_n=${n#-}
    if (( max < abs_n )); then
        max="$abs_n"
    fi
done

printed=""

for n in "${numbers[@]}"; do
    abs_n=${n#-}
    if (( max == abs_n )); then
       if [[ ! "$printed" =~ (^|[[:space:]])"$n"($|[[:space:]]) ]]; then
            echo "$n"
            printed+="$n "
        fi
    fi
done
