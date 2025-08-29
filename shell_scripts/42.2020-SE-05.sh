#!/bin/bash

if [[ ${#} -ne 3 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -f ${1} || ! -d ${3} ]]; then
    echo "Wrong type of arguments"
    exit 2
fi

fileNames=$(find "${3}" -type f -name "*.cfg")

username=$(echo "${2}" | cut -d '.' -f1)
if ! grep -q "$username" "${1}"; then
    password=$(pwgen 16 1)
    echo "$username $password"
    echo "$username:$password" >> ${1}
fi

while IFS= read -r fileName; do
    lineN=1
    isOk=1
    output=$(mktemp)
    name=$(basename "$fileName")
    echo "Error in $name" >> "$output"
    while read -r row; do
        if [[ ! "$row" =~ ^\{.*\}\;$ ]]; then
            if [[ "$row" =~ ^#.*$ ]]; then
                continue
            fi
          isOk=0
          echo "Line $lineN:$row" >> $output
        fi
        (( lineN+=1 ))
    done< <(cat "$fileName")
    
    if [[ "$isOk" -eq 0 ]]; then
        cat "$output"
    else 
        cat "$fileName" >> ${2}
    fi

    rm "$output"
done< <(echo "$fileNames")
