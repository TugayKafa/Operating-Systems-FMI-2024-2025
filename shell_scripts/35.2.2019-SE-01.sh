#!/bin/bash

numbers=()
while read -r line; do
    if [[ "$line" =~ ^-?[1-9][0-9]*$ ]]; then
        numbers+=("$line")
    fi
done

if [[ ${#numbers[@]} -eq 0 ]]; then
    exit 0
fi

min=""
sum=0

for n in "${numbers[@]}"; do
    n_sum=0
    num=${n#-}

    while IFS= read -r -n 1 digit; do
        [[ -z "$digit" ]] && break
        (( n_sum += digit ))
    done < <(echo -n "$num")

    if [[ "$n_sum" -gt "$sum" || -z "$min" ]]; then
        min="$n"
        sum="$n_sum"
    elif [[ "$n_sum" -eq "$sum" && "$n" -lt "$min" ]]; then
        min="$n"
    fi
done

echo "$min"
