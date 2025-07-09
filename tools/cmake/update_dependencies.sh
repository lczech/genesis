#!/bin/bash

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2025 Lucas Czech
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
# Lucas Czech <lucas.czech@sund.ku.dk>
# University of Copenhagen, Globe Institute, Section for GeoGenetics
# Oster Voldgade 5-7, 1350 Copenhagen K, Denmark

####################################################################################################
#    Update Git Submodule Commit Hashes
####################################################################################################

# Here, we update the commit hashes of the download scripts for our dependencies.
# The script uses git to get the hases that the submodules currently point to,
# and replaces them in the cmake download scripts.

# Change to main project dir. This ensures that the script can be called from any directory.
cd "$(dirname "$0")/../.."

# =======================================================
#     Hash
# =======================================================

function get_submodule_hash() {
    local libname=${1}

    # Disect the git submodule output to get the commit that is currently checked out.
    # This output is of the form ".<hash> libs/genesis (v0.19.0-14-g5b77dba)",
    # where the first dot is a space, plus sign or some other git status symbol.
    # This also sets the "return" to the outside scope... bash...
    commit_hash=`git submodule status | grep ${libname} | sed "s/.\([0-9a-f]*\) .*/\1/g" | tr -d "\n"`

    # Use two methods for checking.
    # This fails if the submodule is checked out, but not commited.
    # In that case, we only print a warning.
    local branch=`git rev-parse --abbrev-ref HEAD`
    local hash_a=`git ls-files -s libs/${libname} | cut -d" " -f 2`
    local hash_b=`git ls-tree ${branch} libs/${libname} | awk -F "[ \t]" '{print $3}'`

    if [[ ${hash_a} != ${hash_b} ]]; then
        echo -e "\e[31mProblem with commit hash for ${libname}: ${hash_a} != ${hash_b}\e[0m"
        return 1
    fi
    if [[ ${hash_a} != ${commit_hash} ]]; then
        echo -e "\e[31mNot yet commited submodule ${libname}: ${commit_hash} > ${hash_a}\e[0m"
        return 1
    fi
    return 0
}

function update_submodule_hash() {
    local libname=${1}
    local cmakedir=${2}

    get_submodule_hash ${libname}
    echo "${libname} @ ${commit_hash}"

    LINE="SET( ${libname}_GIT_HASH \"${commit_hash}\" ) #${libname}_GIT_HASH#"
    sed -i "s/.*#${libname}_GIT_HASH#/${LINE}/g" ${cmakedir}/CMakeLists.txt
}

# =======================================================
#     Tag
# =======================================================

function get_submodule_tag() {
    local libname="$1"
    local submod_path="external/${libname}"

    # Ensure the submodule directory exists
    if ! git -C "$submod_path" rev-parse --git-dir &>/dev/null; then
        echo -e "\e[31mError: '$submod_path' is not a git submodule directory.\e[0m"
        return 1
    fi

    # Extract the checked-out commit hash from `git submodule status`
    # lines look like " 5b77dba libs/genesis (v0.19.0-14-g5b77dba)"
    local commit_hash
    commit_hash=$(git submodule status "$submod_path" | sed -n 's/^[ +-]\([0-9a-f]\+\).*/\1/p')
    if [[ -z "$commit_hash" ]]; then
        echo -e "\e[31mError: failed to parse commit hash for '$libname'.\e[0m"
        return 1
    fi

    # Make sure the submodule has its tags locally
    git -C "$submod_path" fetch --tags --quiet

    # Look for an exact-match tag on that commit
    # `describe --exact-match` exits non-zero if none found
    commit_tag=""
    if ! commit_tag=$(git -C "$submod_path" describe --exact-match --tags "$commit_hash" 2>/dev/null); then
        echo -e "\e[31mError: submodule '$libname' at commit ${commit_hash} is not tagged.\e[0m"
        return 1
    fi
    return 0
}

function update_submodule_tag() {
    local libname=${1}
    local cmakedir=${2}

    if ! get_submodule_tag "$libname"; then
        echo "Error: could not retrieve tag for submodule '$libname'." >&2
        exit 1
    fi
    echo "${libname} @ ${commit_tag}"

    LINE="SET( ${libname}_GIT_TAG \"${commit_tag}\" ) #${libname}_GIT_TAG#"
    sed -i "s/.*#${libname}_GIT_TAG#/${LINE}/g" ${cmakedir}/CMakeLists.txt
}

####################################################################################################
#    Submodule Dependencies
####################################################################################################

# The update functions take the submodule name and the CMake subdirectory.
update_submodule_tag "googletest" "./test"
update_submodule_tag "htslib" "."
