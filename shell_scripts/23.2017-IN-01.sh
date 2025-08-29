#!/bin/bash

if [[ $# -ne 3 ]]; then
    echo "args should be 3"
    exit 1
fi

file="$1"
key1="$2"
key2="$3"

if [[ ! -f "$file" ]]; then
    echo "file doesn't exist"
    exit 2
fi

secondKey=$(grep -E "^${key2}=" "$file")
if [[ $? -ne 0 ]]; then
    # Ако няма ключ key2 - просто отпечатваме файла и излизаме
    cat "$file"
    exit 3
fi

firstKeyArgs=$(grep -E "^${key1}=" "$file" | cut -d '=' -f2- | tr ' ' '\n' | sort)
secondKeyArgs=$(echo "$secondKey" | cut -d '=' -f2- | tr ' ' '\n' | sort)

# Намираме термините в secondKeyArgs, които НЕ са в firstKeyArgs
result=$(comm -23 <(echo "$secondKeyArgs") <(echo "$firstKeyArgs") | tr '\n' ' ' | sed 's/ *$//')

# Променяме реда с ключ key2, като заменяме с новите термове
sed -E -i  "s:(^${3}).*:\1=${result}:g" ${1}

