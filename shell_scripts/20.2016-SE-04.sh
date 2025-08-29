#!/bin/bash

if [[ ${#} != 2 ]]; then
  echo "Wrong arguments"
  exit 1
fi


isNumber() {
     [[ $1 =~ ^[1-9]+[0-9]*$ ]]
}

if ! isNumber $1 || ! isNumber $2; then
    echo "Arguments must be numbers!";
    exit 2
fi


mkdir -p a b c

path=$(pwd)

while read -r file; do
    if [[ -f $file && $file != '20.2016-SE-04.sh' ]]; then
        rows=$(cat $file | wc -l)
        if [[ $rows -lt $1 ]]; then
            cp $path/$file $path/a
        fi
        if [[ $rows -ge $1 && $rows -le $2 ]]; then
            cp $path/$file $path/b
        fi
        if [[ $rows -gt $2 ]]; then
            cp $path/$file $path/c
        fi
    fi
done< <(find . -type f -printf "%f\n")
