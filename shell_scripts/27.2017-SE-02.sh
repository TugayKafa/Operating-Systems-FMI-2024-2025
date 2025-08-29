#!/bin/bash

# Проверка за брой аргументи
if [[ $# -ne 3 ]]; then
    echo "Usage: $0 SRC_DIR DST_DIR ABC"
    exit 1
fi

SRC=$1
DST=$2
ABC=$3

# Проверка дали SRC съществува и е директория
if [[ ! -d "$SRC" ]]; then
    echo "First argument must be an existing directory (SRC)!"
    exit 2
fi

# Проверка дали DST съществува и е директория
if [[ ! -d "$DST" ]]; then
    echo "Second argument must be an existing directory (DST)!"
    exit 2
fi

# Проверка дали DST е празна
if [[ $(find "$DST" -mindepth 1 -print -quit 2>/dev/null) ]]; then
    echo "Second directory (DST) must be empty!"
    exit 3
fi

# Намираме всички файлове с подниз ABC в името
find "$SRC" -type f -name "*$ABC*" | while read -r filepath; do
    # Относителен път спрямо SRC
    rel_path="${filepath#$SRC/}" # hashtagit deystva kato "Premahni prefiksa"

    # Създаваме съответната директория в DST
    mkdir -p "$(dirname "$DST/$rel_path")"

    # Копираме файла
    cp "$filepath" "$DST/$rel_path"
    echo "Copied: $rel_path"
done

