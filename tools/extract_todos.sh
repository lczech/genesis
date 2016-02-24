#!/bin/bash

####################################################################################################
#    This script scans all C++ files in lib/, python/src/ and test/src/ for comments
#    that start with TODO or FIXME and writes them to a file ./TODO for easy access.
####################################################################################################

# Change to top level of git repo.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`

rm -f TODO

for file in `find lib/ python/src test/src -type f`; do
    fgrep -Hn -e "TODO"  ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tTODO\t/gI" >> TODO_tmp
    fgrep -Hn -e "FIXME" ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tFIXME\t/gI" >> TODO_tmp
done

cat TODO_tmp | column -t -s $'\t' > TODO
rm TODO_tmp
