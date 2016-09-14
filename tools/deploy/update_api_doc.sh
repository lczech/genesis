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
#    This script builds and transfers the documentation in doc/html to the web server.
####################################################################################################

if [[ $1 != ""         &&
      $1 != "clean"    &&
      $1 != "make"     &&
      $1 != "refresh"  &&
      $1 != "transfer" &&
      $1 != "local"    &&
      $1 != "remote"
]]; then
    echo "Unsupported operation: ${1}."
    cat <<EOF

These are the operations this script can do:

  * clean: Delete all doc files.
  * make: Build the doc. Update only the changed files.
  * refresh: Full new build of the doc files (= clean + make).
  * transfer: Transfer the files to the server. Does not build them!
  * local: Open the local page in a browser.
  * remote: Open the remote page in a browser.

The operation can be provided via the command line parameter.
If none is given, make is executed.
EOF
    exit
fi

function open_in_browser() {
    if which xdg-open > /dev/null
    then
        xdg-open ${1}
    elif which gnome-open > /dev/null
    then
        gnome-open ${1}
    else
        echo "Cannot open page."
    fi
}

# Change to top level of git repo and then to ./doc
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd doc/

# Set options: build and transfer it to server.
do_clean=0
do_make=0
do_trans=0

# Check command line.
if [[ $1 == "clean" ]]; then
    do_clean=1
fi
if [[ $1 == "" || $1 == "make" ]]; then
    do_make=1
fi
if [[ $1 == "refresh" ]]; then
    do_clean=1
    do_make=1
fi
if [[ $1 == "transfer" ]]; then
    do_trans=1
fi

# Open in Browser
if [[ $1 == "local" ]]; then
    open_in_browser "html/index.html"
fi
if [[ $1 == "remote" ]]; then
    open_in_browser "http://doc.genesis-lib.org/"
fi

# Clean up the files.
if [[ ${do_clean} == 1 ]]; then
    make clean
fi

# Build doxygen manual.
if [[ ${do_make} == 1 ]]; then
    echo -e "\e[34mDoxygen error output:\e[0m\n"

    make > /dev/null
fi

# Transfer to server.
if [[ ${do_trans} == 1 ]]; then

    if [[ `which ncftp` == "" ]]; then
        echo "Program ncftp not found."
        echo -e "\n\e[31mAborted.\e[0m"
        exit
    fi

    echo -e "\e[34mTransfer to server\e[0m\n"
    cd html/

    remote_host="genesis-lib.org"
    remote_path="/html/"

    echo -e    "Target:   ${remote_host}${remote_path}doc/"
    read    -p "Username: " remote_user
    read -s -p "Password: " remote_pswd
    echo -e "\n"

    # Run the transfer script. (Unfortunately, indention has to be like that...)
    # The transfer for all the doxygen generated files (>2,000) takes a while. In order to minimize
    # off-time, we first transfer to a tmp dir `doc_new`. then quickly switch with the old one,
    # and only after that delete the old dir.
ncftp <<EOF
open -u $remote_user -p $remote_pswd $remote_host

cd $remote_path
mkdir doc_new

cd doc_new
put -R *
cd ..

rename doc doc_old
rename doc_new doc

rm doc_old/search/*
rmdir doc_old/search/
rm doc_old/*
rmdir doc_old/

bye
EOF

    unset remote_user
    unset remote_pswd
    cd ..
fi
