#!/bin/bash

####################################################################################################
#    This script first runs doxygen to create an xml representation of the code and then
#    uses the python script to generate bindings code from this.
####################################################################################################

# Change to top level of git repo and then to the py bindings dir.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd tools/py_bindings/

function print_separator() {
    echo
    echo -en "\e[32m==========================================================================="
    echo     "========================="
    echo -e  "          ${1}"
    echo -en "================================================================================="
    echo -e  "===================\e[0m"
    echo
}

# Run doxygen
print_separator "Running doxygen"
rm -rf xml/
doxygen > /dev/null

# Run the script
print_separator "Running bindings script"
rm -rf src/
./generate_bindings.py
