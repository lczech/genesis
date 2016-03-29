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

echo ""
echo "===================================================================="
echo "This script creates the basic code for a new class file for genesis."
echo "===================================================================="
echo ""
echo "Please enter the module name that you want the class to be in:"
echo "(this is simply the containing folder for the code files)"
echo ""

# Change to top level of git repo and then to lib dir.
# This ensures that the script can be called from any directory.
cd `git rev-parse --show-toplevel`
cd lib/

select module in "Create new module" `ls -d */ | sed 's/\///g'` "Cancel"; do
    if [ "$module" == "Cancel" ]; then
        echo "Aborted. Nothing done."
        exit
    fi
    if [ "$module" == "Create new module" ]; then
        echo ""
        echo "--------------------------------------------------------------------"
        echo ""

        echo "Please enter a name for the new module:"
        echo ""
        read module
        module=${module,,}
        if [ "${module}" != `echo ${module} | sed 's/[^a-z]//g'` ]; then
            echo "Invalid module name. Name can only contain lowercase letters."
            echo "Aborted. Nothing done."
            exit
        fi
        if [ -d ${module} ]; then
            echo "Info: Module name already exists, it is not a new one."
        fi
        module="${module}/"
    fi
    if [ "${module}" != "" ]; then
        break
    fi
done

echo ""
echo "--------------------------------------------------------------------"
echo ""

# remove trailing / from name
module=${module%/}
echo "You selected module '$module'"

echo ""
echo "Please enter a name for the class, space separated for composed names:"
echo "Example: 'hello world' will create HelloWorld class."
echo ""
read name

echo ""
echo "--------------------------------------------------------------------"
echo ""

# remove surplus spaces
name=`echo ${name} | sed -r 's/^ *'// | sed -r 's/ *$//' | sed -r 's/  */ /g'`

if [ "${name}" != "`echo ${name} | sed 's/[^a-zA-Z0-9 ]//g'`" ]; then
    echo "Invalid class name. Name can only contain letters, digits and spaces."
    echo "Aborted. Nothing done."
    exit
fi

file_base=${name,,}
if [ "${module}" == "`echo ${name,,} | awk '{print $1;}'`" ]; then
    echo "The class name starts with the prefix of the module name."
    echo "Do you want to keep this in the file name, or shorten it for brevity?"
    echo "(the class name itself will be unchanged)"

    select yn in "Keep" "Shorten"; do
        if [ "${yn}" == "Shorten" ]; then
            file_base=`echo ${file_base} | sed -r 's/^\w*\ *//'`
        fi
        if [ "${yn}" != "" ]; then
            break
        fi
    done

    echo ""
    echo "--------------------------------------------------------------------"
    echo ""
fi

# make class name CamelCased and file name with underscores
class_name=`echo $name | sed -r 's/ ([a-z])/\U\1/g' | sed -r 's/^([a-z])/\U\1/g'`
file_name=`echo ${file_base,,} | sed -r 's/ /_/g'`
cap_module=`echo $module  | sed -r 's/([a-z])/\U\1/g'`
cap_file=`echo $file_name | sed -r 's/([a-z])/\U\1/g'`

echo "About to create class '$class_name'."
echo ""

# Names of the different options.
f_name[0]="header"
f_name[1]="code"
f_name[2]="template"
f_name[3]="test"
f_name[4]="python"

# Mark which files the user wants to create.
f_create[0]="x"
f_create[1]="x"
f_create[2]=" "
f_create[3]=" "
f_create[4]=" "

f_source[0]="../tools/templates/class.hpp"
f_source[1]="../tools/templates/class.cpp"
f_source[2]="../tools/templates/class.tpp"
f_source[3]="../tools/templates/test.cpp"
f_source[4]="../tools/templates/python.cpp"

# List of target file names.
f_target[0]="lib/${module}/${file_name}.hpp"
f_target[1]="lib/${module}/${file_name}.cpp"
f_target[2]="lib/${module}/${file_name}.tpp"
f_target[3]="test/src/${module}/${file_name}.cpp"
f_target[4]="python/src/${module}/${file_name}.cpp"

echo -e "Press the number keys to select the files that"
echo -e "you want to create (press again to unselect)."
echo -e "Press enter when done.\n"

# Choose which files to create.
while true; do
    echo "[${f_create[0]}] 1: Header   : ${f_target[0]}"
    echo "[${f_create[1]}] 2: Code     : ${f_target[1]}"
    echo "[${f_create[2]}] 3: Template : ${f_target[2]}"
    echo "[${f_create[3]}] 4: Test     : ${f_target[3]}"
    echo "[${f_create[4]}] 5: Python   : ${f_target[4]}"

    read -s -n 1 key
    if [[ ${key} = "" ]]; then
        break
    fi

    if [[ $key =~ ^-?[1-5]$ ]]; then
        if [[ ${f_create[$(( ${key} - 1 ))]} == "x" ]]; then
            f_create[$(( ${key} - 1 ))]=" "
        else
            f_create[$(( ${key} - 1 ))]="x"
        fi
    fi

    echo "$(tput cuu 6)"
done

echo ""
echo "--------------------------------------------------------------------"
echo ""

# If the user wants to create a template, we need its params.
param=""
if [[ ${f_create[2]} == "x" ]]; then
    echo "Please enter the template parameters as they appear between <...>"
    echo "Example: 'class T' will create template<class T>."
    echo ""
    read param
    echo ""
    echo "--------------------------------------------------------------------"
    echo ""
fi

# Create all files by replacing placeholders in the template files.
for i in $(seq 0 4); do
    if [[ ${f_create[${i}]} == "x" ]]; then
        if [ -f "../${f_target[${i}]}" ] ; then
            echo "File '${f_target[${i}]}' already exist. Will not overwrite it."
            echo "Did not create ${f_name[${i}]} file."
            continue
        fi

        dir_name=$(dirname "../${f_target[${i}]}")
        mkdir -p ${dir_name}

        # Replacing differs between templated and non-templated code.
        if [[ ${f_create[2]} == "x" ]]; then
            cat ${f_source[${i}]}                                       | \
                sed "s/###CLASSNAME###/${class_name}/g"                 | \
                sed "s/###MODULNAME###/${module}/g"                     | \
                sed "s/###FILENAME###/${file_name}/g"                   | \
                sed "s/###CAPMODULNAME###/${cap_module}/g"              | \
                sed "s/###CAPFILENAME###/${cap_file}/g"                 | \
                sed "s/###TEMPLATE_LINE###//g"                          | \
                sed "s/###TEMPLATE_START###//g"                         | \
                sed "s/###TEMPLATE_END###//g"                           | \
                sed "s/###TEMPLATE_PARAM###/${param}/g"                 > \
                "../${f_target[${i}]}"
        else
            cat ${f_source[${i}]}                                       | \
                sed "s/###CLASSNAME###/${class_name}/g"                 | \
                sed "s/###MODULNAME###/${module}/g"                     | \
                sed "s/###FILENAME###/${file_name}/g"                   | \
                sed "s/###CAPMODULNAME###/${cap_module}/g"              | \
                sed "s/###CAPFILENAME###/${cap_file}/g"                 | \
                sed "/###TEMPLATE_LINE###/d"                            | \
                sed "/###TEMPLATE_START###/,/###TEMPLATE_END###/d"      > \
                "../${f_target[${i}]}"
        fi

        echo "Created ${f_name[${i}]} file."
    fi
done

echo -e "\nDone."
