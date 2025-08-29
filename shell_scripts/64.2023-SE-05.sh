#!/bin/bash

commands=$(mktemp)

count=0
while true; do
    
    count_fat_comm=0
    
    prev=
    amount_rss=0
    while read -r comm rss; do
        if [[ "$prev" == "$comm" ]]; then
            (( amount_rss += rss ))
        else
            
            if (( amount_rss >= 65536 )); then
                (( count_fat_comm += 1 ))
                if ! grep -q "$prev " "$commands"; then
                    echo "${prev} 1" >> "$commands"
                else
                    cnt=$(grep "$prev " "$commands" | cut -d ' ' -f2)
                    new_count=$(( cnt + 1 ))
                    sed -iE "s/^${prev} ${cnt}$/${prev} ${new_count}/" "$commands"
                fi
            fi

            prev=$comm
            amount_rss=$rss
        fi
    done< <(ps -eo comm,rss | sort | tail -n +2)
   
    if (( amount_rss >= 65536 )); then
        (( count_fat_comm += 1 ))
        if ! grep -q "$prev " "$commands"; then
            echo "${prev} 1" >> "$commands"
        else
            cnt=$(grep "$prev " "$commands" | cut -d ' ' -f2)
            new_count=$(( cnt + 1 ))
            sed -iE "s/^${prev} ${cnt}$/${prev} ${new_count}/" "$commands"
        fi
    fi

    (( count += 1 ))

    if (( count_fat_comm == 0 )); then
        break
    fi

    sleep 1
done

half=$(( count / 2 ))
while read -r command cnt; do
    if [[ "$cnt" -ge "$half" ]]; then
        echo "$command"
    fi
done < "$commands"

rm "$commands"
