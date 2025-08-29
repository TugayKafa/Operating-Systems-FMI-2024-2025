#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "You must provide 2 arguments!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "First argument must be directory!"
    exit 2
fi

isCorrect() {
   [[ ${1} =~ ^.+-[1-9]+[0-9]*\.[1-9]+[0-9]*\.[1-9]+[0-9]*-${2}$ ]]
}


while IFS=  read -r name; do
   filename=$(basename "$name")
   if isCorrect "$filename" "${2}" ; then
        echo $filename
    fi
done< <(find ${1} -maxdepth 2 -mindepth 1 -type f)
