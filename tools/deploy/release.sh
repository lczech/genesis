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
#    Helper functions
####################################################################################################

function print_separator() {
    echo
    echo -en "\e[32m==========================================================================="
    echo     "========================="
    echo -e  "          ${1}"
    echo -en "================================================================================="
    echo -e  "===================\e[0m"
    echo
}

function get_user_confirmation() {
    if [[ ${1} != "" ]]; then
        text=$1
    else
        text="Do you want to continue?"
    fi

    response="x"
    while [[ ${response} != "y" ]]; do
        read -p "${text} [y/n] " -n 1 -r response
        echo

        if [[ $response == "y" ]]; then
            return 1
        fi
        if [[ $response == "n" ]]; then
            return 0
        fi
    done
}

####################################################################################################
#    Check preconditions
####################################################################################################

print_separator "Check preconditions"

# Change to top level of git repo.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`

# Check repo mame.
# If applied to a different repo, this script might do weird stuff, so better check.
base_name=`git rev-parse --show-toplevel | xargs basename`
if [[ ${base_name} != genesis* ]]; then
    echo -e "\e[31mWrong repository. Expect genesis.\e[0m"
    exit
else
    echo "Repository: ${base_name}"
fi

# Get and check current branch.
branch=`git rev-parse --abbrev-ref HEAD`
if [[ ${branch} != "master" ]]; then
    echo -e "\e[31mNot on master branch. Aborting.\e[0m"
    exit
else
    echo "Branch: ${branch}"
fi

# Check changed files.
changed_files=`git diff --name-only HEAD`
if [[ ${changed_files} != "" ]]; then
    echo -e "\e[31mUncommitted files. Aborting.\e[0m"
    exit
else
    echo "No uncommitted files."
fi

# Check stash.
stashed_files=`git stash list`
if [[ ${stashed_files} != "" ]]; then
    echo -e "\e[31mStashed files. Aborting.\e[0m"
    exit
else
    echo "No stashed files."
fi

# Check for unmerged branches.
unmerged=`git branch --no-merged`
if [[ ${unmerged} != "" ]]; then
    echo "There are unmerged branches:"
    echo ${unmerged}
    echo

    get_user_confirmation
    cont=$?
    if [[ $cont == 0 ]]; then
        echo -e "\e[31mAborted.\e[0m"
        exit
    fi
else
    echo "No unmerged branches."
fi

# Check header guards
header_guards=`./tools/deploy/check_header_guards.sh`
if [[ ${header_guards} != "" ]]; then
    echo -e "\n\e[31mHeader guards inconsistent:\e[0m"
    ./tools/deploy/check_header_guards.sh
    echo -e "\n\e[31mAborted.\e[0m"
    exit
else
    echo "Header guards okay."
fi

####################################################################################################
#    Build all
####################################################################################################

print_separator "Build all"

make clean
make

####################################################################################################
#    Demo and Tutorial Programs
####################################################################################################

print_separator "Demo and Tutorial Programs"

# Build the demo and tutorial programs.
./tools/deploy/build_example_apps.sh
success=$?
echo

# Abort if not successfull.
if [[ ${success} != 0 ]]; then
    echo -e "\e[31mCannot build demo and tutorial programs. Aborting.\e[0m"
    exit
else
    echo "Built demo and tutorial programs."
fi

####################################################################################################
#    Tests
####################################################################################################

print_separator "Test"

# Run tests.
cd test
./run.sh
success=$?
cd ..
echo

# TODO also run mem test?!

# Abort if not successfull.
if [[ ${success} != 0 ]]; then
    echo -e "\e[31mTests failed. Aborting.\e[0m"
    exit
else
    echo "Tests successfull."
fi

####################################################################################################
#    Version
####################################################################################################

print_separator "Version"

# Output current version.
last_tag=`git describe --abbrev=0 --tags`
echo -en "Latest version tag:    \e[34m${last_tag}\e[0m\n\n"

# Read version tag.
read -p "Enter new version tag: v" version
version="v${version}"
# echo ${version}
echo

# Replace version line in genesis header file.
echo "Replace version in lib/utils/core/genesis.hpp"
sed -i "s/    return \".*\"; \/\/ #GENESIS_VERSION#/    return \"${version}\"; \/\/ #GENESIS_VERSION#/g" lib/utils/core/genesis.hpp

# Replace version line in doxygen file.
echo "Replace version in doc/doxygen/Doxyfile"
sed -i "s/PROJECT_NUMBER *=.*/PROJECT_NUMBER         = \"${version}\"/g" doc/doxygen/Doxyfile

# Update genesis header
echo "Update genesis header lib/genesis.hpp"
./tools/deploy/make_genesis_header.sh

####################################################################################################
#    Build with version
####################################################################################################

print_separator "Build with version"

# Build again, this time with the updated version tag.
make update

####################################################################################################
#    Commit and Tag
####################################################################################################

print_separator "Commit and Tag"

# Confirm changes.
echo -e "\e[34mCurrent git status:\e[0m\n"
git status
echo -e "\n\e[34mAbout to commit changes and to create version tag ${version}\e[0m\n"

get_user_confirmation
cont=$?
if [[ $cont == 0 ]]; then
    echo -e "\e[31mAborted.\e[0m"
    exit
fi
echo

# Commit and Tag.
echo "Make commit, create tag..."
git commit --allow-empty -am "Release ${version}"
git tag -a "${version}" -m "Release ${version}"
echo -e "...done."

####################################################################################################
#    Publish to GitHub
####################################################################################################

print_separator "Publish to GitHub"

# Show changes.
echo -e "\e[34mCurrent git status:\e[0m\n"
git status
echo

# Confirm.
get_user_confirmation "Do you want to push the commit and tag to GitHub?"
cont=$?

# Push.
if [[ $cont == 1 ]]; then
    echo
    git push --all --follow-tags
fi

####################################################################################################
#    Prepare GitHub Release
####################################################################################################

print_separator "Prepare GitHub Release"

# Get current (the new) tag.
new_tag=`git describe --abbrev=0 --tags`

if [[ ${new_tag} != ${version} ]]; then
    echo "Weird, version ${version} and tag ${new_tag} differ..."
    echo
fi

# Get all important commits after the last tag and format them for Markdown.
echo -e "\e[34m### Notable Changes ###\e[0m\n"
git log ${last_tag}..${new_tag} --oneline | cut -d " " -f 1 --complement | egrep -iv "^(Minor|Merge|Release)" | sed "s/^/  \* /g"
echo -e "\nUse this list for the release message.\n"

# Ask user for github page.
get_user_confirmation "Do you want to open the GitHub release page?"
cont=$?

# Open github release page.
if [[ $cont == 1 ]]; then
    github_release_url="https://github.com/lczech/genesis/releases/new"
    if which xdg-open > /dev/null
    then
        xdg-open ${github_release_url}
    elif which gnome-open > /dev/null
    then
        gnome-open ${github_release_url}
    else
        echo "Cannot open page."
    fi
fi

####################################################################################################
#    Publish to Web Server
####################################################################################################

print_separator "Publish API to Web Server"

# Confirm.
get_user_confirmation "Do you want to update and publish the doxygen api help to the web server?"
cont=$?

# Transfer doxygen html help to server.
if [[ $cont == 1 ]]; then
    echo
    ./tools/deploy/update_api_doc.sh "refresh"
    echo
    ./tools/deploy/update_api_doc.sh "transfer"
fi

####################################################################################################
#    Finished
####################################################################################################

print_separator "Finished"
