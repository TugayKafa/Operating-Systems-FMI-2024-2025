#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "You must have exactly 2 arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "First argument must be file!"
    exit 2
fi

if [[ ! -d ${2} ]]; then
    echo "Second argument must be directory"
fi

files=$(find "${2}" -type f | wc -l)

if [[ $files -ne 0 ]]; then
    echo "Directory must be empty!"
    exit 3
fi

output=$(echo "${2}/dict.txt")
touch "$output"

res=$(mktemp)
while read -r row; do
    row=$(echo "$row" | sed 's/  */ /g')
    name=$(echo "$row" | cut -d ':' -f1 | cut -d ' ' -f1-2)
    echo "$name" >> "$res"
done< <(cat "${1}")

id=1
while read -r row; do
    echo "$row;$id" >> $output
    (( id++ ))
done< <(cat "$res" | sort | uniq | sed 's/  */ /g')

rm "$res"
