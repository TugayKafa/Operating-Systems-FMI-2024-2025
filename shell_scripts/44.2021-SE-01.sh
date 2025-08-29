#!/bin/bash

user=$(whoami)
if [[ "$user" != "oracle" "$user" != "grid" ]]; then
    echo "Script must be run as oracle or grid!"
    exit 1
fi

if [[ -z "$ORACLE_HOME" ]]; then
    echo "ORACLE_HOME not set"
    exit 2
fi

adrci_exec="$ORACLE_HOME/bin/adrci"

if [[ ! -x "$adrci_exec"  ]]; then
    echo "adrci not found or not executable!"
    exit 3
fi

homes=$("$adrci_exec" exec="show homes" 2>/dev/null)

if echo "$homes" | grep -q "No ADR homes are set"; then
    exit 0
fi

diag_dest="/u01/app/$user"

while read -r real_path; do
    abs_path="$diag_dest/$rel_path"
    if [[ -d "$abs_path" ]]; then
        size_mb=$(du -sm "$abs_path" | awk '{print $1}')
        echo "$size_mb $abs_path"
    fi
done< <(echo "$homes" | sed "1d")
