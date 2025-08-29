#!/bin/bash

if [[ ${#} -ne 1 ]]; then
    echo "You need to provide 1 argument"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    echo "You need to provide configuration file as an argument!"
    exit 1
fi

configuration_file=$(mktemp)

cat "${1}" | sed 's/#.*//g' | sed -E 's/ +/ /g' >> "$configuration_file"

current_state=$(mktemp)

cat /proc/acpi/wakeup | tail -n +2 | sed -E 's/ +/ /g' | cut -d ' ' -f1,3 | sed 's/*//g' >> "$current_state"

while read -r line; do
    if [[ -z "$line" ]]; then
        continue;
    fi
    
    device=$(echo "$line" | cut -d ' ' -f1)
    stat=$(echo "$line" | cut -d ' ' -f2)
    
    if [[ ! "$device" =~ [0-9A-Z]{1,4} ]]; then
        echo "WARNING: Device name $device is wrong!"
        continue
    fi

    if [[ "$stat" != "enabled" && "$stat" != "disabled" ]]; then
        echo "WARNING: Status: $stat for Device: $device does not exist!"
        continue
    fi

    if ! grep -q "^$device " "$current_state"; then
        echo "WARNING: Device: $device does not exist in system!"
        continue
    fi

    cur_dev_status=$(grep -E "^$device " "$current_state" | cut -d ' ' -f2)

    if [[ "$stat" != "$cur_dev_status" ]]; then
        echo "$device" >> /proc/acpi/wakeup
    fi

    echo "Status of device $device is set to be $stat !"

done < "$configuration_file"

rm "$configuration_file"
rm "$current_state"
