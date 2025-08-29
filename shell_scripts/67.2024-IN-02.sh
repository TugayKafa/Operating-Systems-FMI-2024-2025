#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "You need to provide 2 arguments!"
    exit
fi

if [[ ! -d "${1}" ]]; then
    echo "First argument must be directory!"
    exit
fi

tmp_file=$(mktemp)

while read -r filename; do
    first_line=$(cat "$filename" | head -n 1)
    class_name=$(echo "$first_line" | cut -d ' ' -f2)
    echo "$class_name" >> "$tmp_file"
    parent_section=$(echo "$first_line" | cut -d ':' -f2)
    if [[ -z "$parent_section" ]]; then
        continue
    fi

    count_of_parents=$(echo "$parent_section" | grep -c ",")
    (( count_of_parents +=1 ))

    for num in $(seq 1 "$count_of_parents"); do
        parent_name=$(echo "$parent_section" | cut -d ',' -f ${num} | cut -d ' ' -f2)
        echo "${parent_name} -> ${class_name}" >> "$tmp_file"
    done
done < <(find "${1}" -type f -name "*.h" -printf "%p")

cat "$tmp_file" > ${2}

rm "$tmp_file"
