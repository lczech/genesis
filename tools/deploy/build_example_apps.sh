#!/bin/bash

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2016 Lucas Czech
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Contact:
# Lucas Czech <lucas.czech@h-its.org>
# Exelixis Lab, Heidelberg Institute for Theoretical Studies
# Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany

####################################################################################################
#    This script is used to test whether the C++ programs used for the demos and tutorials
#    of the manual are able to compile. This is important, as changes in the code base
#    may break them.
####################################################################################################

# Change to top level of git repo.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`

# First collect all file paths we are about to copy and build. We first check whether all files can
# be copied without conflict. This way, in case of errors, no copying happend yet, so no cleanup
# is necessary.
file_paths=()

# Collect demo files.
for filename in `find ./doc/demos/ -name "*.cpp"`; do
    file_paths+=( ${filename} )
done

# Collect tutorial files.
for filename in `find ./doc/tutorials/ -name "*.cpp"`; do
    file_paths+=( ${filename} )
done

# Store all base names of the files for checking and deleting later.
file_names=()

# Now check if there are naming conflicts and store the result.
do_exit=0
for filename in ${file_paths[@]}; do
    basename=$(basename "${filename}")

    # Check if a file of that name already exists in the apps dir.
    if [ -f ./apps/${basename} ]; then
        echo -e "\e[31mFile ${basename} already exists in apps dir.\e[0m"
        do_exit=1
    fi

    # Check if multiple files of the demos or tutorials have the same name.
    if [[ ${file_names[*]} =~ ${basename} ]]; then
        echo -e "\e[31mFile ${basename} is listed multiple times.\e[0m"
        do_exit=1
    fi

    # If everything is okay, copy this file name to the list if files.
    file_names+=( ${basename} )
done

# If there were conflicts, exit with failure.
if [[ ${do_exit} == 1 ]]; then
    echo "Exiting."
    exit 1
fi

# Now do the actual copying.
for filename in ${file_paths[@]}; do
    cp ${filename} ./apps/
done

# Run the incremental build process.
make update

# Cleanup: delete all copied files again.
for basename in ${file_names[@]}; do
    rm ./apps/${basename}
done

# Did it. Return success.
exit 0
