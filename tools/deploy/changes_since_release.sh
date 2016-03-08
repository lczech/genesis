#!/bin/bash

####################################################################################################
#    This script lists notable changes since the last release.
####################################################################################################

# Get current version.
last_tag=`git describe --abbrev=0 --tags`

# Get all important commits after the last tag and format them for Markdown.
echo -e "\e[34mNotable changes since version ${last_tag}\e[0m\n"
git log ${last_tag}..HEAD --oneline | cut -d " " -f 1 --complement | egrep -iv "^(Minor|Merge|Release)" | sed "s/^/  \* /g"
