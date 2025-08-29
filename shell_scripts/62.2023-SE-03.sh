#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You need to provide 1 argument!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Provided argument must be directory!"
    exit 2
fi

candidates=$(mktemp)

while read -r filename; do
    
    while read -r word; do
       
        if [[ ! "$word" =~ [a-z]+ ]]; then
            continue
        fi
    
        echo "$word,$filename" >> "$candidates"

    done< <(cat "$filename" | tr ' ' '\n')

done< <(find "${1}" -type f -printf "%p\n")

files_count=$(find "${1}" -type f -printf "%p\n" | wc -l)
stop_count=$(( files_count / 2 ))

uniq_words=$(cat "$candidates" | cut -d ',' -f1 | sort | uniq)
uniq_words_frequency=$(cat "$candidates" | sort | uniq -c | sort -nr | sed -E 's/  +//g' | grep -E "^[3-9]|[1-9]+[0-9]+ ")

stop_words=$(mktemp)

while read -r word; do

    files_count=$(cat "$candidates" | sort | uniq | grep -c "^$word,")
    if [[ "$files_count" -ge "$stop_count" ]]; then
        grep "$word" "$uniq_words_frequency" >> "$stop_words"
    fi

done <<< "$uniq_words"

cat "$stop_words" | sort -nr | head -n 10

rm "$candidates"
rm "$stop_words"
