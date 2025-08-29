#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "Wrong arguments"
    exit 1
fi

src=${1}
dest=${2}

if [[ ! -d $src || ! -d $dest ]]; then
    echo "Arguments must be directories!"
    exit 2
fi

count=$(find "$dest" -mindepth 1 | wc -l)
if [[ "$count" -ne 0 ]]; then
    echo "Second directory must be empty"
    exit 3
fi

mkdir -p 
files=$(find "$src" -mindepth 1 -type f)
while IFS= read -r filename; do
    if [[ $filename =~ ^\..*\.swp$ ]]; then
        continue
    fi

    name=$(basename "$filename")
    cp "$filename" "$dest/$name"
done
