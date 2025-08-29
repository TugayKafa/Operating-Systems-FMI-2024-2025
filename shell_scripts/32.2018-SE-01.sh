#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You must have exactly 1 parameter!"
    exit 1
fi

if [[ ! -d  ${1} ]]; then
    echo "Argument must be directory!"
    exit 2
fi

paths=$(mktemp)

find "${1}" -mindepth 4 -maxdepth 5 -type f -exec echo {} \; >> "$paths"

friends=$(mktemp)

while read -r path; do
   filename=$(echo "$path" | cut -d '/' -f5)
    friend=$(echo "$path" | cut -d '/' -f4)
    if [[ $filename =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}\.txt$ ]]; then
       echo "$friend" >> "$friends"
    fi
done < $paths

cat "$friends" | sort | uniq -c | sort -nr | sed 's/ */ /g' | head

rm "$paths"
rm "$friends"
