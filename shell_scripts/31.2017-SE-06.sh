#!/bin/bash

#if [[ $(id -u) -ne 0 ]]; then
 #   echo "You are not root"
  #  exit 0
#fi

uids=$(mktemp)
while IFS=: read -r userName _ uid _ _ directory _ ; do

    if [[ ! -d "$directory" ]]; then
       echo "$uid" >> $uids
       continue
    fi

    if [[ $(basename "$directory") != "$userName" ]]; then
       echo "$uid" >> $uids
       continue
    fi

    permissions=$(stat -c %A "$directory" 2>/dev/null)

    if [[ "${permissions:2:1}" != 'w' ]]; then
       echo "$uid" >> $uids
    fi


done< /etc/passwd

root_rss=0
while read -r rss; do
     (( root_rss += rss ))
done< <(ps -u root -o rss=)

while read -r uid; do
    cur_rss=0
    while read -r rss; do
         (( cur_rss += rss ))
    done< <(ps -u "$uid" -o rss=)

    if [[ $cur_rss -gt $root_rss ]]; then
        ps -u $uid -o pid= | while read -r pid; do
            kill $pid;
        done
    fi
done < "$uids"

rm "$uids"
