#!/bin/bash

input=$(mktemp)

for arg in ${@}; do 
    echo $arg >> $input
done

if ! grep -Eq -- "^-jar$" "$input"; then
    echo "-jar argument does not exist!"
    exit 0
fi

if ! grep -Eq ".+\.jar" "$input"; then
    echo "Not provides .jar file!"
    exit 0
fi

filename=$(grep -E ".+\.jar" "$input")

tmp=$(mktemp)

sed -E "s/^-jar$//" "$input" | sed -E "s/^.+\.jar$//" | tr '\n' ' ' | sed -E 's/ +/ /g; s/^ //; s/ $//' | tr ' ' '\n' > "$tmp"

options=$(cat $tmp | grep -E -- "^-.+$" | tr "\n" " ")
args=$(cat $tmp | grep -Ev "^-" | tr "\n" " ")

java $options -jar $filename $args

rm $input
rm $tmp
