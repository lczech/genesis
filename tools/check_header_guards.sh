#!/bin/bash

# This script checks whether the include guards of the headers in ./lib are what they
# should be according to their path and file name.

cd ../lib/

for header in `find . -name "*.hpp"`; do
    guard="${header/.\//genesis_}"
    guard="${guard/.hpp/_h_}"
    guard="${guard//\//_}"
    guard="${guard^^}"

    line=`head -n 1 ${header} | cut -d " " -f 2`

    if [[ ${line} != ${guard} && ${header} != "./genesis.hpp" ]]; then
        echo "In ${header}:"
        echo "   Actual: ${line}"
        echo "   Wanted: ${guard}"
    fi
done
