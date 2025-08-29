#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "2 args expected"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "dir expected as first argument!"
    exit 2
fi

if [[ ! "${2}" =~ ^([1-9]|[1-9][0-9])$ ]]; then
    echo "number expected as second arg!"
    exit 3
fi

while read -r candidate; do
    if [[ $(df "${1}" | tail -n +2 | tr -s ' ' | cut -d ' ' -f5 | tr -d '%') -le "${2}" ]]; then
        break
    fi
    
    backups=$(echo "$candidate" | cut -d '/' -f2)
    dir=$(echo "$candidate" | cut -d '/' -f1)
    file=$(basename $(echo "$candidate" | rev | cut -d '/' -f2- | rev))

    hostname=$(echo "$file" | cut -d '-' -f1)
    area=$(echo "$file" | cut -d '-' -f2)

    lower_bound=-1
    if [[ "$backups" -eq 0 ]]; then
        lower_bound=1
    elif [[ "$backups" -eq 1 ]]; then
        lower_bound=2
    elif [[ "$backups" -eq 2 ]]; then
        lower_bound=3
    elif [[ "$backups" -eq 3 ]]; then
        lower_bound=4
    else
        continue
    fi

    if [[ $(wc -l $(find "${dir}/${backups}" -type f -regex "${dir}/${backups}/${hostname}-${area}-[0-9]{8}.tar.xz")) -le "$lower_bound" ]]; then
        continue
    fi

    rm "$(echo "$candidate" | rev | cut -d '/' -f2- | rev)"

done< <(find "${1}" -type f -printf "%p/$s\n" 2>/dev/null | sort -t '/' -k 4nr -k 2n | sort -s -t '-' -k 3)

if [[ $(df "${1}" | tail -n +2 | tr -s ' ' | cut -d ' ' -f5 | tr -d '%') -gt ${2} ]]; then
    echo "not possible"
    exit 4
fi
