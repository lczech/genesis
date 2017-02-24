Genesis test suite
==================

Genesis uses [GTest](https://github.com/google/googletest) for testing its components.
The test suite is build using the standard build process of Genesis in the main directory.

## Structure

This directory contains everything needed for testing Genesis:

 * `./bin` contains the test binary `test_suite`, which uses GTest and can be called directly.
 * `./data` contains data that the tests can use for input/output verification.
 * `./out` is created to examine failed tests when the  `run.sh` script is used.
 * `./src` contains the sources files for defining tests. New tests are added here.

## Run script

The script `run.sh` is a small wrapper around the GTest framework which allows for some additional
test modes. The basic usage is

    ./run.sh [mode] [filter]...

Currently, it has three main modes:

1. `validation` (or short `val`) simply runs GTest tests. If no mode is provided, this one is used.
2. `memory` (or short `mem`) runs a full [Valgrind](http://valgrind.org/) memory check on the tests.
3. `speed` runs the tests several times and measures their execution speed. If immediately followed
   by an integer, this number of speed runs is executed (e.g., `./run.sh speed 100`).

Those modes will output a list of all executed tests with some additional information.

### Filtering

GTest provides an easy way of choosing which tests to run by providing filters. Those are forwarded
when given after the mode command of the `run.sh` script:

 * `./run.sh tree*` runs the validation on all tests starting with "tree..."
 * `./run.sh mem tree* *newick*` runs a memory check on all tests either starting with "tree..."
   or containing the string "newick".

### Test Output

When using the `run.sh` script, only a summary of the tests is showed on screen. In order to
examine failed tests, more information is usually needed. Thus, for every failed test, a file is
written to `./out` containing the output of the used program:

 * A failed GTest run (using modes `validation` or `speed`) will be named `./out/gtest_test_name`.
 * A failed Valgrind run (using mode `memory`) will be named `./out/valgrind_test_name`.

where the suffix `test_name` is replaced by the test name itself.

### Additional Modes

For convenience, the `run.sh` script also contains some additional modes:

 * `./run.sh list` lists all available test cases.
 * `./run.sh gtest [filter]...` calls GTest directly, using the filtering as described above.
 * `./run.sh clean` removes the `out` directory.

## Installing and finding GTest

When building Genesis, we try a couple of ways to link the test binary with GTest:

 * First, we check whether it is locally installed in `googletest/` in the genesis `test` directory.
   This enables us to use GTest on systems that do not allow to install it globally.
 * If it is not found there, we check for a global installation.
 * If this fails too, we try to download it and build it from scratch.
   This is particularly useful for running the tests in continuous integration.
   It has the big advantage that it compiles GTest with the same flags used for Genesis.

See `./CMakeLists.txt` for details of this process.

In order to install it locally for Genesis, follow these steps:

 * Get GTest from [https://github.com/google/googletest](https://github.com/google/googletest)
 * Extract it somewhere and `cd` to there
 * GTest < 1.8:
   ~~~{.sh}
   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=path/to/Genesis/test/googletest ..
   make
   make install
   ~~~
 * GTest >= 1.8:
   ~~~{.sh}
   cmake -DBUILD_SHARED_LIBS=ON .
   make
   cp -a include/gtest/ path/to/Genesis/test/googletest/include
   cp -a libgtest_main.so libgtest.so path/to/Genesis/test/googletest/lib
   ~~~
