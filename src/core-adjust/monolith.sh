#!/bin/bash

# This scripts 'assembles' the final core-adjust script.
# See CMakeLists.txt

# 'source' the script in $1
Source() {
  local line
  while IFS= read -r line; do
    [[ $line =~ ^[[:space:]]*# ]] && continue
    echo "$line"
  done < $2/$1
}

# 'assemble' the script in $1
# $2 == $(srcdir)
Main() {
  local line
  while IFS= read -r line; do
    [[ $line =~ ^\.(.*)$ ]] && Source ${BASH_REMATCH[1]} $2 || echo "$line"
  done < $1
}

Main "$@"

