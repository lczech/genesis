#!/bin/bash

####################################################################################################
#    This script updates the generell (all-including) header file lib/genesis.hpp
####################################################################################################

# Change to top level of git repo and then to /lib
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/
rm -f genesis.hpp

echo "#ifndef GENESIS_H_" >> genesis.hpp
echo "#define GENESIS_H_" >> genesis.hpp
echo "" >> genesis.hpp
echo "/**" >> genesis.hpp
echo " * @brief This header includes all other genesis headers (*.hpp)." >> genesis.hpp
echo " * This makes it easy to integrate the code as a library, as all" >> genesis.hpp
echo " * symbols of genesis are available after including this header." >> genesis.hpp
echo " *" >> genesis.hpp
echo " * You do not need to edit this file manually. Simply run the script" >> genesis.hpp
echo " * make_genesis_header.sh in ./tools to update this file." >> genesis.hpp
echo " */" >> genesis.hpp
echo "" >> genesis.hpp

for d in `ls -v -d */`
do
    for f in `find ${d} -name "*.hpp" | sort`
    do
        echo "#include \"${f}\"" >> genesis.hpp
    done
    echo "" >> genesis.hpp
done

echo "#endif // include guard" >> genesis.hpp
