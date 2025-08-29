#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You need to provide 1 argument!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Provided argument must be a directory!"
    exit 2
fi

shasums=$(mktemp)

bytes=0

while read -r filename; do
    sum=$(sha256sum "$filename" | cut -d ' ' -f1)
    if ! grep -q -E "^$sum " "$shasums"; then
        echo "$sum 0" >> "$shasums"
    else
        name_and_exist=$(grep -E "^$sum " "$shasums" | cut -d ' ' -f2-)
        name=$(echo "$name_and_exist" | cut -d ' ' -f1)
        sed -iE "s/ $name_and_exist$/ $name 1/"
        file_size=$(stat -c%s "$filename")
        (( bytes += file_size ))
        rm "$filename"
        ln "$name" "$filename"
    fi

done < <(find "${1}" -type f -printf "%p\n")

groups=(cat shasums | cut -d ' ' -f3 | grep -c '1')
echo "Deduplicated $groups groups of duplicated files!"
echo "Released $bytes bytes!"

rm "$shasums"
