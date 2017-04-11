Build Process {#supplement_build_process}
===========

This page contains more details about the build process of Genesis. It is intended for
troubleshooting and for developers who want to contribute to the Genesis code base.

# Overview @anchor supplement_build_process_overview

We use CMake as build system. The `CMakeLists.txt` in the main directory contains basic
information about the whole project. It further calls the following sub-build-scripts:

 *  `lib/genesis/CMakeLists.txt`: main build script for the library.
 *  `apps/CMakeLists.txt`: compiles small applications using Genesis.
 *  `python/CMakeLists.txt`: if [Pybind11](https://github.com/pybind/pybind11)
    is available, this script compiles the genesis python module.
 *  `test/CMakeLists.txt`: if [Google Test](https://github.com/google/googletest) is available,
    this script compiles the Genesis tests. See below for details.

For ease of development, the source files that need to be compiled are included using CMake's
glob file search. This however means that added or removed source files (such as a newly created
class) are not found once CMake is run. In order to still support incremental builds (that is,
compile only new or changed files), there is also a `Makefile`. It has a target `update`, which
prompts CMake to update its file list.

Thus, the recommended way of building Genesis is to use the `Makefile`:

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

You can also run the CMake process manually like this:

~~~{.sh}
# In genesis main directory.

# Create build scripts.
mkdir build
cd build/
cmake .. [options]

# Run the build process.
make
~~~

This is for example necessary when you want to customize the CMake options.

# Build Output @anchor supplement_build_process_build_output

The following binaries are created:

 *  `genesis/bin/libgenesis.[so|a]`: Shared and static binaries of the library. Those are the files
    you need to link against when using Genesis as a library for your own C++ projects.
    By default, only the shared lib (`.so`) is build. If you need the static lib, use the option
    flag `BUILD_STATIC_LIB` of the main CMake script.
 *  `genesis/bin/apps/*`: App executables are stored here. See @ref setup_apps for details.
<!--
 *  `genesis/bin/python/genesis.so`: Python module file. See section @ref setup_python.
-->
 *  `genesis/bin/test/genesis_tests`: Test executable. See `genesis/test/README.md` for more information.
 *  `genesis/build`: Intermediate build files. No need to worry about them too much.

# Release vs Debug Version @anchor supplement_build_process_release_vs_debug

By default, Genesis is compiled in release mode. The main `CMakeLists.txt` however checks the value
of the environment variable `GENESIS_DEBUG` and, if set, compiles in debug mode.

So, for development, adding the line

    export GENESIS_DEBUG=1

to the terminal startup script (e.g., `.bash_rc` in the users home directory) will compile Genesis
in debug mode.

# Unity Build @anchor supplement_build_unity_build

By default, in release mode, Genesis builds as a so called unity build. That means, all source
files are combined into one big file that is then compiled in one go. See for example
https://en.wikipedia.org/wiki/Single_Compilation_Unit for more details.

The purpose of this is twofold: Faster total compilation time, and better program optimization.
The downside is that this needs more memory when compiling. Thus, we also offer ways to disable
this and instead build smaller units, using less memory.

There are three modes:

  1. `FULL`: Default, compile everything in one unit.
  2. `MODULES`: Divide the code into modules (corresponding to the main Genesis
     [Namespaces](namespaces.html)). This is a somewhat in-between solution, if your computer does
     not have enough memory for the full build, but you still want to try to get best program
     optimization.
  3. `OFF`: Every `cpp` file is compiled on its own. This takes longest for a fresh build but needs
     the least memory. It is also used for the debug build, because when working on files, it is
     still faster to just compile them, instead of fully compiling everything.

You can change those modes via the CMake option `UNITY_BUILD`.
See the @ref supplement_build_process_overview "Overview" for how to explicitly build Genesis using
Cmake, and replace the `cmake ..` line by

~~~{.sh}
cmake .. -DUNITY_BUILD=MODULE
~~~

where, instead of `MODULE`, you can also use `OFF`.

# Supported Compilers @anchor supplement_build_process_supported_compilers

Genesis uses C++11, thus we need a compiler that supports all its features.

The first GCC version with all necessary features (including `std::regex`) is 4.9, see
[here](https://gcc.gnu.org/projects/cxx-status.html#cxx11) and
[here](https://gcc.gnu.org/ml/gcc/2014-04/msg00195.html). Compiling with GCC 4.8 should work,
and most features of Genesis can be used, too. However, this throws an exception when trying to call
any function that uses `std::regex`.

Clang supports C++11 since version 3.3, see [here](https://clang.llvm.org/cxx_status.html).
However, we currently test it with Clang 3.6 and 3.7 only. You are welcome to report any issues
with other Clang versions to the [GitHub Issues Page](https://github.com/lczech/genesis/issues).

Make sure that your `libstdc++` is also up to date. This is the standard library for C++, which is
heavily used in Genesis. It is usually installed with `g++`. However, `clang++` uses the `g++`
version of `libstdc++`, so if you have a new `clang++`, but old `g++` version, this might cause
trouble. Best to update both of them.

# Testing Genesis with GTest @anchor supplement_build_process_testing_with_gtest

When building Genesis in @ref supplement_build_process_release_vs_debug "debug mode", testing is
automatically activated. In release mode, the CMake option `BUILD_TESTS` has to be set to `ON`
manually. See `genesis/CMakeLists.txt` for details.

We use [GTest](https://github.com/google/googletest) for testing its components. The test binary is
located in `genesis/bin/test/genesis_tests`. It can be run directly.

We also offer an extended interface to GTest by using `genesis/test/run.sh`. This script
encapsulates GTest and offers several extra options like speed testing and memory testing with
[Valgrind](http://valgrind.org/). See `genesis/test/README.md` for details.

# Installing and finding GTest @anchor supplement_build_process_installing_gtest

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
