#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "Wrong arguments"
    exit 1
fi

if [[ ! -d ${1} || ! -d ${2} ]]; then
    echo "You must provide directories"
    exit 2
fi

isDbFileExist=$(find "${1}" -maxdepth 1 -mindepth 1 -type f -name "db")
isPackagesDirectoryExist=$(find "${1}" -maxdepth 1 -mindepth 1 -type d -name "packages")

if [[ -z "$isDbFileExist" || -z "$isPackagesDirectoryExist" ]]; then
    echo "You provided wrong database"
    exit 3
fi

isVersionFileExist=$(find "${2}" -maxdepth 1 -mindepth 1 -type f -name "version")
isTreeDirectoryExist=$(find "${2}" -maxdepth 1 -mindepth 1 -type d -name "tree")

if [[ -z "$isVersionFileExist" || -z "$isTreeDirectoryExist" ]]; then
    echo "You provided wrong package"
    exit 4
fi

version=(cat "${2}/version")

pkg_name=$(basename "${2}")

archive=$(mktemp --suffix=.tar.xz)
tar -c -C "${2}/tree" . | xz -z > "$archive"
sum=$(sha256sum "$archive" | cut -d ' ' -f1)

existing_line=$(grep "^${pkg_name}-${version} " "${1}/db" || true)
if [[ -n "$existing_line" ]]; then
    old_sum=$(echo "$existing_line" | cut -d ' ' -f2)
    old_archive="${1}/packages/${old_sum}.tar.xz"
    if [[ -f "$old_archive" ]]; then
        rm -f "$old_archive"
    fi
    grep -v "^${pkg_name}-${version} " "${1}/db" > "${1}/db.tmp"
    mv "${1}/db.tmp" "${1}/db"
fi

mv "$archive" "${1}/packages/${sum}.tar.xz"

echo "${pkg_name}-${version} ${sum}" >> "${1}/db"

sort -o "${1}/db" "${1}/db"
