#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You must provide 1 parrameter"
    exit 1
fi

if [[ ! ${1} =~ [1-9]+[0-9]* ]]; then 
    echo "Argument must be positive number!"
    exit 2
fi

usr="0"
biggest_pid=-1
rss_sum=0
max_rss=0

while read -r user pid rss; do
    if [[ $usr == $user ]]; then
        ((rss_sum += rss))
        if [[ $max_rss -lt $rss ]]; then
            max_rss=$rss
            biggest_pid=$pid
        fi
    else 
        if [[ $usr != "0" ]]; then
            echo "User: $usr RSS: $rss_sum"
            if [[ $rss_sum -ge ${1} ]]; then
                echo "Killing proccess with biggest rss. PID: $biggest_pid RSS: $max_rss"
            fi
        fi
        usr=$user
        biggest_pid=$pid
        rss_sum=$rss
        max_rss=$rss
    fi
done< <(ps -e -o user,pid,rss --no-headers | sort)
