genesis test suite
==================

Genesis uses [Gtest](https://github.com/google/googletest) for testing its components.
The test suite is build using the standard build process of genesis in the main directory.

## Structure

This directory contains everything needed for testing genesis:

* `./bin` contains the test binary `test_suite`, which uses Gtest and can be called directly.
* `./data` contains data that the tests can use for input/output verification.
* `./out` is created to examine failed tests when the  `run.sh` script is used.
* `./src` contains the sources files for defining tests. New tests are added here.

## Run script

The script `run.sh` is a small wrapper around the Gtest framework which allows for some additional
test modes. The basic usage is

    ./run.sh [mode] [filter]...

Currently, it has three main modes:

1. `validation` (or short `val`) simply runs Gtest tests. If no mode is provided, this one is used.
2. `memory` (or short `mem`) runs a full [Valgrind](http://valgrind.org/) memory check on the tests.
3. `speed` runs the tests several times and measures their execution speed. If immediately followed
   by an integer, this number of speed runs is executed (e.g., `./run.sh speed 100`).

Those modes will output a list of all executed tests with some additional information.

### Filtering

Gtest provides an easy way of choosing which tests to run by providing filters. Those are forwarded
when given after the mode command of the `run.sh` script:

* `./run.sh tree*` runs the validation on all tests starting with "tree..."
* `./run.sh mem tree* *newick*` runs a memory check on all tests either starting with "tree..."
  or containing the string "newick".

### Test Output

When using the `run.sh` script, only a summary of the tests is showed on screen. In order to
examine failed tests, more information is usually needed. Thus, for every failed test, a file is
written to `./out` containing the output of the used program:

* A failed Gtest run (using modes `validation` or `speed`) will be named `./out/gtest_test_name`.
* A failed Valgrind run (using mode `memory`) will be named `./out/valgrind_test_name`.

where the suffix `test_name` is replaced by the test name itself.

### Additional Modes

For convenience, the `run.sh` script also contains some additional modes:

* `./run.sh list` lists all available test cases.
* `./run.sh gtest [filter]...` calls Gtest directly, using the filtering as described above.
* `./run.sh clean` removes the `out` directory.
