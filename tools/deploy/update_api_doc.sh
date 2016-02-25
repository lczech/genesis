#!/bin/bash

####################################################################################################
#    This script builds and transfers the api documentation in doc/html to the web server.
####################################################################################################

# Change to top level of git repo and then to ./doc
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd doc/

# Set options: build api and transfer it to server.
do_build=0
do_trans=0
if [[ $1 == "" || $1 == "both" ]]; then
    do_build=1
    do_trans=1
fi
if [[ $1 == "build" ]]; then
    do_build=1
fi
if [[ $1 == "transfer" ]]; then
    do_trans=1
fi

# Build doxygen api manual.
if [[ ${do_build} == 1 ]]; then
    echo -e "\e[34mDoxygen error output:\e[0m\n"

    make clean
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
    cd api/

    remote_host="genesis-lib.org"
    remote_path="/html/"

    echo -e    "Target:   ${remote_host}${remote_path}api/"
    read    -p "Username: " remote_user
    read -s -p "Password: " remote_pswd
    echo -e "\n"

    # Run the transfer script. (Unfortunately, indention has to be like that...)
    # The transfer for all the doxygen generated files (>2,000) takes a while. In order to minimize
    # off-time, we first transfer to a tmp dir `api_new`. then quickly switch with the old one,
    # and only after that delete the old dir.
ncftp <<EOF
open -u $remote_user -p $remote_pswd $remote_host

cd $remote_path
mkdir api_mew

cd api_mew
put -R *
cd ..

rename api api_old
rename api_mew api

rm api_old/search/*
rmdir api_old/search/
rm api_old/*
rmdir api_old/

bye
EOF

    unset remote_user
    unset remote_pswd
    cd ..
fi
