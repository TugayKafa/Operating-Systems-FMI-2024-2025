#!/bin/bash

lastModified=$(find /home/students -type f -printf "%u %p %T@\n" 2>/dev/null | sort -k3 -n -t " " | tail -n 1)
echo ${lastModified}
name=$(echo "${lastModified}" | cut -d ' ' -f1)
echo "Username: ${name}"
fileName=$(echo "${lastModified}" | cut -d ' ' -f2)
echo "Filename: ${fileName}"
