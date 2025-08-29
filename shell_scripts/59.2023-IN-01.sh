#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You must provide 1 argument!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Provided argument must be directory"
fi

sha_sums=$(mktemp)
to_remove=$(mktemp)

find "${1}" -type f 2>/dev/null | xargs -I{} sha256sum {} > "$sha_sums"

while read -r sha; do
    files_to_delete=$(mktemp)
    hardlinks_to_delete=$(mktemp)
    current_files=$(mktemp)

    grep -E "^$sha" "$sha_sums" | cut -d ' ' -f3 | xargs -I{} stat -c "%n %i" {} > "$current_files"

    while read -r cnt inode; do
        
        if [[ "$cnt -eq 1" ]]; then
            grep -E "${inode}$" "$current_files" | cut -d ' ' -f1 >> "$files_to_delete"
        else
            grep -E "${inode}$" "$current_files" | cut -d ' ' -f1 | head -n 1 >> "$hardlinks_to_delete" 
        fi

    done < <(cut -d ' ' -f2 "$current_files" | sort | uniq -c | head -n 1 | tr -s ' ' | sed -E "s/^[ ]*//g")

    if [[ $(cat "$files_to_delete" | wc -c) -eq 0 ]]; then
        cat "$hardlinks_to_delete" >> "$to_remove"
    elif [[ $(cat $hardlinks_to_delete | wc -c) -eq 0 ]]; then
        cat "$files_to_delete" | tail -n +2 >> "$to_remove"
    else
        cat "$hardlinks_to_delete" >> "$to_remove"
        cat "$files_to_delete" >> "$to_remove"
    fi

    rm "$files_to_delete"
    rm "$hardlinks_to_delete"
    rm "$current_files"

done < <(cut -d ' ' -f1 "$sha_sums" | sort | uniq)

cat "$to_remove"

rm "$sha_sums"
rm "$to_remove"
