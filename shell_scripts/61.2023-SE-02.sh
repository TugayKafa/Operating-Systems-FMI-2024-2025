#!/bin/bash

if [[ "${#}" -le 1 ]]; then
    echo "You should provide at least 2 arguments!"
    echo "First argument must be positive number(seconds) and other arguruments are command and arguments for the command!"
    exit 1
fi

if [[ ! "${1}" =~ ^[1-9]|[1-9]+[0-9]* ]]; then
    echo "First argument must be positive number!"
    exit 2
fi

seconds=${1}
shift 1

cmd="$*"
count=0

start=$(date +'%s')

while true; do
    bash -c "$cmd" >/dev/null
    curr=$(date +'%s')

    diff=$(( curr - start ))
    (( count += 1 ))

    if (( diff >= seconds  )); then
        break;
    fi
done

echo "Ran the command '${cmd}' ${count} for $(echo "${diff}" | bc -l) seconds."
echo "Average runtime: $(echo "${diff} / ${count}" | bc -l) seconds."
