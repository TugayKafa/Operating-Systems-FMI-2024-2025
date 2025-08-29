#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "First argument must be directory!"
    exit 2
fi

if [[ ! -d ${2} ]]; then
    mkdir "${2}"
else
    echo "Second argument must be name for directory wich gonna be create!"
    exit 3
fi

fileNames=$(mktemp) 
find "${1}" -type f -name "*.jpg" -printf "%f\n" > "$fileNames"

mkdir "${2}/images"

while read -r fileName; do
    name=$(basename $fileName)
    title=$(echo "$name" | cut -d '.' -f1 | sed -E "s/\(.*\)//g" | sed "s/  */ /g")
    album=$(echo "$name" | cut -d '.' -f1 | rev | cut -d '(' -f1 | cut -d ')' -f2 | rev)
    date=$(stat -c %y "$fileName" | cut -d ' ' -f1)
    hash=$(sha256sum "$fileName" | cut -c 1-16)
    
    if [[ -z "$album" ]]; then
        album=$(echo "misc")
    fi

    cp "$filename" "${2}/images/$hash.jpg"
    
    mkdir -p "${2}/by-date/$date/by-album/$album/by-title"
    ln -s "${2}/images/$hash.jpg" "${2}/by-date/$date/by-album/$album/by-title/$title.jpg"
    
    mkdir -p "${2}/by-date/$date/by-title"
    ln -s "${2}/images/$hash.jpg" "${2}/by-date/$date/by-title/$title.jpg"
    
    mkdir -p "${2}/by-album/$album/by-date/$date/by-title"
    ln -s "${2}/images/$hash.jpg" "${2}/by-album/$album/by-date/$date/by-title/$title.jpg"
    
    mkdir -p "${2}/by-album/$album/by-title"
    ln -s "${2}/images/$hash.jpg" "${2}/by-album/$album/by-title/$title.jpg"
    
    mkdir -p "${2}/by-title"
    ln -s "${2}/images/$hash.jpg" "${2}/by-title/$title.jpg"
done< <(cat "$fileNames")

rm "$fileNames"
