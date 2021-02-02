Build Process {#supplement_build_process}
===========

This page contains more details about the build process of Genesis. It is intended for
troubleshooting and for developers who want to contribute to the Genesis code base.

# Overview @anchor supplement_build_process_overview

We use CMake as build system generator. The `CMakeLists.txt` in the main directory contains basic
information about the whole project. It further calls the following sub-build-scripts:

 *  `lib/genesis/CMakeLists.txt`: main build script for the library.
 *  `apps/CMakeLists.txt`: compiles small applications using Genesis.
 *  `python/CMakeLists.txt`: if [Pybind11](https://github.com/pybind/pybind11)
    is available, this script compiles the genesis python module.
 *  `test/CMakeLists.txt`: if [Google Test](https://github.com/google/googletest) is available,
    this script compiles the Genesis tests. See below for details.

For ease of development, the source files that need to be compiled are included using CMake's
glob file search. This however means that added or removed source files (such as a newly created
class) are not found once CMake has run once. In order to still support incremental builds (that is,
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

This is for example necessary when you want to customize the CMake options, such as deactivating
features. We however do not recommend to do this (e.g., deactivate OpenMP or zlib), as this reduces
the functionality and speed of genesis.

# Build Output @anchor supplement_build_process_build_output

The following binaries are created:

 *  `genesis/bin/libgenesis.[so|a]`: Shared and static binaries of the library. Those are the files
    you need to link against when using Genesis as a library for your own C++ projects.
    See @ref supplement_build_process_shared_vs_static "Shared vs Static Library" for details.
 *  `genesis/bin/apps/*`: App executables are stored here. See @ref setup_apps for details.
<!--
 *  `genesis/bin/python/genesis.so`: Python module file. See section @ref setup_python.
-->
 *  `genesis/bin/test/genesis_tests`: Test executable. See `genesis/test/README.md` for more information.
 *  `genesis/build`: Intermediate build files. No need to worry about them too much.

# Shared vs Static Library @anchor supplement_build_process_shared_vs_static

For the general setup of Genesis as a library, see @ref setup_lib.
If Genesis is used stand-alone, only the shared lib (`.so`) is build by default.
If it is used as a library (i.e., if it is used via CMake's `add_subdirectory`),
we build the static lib (`.a`) by default.

The reasoning behind this is as follows: As stand-alone, Genesis is most likely to be used with
its @ref setup_apps setup, where the apps are probably staying in one place, so linking against a
shared object makes sense to share resources. However, when used a dependency/library, Genesis is
just one among many tools that the master program wants to use, and in order to make it possible
to move the final binary around, it makes sense to link statically.

In order to explicitly change those defaults, use the CMake options `GENESIS_BUILD_SHARED_LIB` and
`GENESIS_BUILD_STATIC_LIB`. For example, in your `CMakeLists.txt`, you can force these options via:

~~~{.cmake}
SET( GENESIS_BUILD_SHARED_LIB OFF CACHE BOOL "Build the shared library version of Genesis." )
SET( GENESIS_BUILD_STATIC_LIB ON  CACHE BOOL "Build the static library version of Genesis." )
~~~

This is unfortunately a bit cumbersome, but it's CMake's way of changing options from a parent
scope...

Linking against those libraries can still be done in CMake via:

~~~{.cmake}
target_link_libraries ( YourTarget ${GENESIS_LINK_LIBRARIES} )
~~~

which is set to either the shared or static library, depending on which is being build, and
on whether Genesis is build stand-alone or as a dependency/library (see above).

Alternatively, if you build both library versions and need to explicitly decide which one to link
against, you can use:

~~~{.cmake}
# Either use this...
target_link_libraries ( YourTarget ${GENESIS_LINK_SHARED_LIBRARIES} )

# ... or this.
target_link_libraries ( YourTarget ${GENESIS_LINK_STATIC_LIBRARIES} )
~~~

This should however rarely be necessary.

# Release vs Debug Version @anchor supplement_build_process_release_vs_debug

By default, Genesis is compiled in release mode. The main `CMakeLists.txt` however checks the value
of the environment variable `GENESIS_DEBUG` and, if set, compiles in debug mode.

So, for development, adding the line

    export GENESIS_DEBUG=1

to the terminal startup script (e.g., `.bash_rc` in the users home directory) will compile Genesis
in debug mode (with incremental builds, and with tests) automatically all the time. This is what
we do for development. It is a hack, but it is convenient!

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
     still faster to just compile them incrementally, instead of fully compiling everything.

You can change those modes via the CMake option `GENESIS_UNITY_BUILD`.
See the @ref supplement_build_process_overview "Overview" for how to explicitly build Genesis using
Cmake, and replace the `cmake ..` line by

~~~{.sh}
cmake .. -DGENESIS_UNITY_BUILD=MODULE
~~~

where, instead of `MODULE`, you can also use `OFF`.

# Supported Compilers @anchor supplement_build_process_supported_compilers

Genesis uses C++11, thus we need a compiler that supports all its features.
We recommend gcc >= 5 or clang >= 3.9.

## GCC @anchor supplement_build_process_supported_compilers_gcc

The first gcc version with all necessary features is gcc 5.
Before that, gcc 4.8 and gcc 4.9 were feature-complete for C++11, see
[here](https://gcc.gnu.org/projects/cxx-status.html#cxx11) and
[here](https://gcc.gnu.org/ml/gcc/2014-04/msg00195.html).
Unfortunately, the STL was not feature-complete in these versions, in particular,
`std::regex`, `std::get_time`, and `std::put_time`, which are used in some functions of genesis.
Hence, we recommend at least gcc 5.
Compiling with gcc 4.8 and 4.9 should still work, and most features of Genesis can be used, too.
However, this throws an exception when trying to call any function that uses `std::regex`,
`std::get_time`, and `std::put_time` (which are just a few).

## Clang @anchor supplement_build_process_supported_compilers_clang

Clang supports C++11 since version 3.3, see [here](https://clang.llvm.org/cxx_status.html).
We currently only test it with later versions though. You are welcome to report any issues
with other Clang versions to the [GitHub Issues Page](https://github.com/lczech/genesis/issues).

**Clang and OpenMP**. Many (older) versions of clang have issues with finding OpenMP. If this causes issues,
you can either deactivate OpenMP via the CMake options (not recommended), or install `libiomp-dev`
(and potentially `libomp5`) before compiling. See below for more on OpenMP,
and also see the `.travis.yml` CI file in the main genesis directory for some more hints.

<!-- ## Remarks on the STL

Make sure that your `libstdc++` is also up to date. This is the standard library for C++, which is
heavily used in Genesis. It is usually installed with `g++`. However, some versions of `clang++` seem
to use the `g++` version of `libstdc++`, so if you have a new `clang++`, but old `g++` version,
this might cause trouble. Best to update both of them. -->

# Dependencies @anchor supplement_build_process_dependencies

Genesis does not have any external library dependencies, with the exception of a few basics:
OpenMP and zlib. We recommend to get both working, for full functionality of genesis.
If you want/need to deactivate any of them nonetheless, use the above manual CMake setup
as explained in @ref supplement_build_process_overview, and provide the following flags:

```
cmake -DGENESIS_USE_OPENMP=OFF -DGENESIS_USE_ZLIB=OFF ..
```

to deactivate both. Adapt to your needs if only one needs to be deactivated.

## OpenMP @anchor supplement_build_process_openmp

To enable genesis to deal with large amounts of data, we need thread parallelization. To this end,
we use [OpenMP](https://en.wikipedia.org/wiki/OpenMP), and highly recommend to get this running
if you compile genesis.

Mostly, this should work out of the box with gcc on Linux systems.
With clang, as mentioned above, you might first need to install `libiomp-dev`
(and potentially `libomp5`) before CMake can find the OpenMP libraries.

On maOS (osx), this is more complicated, as the built-in AppleClang compiler does not support OpenMP.
Hence, we need custom Clang versions and the OpenMP libraries:

```
brew install llvm libomp
```

Furthermore, we then need to set the path to the compiler, so that it can be found by CMake
when compiling genesis:

```
export PATH="$(brew --prefix llvm)/bin:$PATH"
export CC="/usr/local/opt/llvm/bin/clang"
export CXX="$(CC)++"
export CFLAGS="-I /usr/local/include -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/"
export CXXFLAGS=${CFLAGS}
export CPPFLAGS=${CFLAGS}
export LDFLAGS="-L /usr/local/lib -L/usr/local/opt/llvm/lib"
```

For further information, try these resources:

 * https://iscinumpy.gitlab.io/post/omp-on-high-sierra/
 * https://embeddedartistry.com/blog/2017/02/24/installing-llvm-clang-on-osx/

Good luck, and please let us know if you have any trouble with this!

## zlib @anchor supplement_build_process_zlib

Genesis uses zlib to support working with gz-compressed files. This should mainly work out of the
box on both Linux and macOS systems. However, occasionally, you might need to install `zlib1g-dev`
first.

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
   It has the additional advantage that it compiles GTest with the same flags used for Genesis.

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
