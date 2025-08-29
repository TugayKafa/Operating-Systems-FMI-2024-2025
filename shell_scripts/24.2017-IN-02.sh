#!/bin/bash

if [[ "$(id -u)" -ne 0 ]]; then
  echo "This script must be run as root"
  exit 4
fi


if [[ ${#} -ne 1 ]]; then
    echo "Wrong arguments"
    exit 1
fi

foo_processes=$(ps -eo user | grep -c "^${1}$")

while read -r row; do
    count=$(ps -eo user | grep -c "^$row$")
    if [[ $count -gt $foo_processes ]]; then
        echo "$row"
    fi
done< <(ps -eo user | sort | uniq)


total_time=0
process_count=0

while read -r time_str; do 
    IFS=: read -r h m s <<< "$(printf "%02d:%02d:%02d" $(echo "$time_str" | awk -F: '{if(NF==3){print $1, $2, $3} else if(NF==2){print 0, $1, $2} else {print 0, 0, $1}}'))"
    seconds=$((10#$h * 3600 + 10#$m * 60 + 10#$s))
    total_time=$((total_time + seconds))
    process_count=$((process_count + 1))
done< <(ps -eo time --no-headers)

if [[ $process_count -eq 0 ]]; then
    echo "No processes found"
    exit 3
fi

average=$(( $total_time/$process_count ))
echo "Average process time: $average seconds"

echo "Killing processes of ${1} running more than $(( 2 * $average )) seconds..."

while read -r pid user time_str; do
    IFS=: read -r h m s <<< "$(printf "%02d:%02d:%02d" $(echo "$time_str" | awk -F: '{if(NF==3){print $1, $2, $3} else if(NF==2){print 0, $1, $2} else {print 0, 0, $1}}'))"
    seconds=$((10#$h * 3600 + 10#$m * 60 + 10#$s))

    if [[ "$user" == "${1}" && $seconds -gt $((2 * average)) ]]; then
        echo "Killing PID $pid (ran $seconds sec)"
        kill "$pid"
    fi
done< <(ps -eo pid,user,time --no-headers)
