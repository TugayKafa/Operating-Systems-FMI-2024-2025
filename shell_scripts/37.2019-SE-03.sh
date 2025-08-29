#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Not a directory!"
    exit 2
fi

if [[ ! -e "${1}/hash.db" ]]; then
    touch ${1}/hash.db
fi

if [[ ! -d "${1}/extracted" ]]; then
    mkdir ${1}/extracted
fi
tmpHash=$(mktemp)

hashDB="${1}/hash.db"
files=$(find "${1}" -type f -name "*.tgz")
while read -r line; do
    name=$(basename "$line" | cut -d '-' -f1 | sed -E s:"_[^_]*$"::g)
    if [[ $name =~ _ ]]; then
        echo "CONTINUE:: $name"
        continue
    fi
    timestamp=$(echo "$line" | cut '-' -f2 | sed s:".tgz"::g)
    hash=$(sha256sum "$line" | cut -d ' ' -f1)
    echo "Name: $name   Timestamp: $timestamp"
    echo "File: $line"
    echo "Hash: $hash"
    if grep -q -E "$hash" $hashDB; then
        continue
    fi

    if tar -tzf $line | grep -q -E "meow.txt"; then
        path=$(tar -tzf $line | grep -E "meow.txt")
        tar -xOf "$line" "$path" > ${1}/extracted/"$name"_"$timestamp".txt
    fi

    echo "$hash" >> "$tmpHash"
done< <echo "$files"

cat "$tmpHash" | uniq > "$hashDB"
rm $tmpHash
