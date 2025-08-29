#!/bin/bash

#if [[ $(id -u) -ne 0 ]]; then
 #   echo "You are not root"
  #  exit
#fi


result=$(cat /etc/passwd | grep -E -v "home/")
while read -r row; do
    path=$(echo "${row}" | cut -d ':' -f6)
    if [[ -w $path ]]; then
        echo "$row" >> result
    fi
done< <(cat /etc/passwd | grep -E "home/")

echo "$result"
