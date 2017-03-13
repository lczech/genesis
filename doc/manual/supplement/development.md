Development {#supplement_development}
===========

This page contains information for developers of genesis and those who want to contribute to
its code base.

# Build Process

We use CMake as build system. The `CMakeLists.txt` in the main directory contains basic
information about the whole project. It further calls the following sub-build-scripts:

 *  `lib/genesis/CMakeLists.txt`: main build script for the library.
 *  `apps/CMakeLists.txt`: compiles small applications using genesis.
 *  `python/CMakeLists.txt`: if
    [Boost Python](http://www.boost.org/doc/libs/1_60_0/libs/python/doc/html/index.html)
    is available, this script compiles the genesis python module.
 *  `test/CMakeLists.txt`: if [Google Test](https://github.com/google/googletest) is available,
    this script compiles the genesis tests. See below for details.

For ease of development, the source files that need to be compiled are included using CMake's
glob file search. This however means that added or removed source files (such as a newly created
class) are not found once CMake is run. In order to still support incremental builds (that is,
compile only new or changed files), there is also a `Makefile`. It has a target `update`, which
prompts CMake to update its file list.

Thus, the recommended way of building genesis is to use the `Makefile`:

~~~{.sh}
# In genesis main directory.

# Initial build:
make

# Incremental build (after adding or removing files):
make update

# Clean all build files:
make clean
~~~

This combines both the CMake configure and build phase and is intended for quick development.

# Release vs Debug version

By default, genesis is compiled in release mode. The main `CMakeLists.txt` however checks the value
of the environment variable `GENESIS_DEBUG` and, if set, compiles in debug mode.

So, for development, adding the line

    export GENESIS_DEBUG=1

to the terminal startup script (e.g., `.bash_rc` in the users home directory) will compile genesis
in debug mode.

# Compilers

Genesis uses c++11, thus we need a compiler that supports all features.
The first GCC version to do so is 4.8.1, see [here](https://gcc.gnu.org/projects/cxx-status.html#cxx11).

# Testing Genesis with GTest

When building Genesis in debug mode, testing is automatically activated. In release mode, the CMake
option `BUILD_TESTS` has to be set to `ON` manually. See `genesis/CMakeLists.txt` for details.

We use [GTest](https://github.com/google/googletest) for testing its components. The test binary is
located in `genesis/bin/test/genesis_tests`. It can be run directly.

We also offer an extended interface to GTest by using `genesis/test/run.sh`. This script
encapsulates GTest and offers several extra options like speed testing and memory testing with
[Valgrind](http://valgrind.org/). See `genesis/test/README.md` for details.

# Installing and finding GTest

When building Genesis tests, we try a couple of ways to find GTest:

 * First, we check whether it is locally installed in `genesis/tools/googletest/`.
   This enables us to use GTest on systems that do not allow to install it globally.
 * If it is not found there, we check for a global installation.
 * If this fails too, we try to download it and build it from scratch.
   This is particularly useful for running the tests in continuous integration.
   It has the big advantage that it compiles GTest with the same flags used for Genesis.

See `genesis/test/CMakeLists.txt` for details of this process.

In order to install GTest locally for testing Genesis, follow these steps:

 * Get GTest from [https://github.com/google/googletest](https://github.com/google/googletest)
 * Extract it somewhere and `cd` to there
 * GTest < 1.8:
   ~~~{.sh}
   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=path/to/genesis/tools/googletest ..
   make
   make install
   ~~~
 * GTest >= 1.8:
   ~~~{.sh}
   cmake -DBUILD_SHARED_LIBS=ON .
   make
   cp -a include/gtest/ path/to/genesis/tools/googletest/include
   cp -a libgtest_main.so libgtest.so path/to/genesis/tools/googletest/lib
   ~~~

After this, it should be found by CMake when building the tests.
