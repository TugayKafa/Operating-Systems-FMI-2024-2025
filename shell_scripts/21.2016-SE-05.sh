#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "You must provide 2 files as arguments!"
    exit 1
fi

if [[ ! -f ${1} || ! -f ${2} ]]; then
    echo "Provided arguments must be files!"
    exit 2
fi

count_first=0
count_second=0

firstFile=$(mktemp)
secondFile=$(mktemp)

while read -r line; do
    singer=$(echo $line | cut -d " " -f2)
    if [[ $singer == ${1} ]]; then 
        ((count_first += 1))
        echo $line | cut -d " " -f4- >> $firstFile
    fi
    echo $line
done< <(cat ${1})

while read -r line; do
    singer=$(echo $line | cut -d " " -f2)
    if [[ $singer == ${2} ]]; then 
        ((count_second += 1))
        echo $line | cut -d " " -f4- >> $secondFile
    fi
    echo $line
done< <(cat ${2})

if [[ $count_first -ge $count_second ]]; then
    cat $firstFile | sort >> ${1}.songs
else
    cat $secondFile | sort >> ${2}.songs
fi
