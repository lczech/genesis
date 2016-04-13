Development {#dev}
===========

This page contains information for developers of genesis and those who want to contribute to
its code base.

## Build Process

We use cmake as build system. The `CMakeLists.txt` in the main directory contains basic
information about the whole project. It further calls the following sub-build-scripts:

 *  `lib/CMakeLists.txt`: main build script for the library.
 *  `apps/CMakeLists.txt`: compiles small applications using genesis.
 *  `python/src/CMakeLists.txt`: if
    [Boost Python](http://www.boost.org/doc/libs/1_60_0/libs/python/doc/html/index.html)
    is available, this script compiles the genesis python module.
 *  `test/src/CMakeLists.txt`: if [Google Test](https://github.com/google/googletest) is available,
    this script compiles the genesis tests.

For ease of development, the source files that need to be compiled are included using cmake's
glob file search. This however means that added or removed source files (such as a newly created
class) are not found once cmake is run. In order to still support incremental builds (that is,
compile only new or changed files), there is also a `Makefile`. It has a target `update`, which
prompts cmake to update its file list.

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

This way, cmake does not have to be invoked directly.

## Release vs Debug version

By default, genesis is compiled in release mode. The main `CMakeLists.txt` however checks the value
of the environment variable `GENESIS_DEBUG` and, if set, compiles in debug mode.

So, for development, adding the line

    export GENESIS_DEBUG=1

to the terminal startup script (e.g., `.bash_rc` in the users home directory) will compile genesis
in debug mode.
