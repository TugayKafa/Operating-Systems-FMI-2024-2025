#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You must provide 1 directory as argument!"
    exit 1
fi

if [[ ! -d ${1} ]]; then
    echo "Provided argument must be directory!"
    exit 2
fi

fuga=${1}

if [[ ! -f $fuga/foo.pwd ]]; then
    echo "Provided directory does not have authentication file!"
    exit 3
fi

if [[ ! -d $fuga/cfg ]]; then
    echo "Directory $fuga/cfg does not exist!"
    exit 0
fi

while read -r filename; do
    output=$("$fuga/validate.sh" "$filename")
    f_status=${?}
    if [[ $f_status -eq 0 ]]; then
        basename=$(basename $filename)
        name=${base%.cfg}
        if ! grep -Eq "^$name:" "$fuga/foo.pwd"; then
            password=$(pwgen 8 1 | mkpasswd)
            echo "$name:$password" >> $fuga/foo.pwd
            echo "$name:$plain"
        fi
    elif [[ $f_status -eq 1 ]]; then
        while read -r line; do
            echo "$filename:$line" >&2
        done <<< $output
    else
        echo "Something bad happened while validation $filename"
    fi
done< <(find $fuga/cfg -mindepth 1 -type f -name "*.cfg")
