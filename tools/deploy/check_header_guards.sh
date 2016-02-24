#!/bin/bash

####################################################################################################
#    This script checks whether the include guards of the headers in ./lib are what they
#    should be according to their path and file name.
####################################################################################################

# Change to top level of git repo and then to /lib
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/

for header in `find . -name "*.hpp"`; do
    # Extract wanted guard name from file name.
    guard="${header/.\//genesis_}"
    guard="${guard/.hpp/_h_}"
    guard="${guard//\//_}"
    guard="${guard^^}"

    # Extract actual names from file.
    line_ifn=`head -n 1 ${header} | cut -d " " -f 2`
    line_def=`head -n 2 ${header} | tail -n 1 | cut -d " " -f 2`

    # Check first line (ifndef).
    # We skip the (all-including) genesis header, because it is special.
    if [[ ${line_ifn} != ${guard} && ${header} != "./genesis.hpp" ]]; then
        echo "In ${header}:"
        echo "   Actual: ${line_ifn}"
        echo "   Wanted: ${guard}"
    fi

    # Check second line (define)
    if [[ ${line_ifn} != ${line_def} ]]; then
        echo "In ${header}:"
        echo "   Condition:  ${line_ifn}"
        echo "   Definition: ${line_def}"
    fi
done
