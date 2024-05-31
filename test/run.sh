#!/bin/bash

# Genesis - A toolkit for working with phylogenetic data.
# Copyright (C) 2014-2024 Lucas Czech
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
# Lucas Czech <lczech@carnegiescience.edu>
# Department of Plant Biology, Carnegie Institution For Science
# 260 Panama Street, Stanford, CA 94305, USA

####################################################################################################
#    Settings
####################################################################################################

# Name of the test executable, relative to this file:
test_exe="../bin/test/genesis_tests"

# Mode to use when no other provided:
default_mode="validation"

# Default number of speed runs:
speed_runs=25

# Output directory for failed tests:
out_dir="out"

# Use *'s around each test filter:
loose_filtering=0

# Make it work on non-english systems:
LC_NUMERIC="en_US.UTF-8"

####################################################################################################
#    Initialization
####################################################################################################

# Change to test dir. This ensures that the script can be called from any directory.
cd "$(dirname "$0")"

# Cannot test without test suite!
if [ ! -f "${test_exe}" ] ; then
    echo "Test executable '${test_exe}' not found."
    echo "Run the build process first!"
    exit 1
fi

# This script needs bc.
if [ -z "`which bc`" ] ; then
    echo "Program 'bc' not found. Cannot run this script."
    exit 1
fi

# Check for some simple commands.
if [[ $1 == "list" ]] ; then
    # Simply output all available tests and exit.
    ${test_exe} --gtest_list_tests
    exit
fi
if [[ $1 == "clean" ]] ; then
    # Remove output files.
    rm -rf ${out_dir}
    exit
fi

# Get run mode from command line.
# Also, shift away any used command line parameters.
mode="none"
if [[ $1 == "validation" || $1 == "validate" || $1 == "val" ]] ; then
    mode="validation"
    shift
fi
if [[ $1 == "memory" || $1 == "mem" ]] ; then
    mode="memory"
    shift
    if [ -z "`which valgrind`" ] ; then
        echo "Program 'valgrind' not found. Cannot run memory tests."
        exit 1
    fi
fi
if [[ $1 == "heap" || $1 == "massif" ]] ; then
    mode="heap"
    shift
    if [ -z "`which valgrind`" ] ; then
        echo "Program 'valgrind' not found. Cannot run memory tests."
        exit 1
    fi
fi
if [[ $1 == "call" || $1 == "callgrind" || $1 == "cachegrind" ]] ; then
    mode="call"
    shift
    if [ -z "`which valgrind`" ] ; then
        echo "Program 'valgrind' not found. Cannot run memory tests."
        exit 1
    fi
fi
if [[ $1 == "speed" ]] ; then
    mode="speed"
    shift

    # Overwrite number of runs if set.
    if [[ ${1} =~ ^[0-9]+$ ]] ; then
        speed_runs=$1
        shift
    fi
    if [[ $speed_runs -gt 1000 ]]; then
        speed_runs=999
    fi
fi
if [[ $1 == "gtest" ]] ; then
    mode="gtest"
    shift
fi
if [[ $1 == "debug" || $1 == "dbg" || $1 == "gdb" ]] ; then
    mode="debug"
    shift

    if [ -z "`which gdb`" ] ; then
        echo "Program 'gdb' not found. Cannot run this script with mode debug."
        exit 1
    fi
fi
if [[ $mode == "none" ]] ; then
    # If the mode was not set by any previous lines,
    # use default.
    mode=${default_mode}
fi

####################################################################################################
#    Setup
####################################################################################################

# Turn the rest of the command line arguments into a gtest filter.
# If filtering is set to loose, add * around each argument.
if [[ $# == 0 ]]; then
    filter="*"
elif [[ ${loose_filtering} == 1 ]]; then
    filter=$(printf ":*%s*" "$@")
    filter=${filter:1}
else
    filter=$(printf ":%s" "$@")
    filter=${filter:1}
fi

# Special relay mode gtest. Forward the filters, call gtest, then exit.
if [[ $mode == "gtest" ]] ; then
    ${test_exe} --gtest_filter="${filter}"
    exit
fi

# Other special mode debug. Forward the filters, and run gdb on the gtest program.
if [[ $mode == "debug" ]] ; then
    # When we are on GitHub Actions, we want a fully automated debugging, print the stack, then exit.
    if [[ `whoami` == "runner" ]] ; then
        echo "Running gdb auto"
        gdb -ex "set confirm off" -iex "set pagination off" -ex "run" -ex "bt" -ex "q" --args ${test_exe} --gtest_catch_exceptions=0 --gtest_filter="${filter}"
    else
        gdb -ex "run" -ex "bt" --args ${test_exe} --gtest_catch_exceptions=0 --gtest_filter="${filter}"
    fi
    exit
fi

# Get number of tests to run.
num_tests=`${test_exe} --gtest_list_tests --gtest_filter="${filter}" | grep -v "\.$" | wc -l`

# Names of the temp output files.
gtest_out_file="gtest_tmp"
valgrind_out_file="valgrind_tmp"

# Set marker for how many tests failed.
# This variable is returned as exit status of the script.
fail_count=0

####################################################################################################
#    Test Run Functions
####################################################################################################

# Run a simple validation test.
# Takes the test case as input, returns 0 if successfull.
function run_validation() {
    outfile=${gtest_out_file}
    ${test_exe} --gtest_filter=${1} >& ${outfile}
    success=$?

    return ${success}
}

# Run a full memory check.
# Takes the test case as input, returns 0 if successfull.
function run_memory() {
    outfile=${valgrind_out_file}
    # more verbose if needed: --track-origins=yes --show-leak-kinds=all
    valgrind --tool=memcheck --leak-check=full --log-file=${outfile} ${test_exe} --gtest_filter=${1} > /dev/null

    deflost=`fgrep "definitely lost:" ${outfile} | xargs | cut -d' ' -f4 | sed "s/,//g"`
    indlost=`fgrep "indirectly lost:" ${outfile} | xargs | cut -d' ' -f4 | sed "s/,//g"`
    reachable=`fgrep "still reachable:" ${outfile} | xargs | cut -d' ' -f4 | sed "s/,//g"`
    errors=`fgrep "Invalid" ${outfile}`

    if [ -z "${deflost}" ]; then deflost=0; fi
    if [ -z "${indlost}" ]; then indlost=0; fi
    if [ -z "${reachable}" ]; then reachable=0; fi
    if [ -z "${errors}" ]; then errors=0; else errors=1; fi

    # Special case to take care of openmp not freeing until the very tear down,
    # see https://stackoverflow.com/a/6973510/4184258 for details.
    if [[ ${reachable} == 8 ]]; then
        reachable=0
    fi

    # Return 0 only if nothing bad happend.
    return $((deflost+indlost+reachable+errors))
}

# Run a valgrind massif heap check.
# Takes the test case as input, returns 0 if successfull.
function run_heap() {
    outfile=${valgrind_out_file}
    valgrind --tool=massif --log-file=${outfile} ${test_exe} --gtest_filter=${1} > /dev/null

    # Return 0 only if nothing bad happend.
    return 0
}

# Run a valgrind callgrind.
# Takes the test case as input, returns 0 if successfull.
function run_call() {
    outfile=${valgrind_out_file}
    valgrind --tool=callgrind --log-file=${outfile} ${test_exe} --gtest_filter=${1} > /dev/null

    # Return 0 only if nothing bad happend.
    return 0
}

# Run the test `speed_runs` times and show execution times.
# Takes the test case as input, returns 0 if successfull.
function run_speed() {
    outfile=${gtest_out_file}

    local min=0 max=0 sum=0 avg
    local s_time e_time

    printf ' %.0s' {1..48}
    for i in $(seq 1 ${speed_runs}); do
        printf "%3u/%-3u" ${i} ${speed_runs}

        # Run test and measure time.
        s_time=`date +%s%N`
        ${test_exe} --gtest_filter=${1} >& ${outfile}
        success=$?
        e_time=`date +%s%N`

        # Set timing counters.
        duration=`echo "scale=3;(${e_time} - ${s_time})/(10^06)" | bc`
        if [[ ${max} == 0 ]]; then
            min=${duration}
            max=${duration}
        else
            if [[ `echo "${duration} > ${max}" | bc` == 1 ]]; then
                max=${duration}
            fi
            if [[ `echo "${duration} < ${min}" | bc` == 1 ]]; then
                min=${duration}
            fi
        fi
        sum=`echo "${sum} + ${duration}" | bc`
        printf "\b\b\b\b\b\b\b"

        # Break if the test failed. We do not need to repeat it then.
        if [[ ${success} != 0 ]]; then
            break
        fi
    done
    printf ' %.0s'  {1..7}
    printf '\b%.0s' {1..7}
    printf '\b%.0s' {1..48}

    # Print execution time summaries.
    if [[ ${success} == 0 ]]; then
        avg=`echo "scale=3;${sum}/${speed_runs}" | bc`
    else
        avg=`echo "scale=3;${sum}/${i}" | bc`
    fi
    printf "   \e[32m% 10.3fms\e[0m" ${min}
    printf "   \e[32m% 10.3fms\e[0m" ${max}
    printf "   \e[32m% 10.3fms\e[0m" ${avg}

    return ${success}
}

####################################################################################################
#    Header
####################################################################################################

# Show some information about the run.
echo -en "Running \e[33m${mode}\e[0m test on \e[33m${num_tests}\e[0m test cases"
if [[ ${mode} == "speed" ]] ; then
    echo -en " with \e[33m${speed_runs}\e[0m replicates each"
fi
echo -en ".\n\n"

function print_table_header() {
    printf "   Test   "
    printf "   \e[90m%-8s\e[0m"  "Start"
    printf "   \e[34m%-60s\e[0m" "Test Case"
    if [[ ${mode} == "speed" ]] ; then
        printf "   \e[32m   Min      \e[0m"
        printf "   \e[32m   Max      \e[0m"
        printf "   \e[32m   Avg      \e[0m"
    else
        printf "     Time      "
    fi
    printf "   Result\n"
}
function print_table_line() {
    if [[ ${mode} == "speed" ]] ; then
        echo -en "----------- ---------- ----------------------------------------------------------"
        echo -en "---- -------------- -------------- -------------- ----------\n"
    else
        echo -en "----------- ---------- ----------------------------------------------------------"
        echo -en "---- -------------- ----------\n"
    fi
}

print_table_header
print_table_line

####################################################################################################
#    Main Loop
####################################################################################################

# Init counters.
total_s_time=`date +%s%N`
i_test=1

# Parse Gtest test case list and run function for the mode.
for line in `${test_exe} --gtest_list_tests --gtest_filter="${filter}"` ; do
    # Get the current test id.
    if [[ $line == *. ]] ; then
        test_case=${line}
        continue
    fi
    test_name=${line}
    test_id=${test_case}${test_name}

    # Print general information about the current test.
    printf "   %3u/%-3u"         ${i_test} ${num_tests}
    printf "   \e[90m%s\e[0m"    $(date +"%H:%M:%S")
    printf "   \e[34m%-60s\e[0m" ${test_id}

    # Run the appropriate mode.
    run_s_time=`date +%s%N`
    if [[ ${mode} == "validation" ]] ; then
        run_validation $test_id 2>> ${gtest_out_file}
        success=$?
    fi
    if [[ ${mode} == "memory" ]] ; then
        run_memory $test_id 2>> ${valgrind_out_file}
        success=$?
    fi
    if [[ ${mode} == "heap" ]] ; then
        run_heap $test_id 2>> ${valgrind_out_file}
        success=$?
    fi
    if [[ ${mode} == "call" ]] ; then
        run_call $test_id 2>> ${valgrind_out_file}
        success=$?
    fi
    if [[ ${mode} == "speed" ]] ; then
        run_speed $test_id 2>> ${gtest_out_file}
        success=$?
    fi

    # Print timing summary.
    run_e_time=`date +%s%N`
    run_duration=`echo "scale=3;(${run_e_time} - ${run_s_time})/(10^06)" | bc`
    if [[ ${mode} == "validation" ]] ; then
        printf "   % 10.3fms" ${run_duration}
    elif [[ ${mode} == "memory" || ${mode} == "heap" ]] ; then
        run_duration=`echo "scale=3;(${run_duration})/(10^03)" | bc`
        printf "   % 10.3fs " ${run_duration}
    fi

    # Print success summary.
    if [[ ${success} == 0 ]]; then
        printf "   \e[32mpassed\e[0m"
        rm ${outfile}
    else
        printf "   \e[31mfailed\e[0m"

        # Increase the failed counter within the limits of legal exit codes.
        ((fail_count++))
        if [[ ${fail_count} -gt 255 ]]; then
            fail_count=255
        fi

        # Move output files to a place where it can be read later.
        mkdir -p ${out_dir}
        mv -f ${outfile} ${out_dir}/${outfile/tmp/}${test_id/./_}.log
        if [ -f core ]; then
            mv -f core ${out_dir}/${outfile/tmp/}${test_id/./_}.core
        fi
        if [ -f valgrind_tmp.core.* ]; then
            ls valgrind_tmp.core.* | xargs -I corefile mv -f corefile ${out_dir}/${outfile/tmp/}${test_id/./_}.core
        fi
    fi

    ((i_test++))
    printf "\n"
done

####################################################################################################
#    Footer
####################################################################################################

# Calculate total summary.
total_e_time=`date +%s%N`
total_time=`echo "scale=3;(${total_e_time} - ${total_s_time})/(10^09)" | bc`
success_count=$((${num_tests} - ${fail_count}))

# Print total summary.
print_table_line
echo -e "\n  Total time: \e[33m${total_time}s\e[0m"
printf "Passed tests: \e[32m%3u/%-3u\e[0m\n"   ${success_count} ${num_tests}
printf "Failed tests: \e[31m%3u/%-3u\e[0m\n\n" ${fail_count} ${num_tests}

echo -n "Total result: "
if [[ ${fail_count} == 0 ]]; then
    echo -e "\e[32mPASSED\e[0m"
else
    echo -e "\e[31mFAILED\e[0m"
fi

exit ${fail_count}
