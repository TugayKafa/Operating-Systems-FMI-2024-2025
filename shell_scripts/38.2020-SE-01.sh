#!/bin/bash

if [[ ${#} -ne 2 ]]; then
    echo "Wrong Arguments!"
    exit 1
fi

if [[ ! -f ${1} ]]; then
    touch ${1}
fi

if [[ ! -d ${2} ]]; then
    echo "Second argument must be directory!"
    exit 2
fi

files=$(find "${2}" -type f -name "*.log")

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" > "${1}"

while IFS= read -r file; do
    data=$(cat "$file" | sed -e "s/  */ /g")    
    hostname=$(basename "$file" | sed -e "s/.log//g")
    phy=$(echo "$data" | grep -e "Maximum Physical Interfaces" | cut -d ':' -f2 | sed -e "s/ *//g")
    vlans=$(echo "$data" | grep -e "VLANs" | cut -d ':' -f2 | sed -e "s/ *//g")
    hosts=$(echo "$data" | grep -e "Inside Hosts" | cut -d ':' -f2 | sed -e "s/ *//g")
    failover=$(echo "$data" | grep -e "Failover" | cut -d ':' -f2 | sed -e "s/ *//g")
    VPN=$(echo "$data" | grep -e "VPN-3DES-AES" | cut -d ':' -f2 | sed -e "s/ *//g")
    peers=$(echo "$data" | grep -e "Total VPN Peers" | cut -d ':' -f2 | sed -e "s/ *//g")
    VLAN=$(echo "$data" | grep -e "VLAN Trunk Ports" | cut -d ':' -f2 | sed -e "s/ *//g")
    license=$(echo "$data" | grep -e "This platform has" | awk '{for(i=5;i<NF;i++) printf "%s%s", $i, (i<NF-1 ? OFS : ORS)}')
    SN=$(echo "$data" | grep -e "Serial Number" | cut -d ':' -f2 | sed -e "s/ *//g")
    key=$(echo "$data" | grep -e "Running Activation Key" | cut -d ':' -f2 | sed -e "s/ *//g")
    echo "$hostname,$phy,$vlans,$hosts,$failover,$VPN,$peers,$VLAN,$license,$SN,$key" >> "${1}"
done< <(echo "$files")
