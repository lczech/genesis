#!/bin/bash

cd ../
rm -f TODO

for file in `find lib/ python/src test/src -type f`; do
    fgrep -Hn -e "TODO"  ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tTODO\t/gI" >> TODO_tmp
    fgrep -Hn -e "FIXME" ${file} | sed "s/[[:space:]]*[\/\*]*[[:space:]]*\(TODO\|FIXME\)[[:space:]]*/\tFIXME\t/gI" >> TODO_tmp
done

cat TODO_tmp | column -t -s $'\t' > TODO
rm TODO_tmp
