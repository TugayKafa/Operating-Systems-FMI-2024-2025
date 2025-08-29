#!/bin/bash

if [[ ${#} -gt 2 || ${#} -eq 0 ]]; then
    echo "Wrong arguments"
    exit 1
fi

count=0

while IFS= read -r row; do
    path=$(readlink $row)
    output=''
    if [[ -e $path ]]; then
        output="$row -> $path"
    else 
       ((count++)) 
       continue
    fi

    if [[ ${#} -eq 2 ]]; then
        echo $output >> ${2}
    else
        echo $output
    fi

done< <(find ${1} -type l)

if [[ ${#} -eq 2 ]]; then
    echo "Broken symbolic links: $count" >> ${2}
else
    echo "Broken symbolic links: $count"
fi
