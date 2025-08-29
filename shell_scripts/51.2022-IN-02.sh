#!/bin/bash

# Проста версия по твоето задание, без допълнителни функции
user=$(whoami)
if [[ "$user" != "oracle" && "$user" != "grid" ]]; then
  echo "You cannot execute this script!"
  exit 1
fi

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <days>"
  exit 2
fi

days=$1
if [[ ! $days =~ ^[1-9][0-9]*$ ]]; then
  echo "Argument must be a positive integer (days)"
  exit 3
fi

# Проверка на env променливи
if [[ -z "${ORACLE_BASE:-}" ]]; then
  echo "ORACLE_BASE is not set!"
  exit 4
fi

role="SYSASM"
if [[ "$user" == "oracle" ]]; then
  if [[ -z "${ORACLE_HOME:-}" || -z "${ORACLE_SID:-}" ]]; then
    echo "ORACLE_HOME and ORACLE_SID must be set for oracle user!"
    exit 4
  fi
  role="SYSDBA"
fi

# Определяме diagnostic_dest
diagnostic_dest=$("${ORACLE_HOME:-}"/bin/sqlplus -S -L "/ as $role" @foo.sql 2>/dev/null | sed -e '1,2d' -e '/^$/d')
if [[ -z "$diagnostic_dest" ]]; then
  diagnostic_dest="$ORACLE_BASE"
fi

diag_dir="$diagnostic_dest/diag"
if [[ ! -d "$diag_dir" ]]; then
  echo "Diagnostic directory $diag_dir not found!"
  exit 5
fi

# --- Oracle: rdbms ---
if [[ "$user" == "oracle" ]]; then
  bytes=0
  while IFS= read -r file; do
    if [[ "$file" =~ ^.+_[0-9]+\.(trc|trm)$ ]]; then
      file_size=$(stat -c '%s' "$file")
      bytes=$(echo "$bytes + $file_size" | bc)
    fi
  done < <(find "$diag_dir/rdbms" -mindepth 3 -maxdepth 3 -type f -mtime +"$days")
  # конверсия в KB
  kb=$((bytes / 1024))
  echo "rdbms: $kb"
  exit 0
fi

# --- Grid: crs и tnslsnr ---
machine=$(hostname -s)

# CRS
bytes=0
while IFS= read -r file; do
  if [[ "$file" =~ ^.+_[0-9]+\.(trc|trm)$ ]]; then
    file_size=$(stat -c '%s' "$file")
    bytes=$(echo "$bytes + $file_size" | bc)
  fi
done < <(find "$diag_dir/crs/$machine/crs/trace" -type f -mtime +"$days")
kb_crs=$((bytes / 1024))
echo "crs: $kb_crs"

# TNSLSNR
bytes=0
while IFS= read -r dir; do
  # alert
  while IFS= read -r file; do
    if [[ "$file" =~ ^.+_[0-9]+\.xml$ ]]; then
      file_size=$(stat -c '%s' "$file")
      bytes=$(echo "$bytes + $file_size" | bc)
    fi
  done < <(find "$dir/alert" -type f -mtime +"$days")
  # trace
  while IFS= read -r file; do
    if [[ "$file" =~ ^.+_[0-9]+\.log$ ]]; then
      file_size=$(stat -c '%s' "$file")
      bytes=$(echo "$bytes + $file_size" | bc)
    fi
  done < <(find "$dir/trace" -type f -mtime +"$days")
done < <(find "$diag_dir/tnslsnr/$machine" -mindepth 2 -maxdepth 2 -type d)
kb_tns=$((bytes / 1024))
echo "tnslsnr: $kb_tns"
