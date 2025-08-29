#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "Wrong arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "Provided argument must be file"
    exit 2
fi

http=$(mktemp)
non_http=$(mktemp)
wrongReq=$(mktemp)
uniqHost=$(mktemp)

while read -r line; do
    client=$(echo "$line" | cut -d ' ' -f1)
    hostname=$(echo "$line" | cut -d ' ' -f2)
    reqType=$(echo "$line" | cut -d ']' -f2 | cut -d ' ' -f4)
    reqCode=$(echo "$line" | cut -d ']' -f2 | cut -d ' ' -f5)
    
    if [[ "$reqType" =~ "HTTP/2.0" ]]; then
        echo "$hostname" >> "$http"    
    else
        echo "$hostname" >> "$non_http"
    fi

    if (( reqCode > 302 )); then
        echo "$client" >> "$wrongReq"
    fi

    if ! grep -q "$hostname" "$uniqHost"; then
        echo "$hostname" >> "$uniqHost"
    fi
done< <(cat "${1}")

while read -r host; do
    correct=$(grep "$host" "$http" | wc -l)
    not_correct=$(grep "$host" "$non_http" | wc -l)
    echo "$host HTTP/2.0: $correct non-HTTP/2.0: $not_correct"
done< <(cat "$uniqHost")

cat "$wrongReq" | sort | uniq -c | head -n 5 | sed "s/  */ /g"

rm "$http"
rm "$non_http"
rm "$wrongReq"
rm "$uniqHost"
