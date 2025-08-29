#!/bin/bash

if [[ "${#}" -ne 2 ]]; then
    echo "You need to provide 2 arguments!"
    exit 1
fi

if [[ ! -d "${1}" ]]; then
    echo "First argument must be directory!"
    exit 2
fi

if [[ -d "${2}" ]]; then
    echo "Second argument must be directory name that will be created!"
    exit 3
fi

dates=$(mktemp)

while read -r filename; do
    date=$(stat -c "%y" "$filename" | cut -d ' ' -f1)
    echo "$date $filename" >> "$dates"
done < <(find "${1}" -type f -name "*.jpg" -printf "%p\n")

start_date=
end_date=
tmp_file=$(mktemp)

while read -r fdate filename; do
    if [[ -z "$start_date" && -z "$end_date" ]]; then
        start_date="$fdate"
        end_date="$end_date"
        echo "$filename" >> "$tmp_file"
    fi
    
    tmp_date=$(date -d '"$end_date" + 1 day' + '%Y-%m-$d')
    if [[ "$tmp_date" == "$fdate" ]]; then
        end_date="$fdate"
        echo "$filename" >> "$tmp_file"
    else
        dir="${2}/${start_date}_${end_date}"
        mkdir -p "${2}/${start_date}_${end_date}"
        while read -r file; do
            simple=$(echo "$file" | rev | cut -d '/' f1 | rev)
            mv "$file" "${dir}/${simple}"
        done < "$tmp_file"
        start_date="$fdate"
        end_date="$fdate"
        echo "$filename" > "$tmp_file"
    fi

done < <(cat "$dates" | sort -n)

dir="${2}/${start_date}_${end_date}"
mkdir -p "${2}/${start_date}_${end_date}"
while read -r file; do
    simple=$(echo "$file" | rev | cut -d '/' f1 | rev)
    mv "$file" "${dir}/${simple}"
done < "$tmp_file"

rm "$tmp_file"
rm "$dates"

