#!/bin/bash

####################################################################################################
#    This script builds and transfers the documentation in doc/doc to the web server.
####################################################################################################

# These are the operations this script can do:
#   * clean: Delete all doc files.
#   * make: Build the doc. Update only the changed files.
#   * refresh: Full new build of the doc files (= clean + make).
#   * transfer: Transfer the files to the server. Does not build them!
#   * local: Open the local page in a browser.
#   * remote: Open the remote page in a browser.
# The operation can be provided via the command line parameter.
# If none is given, make is executed.

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
    open_in_browser "doc/index.html"
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
    cd doc/

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
