Setup {#setup}
============

This page contains information about how to set up and use genesis.

# Overview {#setup_overview}

The functionality of genesis can be used in two ways:

 *  As a C++ library.
 *  As a Python module.

The code is written as a C++11 library. It can thus be used in custom projects by including
the necessary headers and linking against the binary (either shared or static, see section
@ref setup_build).

A common use case of genesis are small programs for a certain limited purpose, e.g., project related
analyses. Usually, scripts are used for such purposes. However, C++ is not a scripting language.
In order to still provide a quick and easy way for multiple small script-like programs in C++,
we provide the so-called genesis apps. See section @ref setup_apps.

In addition to C++, the classes and functions are (mostly) exported to Python. The genesis module
can be imported and used as any other Python module. See section @ref setup_python.

Currently, we only test and use genesis on Linux systems. It is tested with Ubuntu 12.04 and 14.04,
as well as CentOS 6.3. If you are using a different operating system, you can either try yourself,
or use a virtual machine. In the future, we hope to support more systems out of the box.
Please report any issues, questions or solutions to our
[support forum](http://support.genesis-lib.org/).

# Download {#setup_download}

Genesis is open source software under the [GNU GPL v3 license](http://www.gnu.org/licenses/gpl.html).

To get the source, you can [download the latest release](https://github.com/lczech/genesis/releases).

Alternatively, in order to stay up to date, you can clone the repository. For this, you need to have
[Git](https://git-scm.com/) installed. Then, call

    git clone https://github.com/lczech/genesis.git

in some directory. This will create a sub-directory `genesis`, containing all files. In case new
commits are pushed to the GitHub repository, you can call

    git pull

from within the `genesis` directory to get them. Don't forget to build genesis again after pulling
(see next section for details).
We recommend this method, as this is the easiest way to get new features, bug fixes etc.

Furthermore, if you want to contribute code of your own, you can also
[fork the project on GitHub](https://github.com/lczech/genesis).

# Build {#setup_build}

To build all of genesis, call

    make

in the main directory. This builds all parts of genesis for which the dependencies are fulfilled.

Requirements:

 *  [make](https://www.gnu.org/software/make/) and [cmake](https://cmake.org/) 2.6 or higher.
 *  A fairly up-to-date C++11 compiler. We currently test with [clang++](http://clang.llvm.org/)
    3.6 and 3.7, as well as [g++](https://gcc.gnu.org/) 4.9 and 5.0.

Make sure that your `libstdc++` is also up to date. This is the standard library for C++, which is
heavily used in genesis. It is usually installed with `g++`. However, `clang++` uses the `g++`
version of `libstdc++`, so if you have a new `clang++`, but old `g++` version, this might cause
trouble. Best to update both of them.

Optional:

 *  The Python module requires [Boost Python](www.boost.org/doc/libs/release/libs/python/)
    1.41 or higher. Frankly, this is a huge requirement, but currently this is the way it is.
    You might need to compile Boost Python with the `-fPIC` flag to work properly.
 *  If you also want to build the tests, you need [Google Test](https://github.com/google/googletest).
    We internally run the tests before every release, so you don't need to. Have a look at the
    `./test` directory for more information.

Build output:

 *  `./bin/libgenesis.[so|a]`: Shared and static binaries of the library. Those are the files you
    need to link against when using genesis as a library for your own C++ projects.
    By default, only the shared lib (`.so`) is build. If you need the static lib, use the option
    flag `BUILD_STATIC_LIB` of the main cmake script.
 *  `./bin/*`: App executables are also stored in the bin directory. See section @ref setup_apps.
 *  `./python/module/genesis.so`: Python module file. See section @ref setup_python.
 *  `./test/bin/test_suite`: Test executable. See `./test/README.md` for more information.
 *  `./build`: Intermediate build files. No need to worry about them too much.

If you just pulled new commits from the repository, or added new files to be compiled
(either an app, some lib function, Python bindings, or tests), you can call

    make update

to include those files without need for recompiling everything (a so-called incremental build).
See @ref dev for more information on incremental builds.

# Apps {#setup_apps}

The `./apps` directory is provided for conveniently and quickly developing small C++ applications
which use genesis as their main library. This is a way of using C++ similar to a scripting language.

Reasons to use this method (instead of the genesis Python bindings) include:

 *  If you want to try out the @ref tutorials and @ref demos.
 *  If you quickly want to test an idea or get some experience with genesis.
 *  If Boost Python is not available on your target system.
 *  If not all needed functions of genesis are exported to Python (yet).
 *  If you simply feel more comfortable developing in C++ than in Python.

To create a new application, create a `*.cpp` file in the `./apps` directory and provide it with
a main function:

~~~{.cpp}
int main( int argc, const char* argv[] )
{
	// [Your code here.]
}
~~~

Include all needed headers and write the desired functionality.
Of course, you can use other local functions in addition to the main function in order to structure
your code.

The app is automatically compiled by the main build process and linked against the shared library
version of genesis by calling

    make update

in the main directory. The resulting executable is placed in `./bin` and named after the `*.cpp`
file (without the ending).

Apart from that, there are no further requirements. Simply include the needed genesis headers (or
any other headers you might need) and compile. Happy coding!

# Library {#setup_lib}

The apps are only a convenience for users. Genesis can of course also be used like any
other C++ library by including its headers and linking against its binaries.

If you include genesis in bigger projects which are separately compiled, you need to make sure to
use compatible options for compiling the genesis binaries. For example, if you want to link against
the shared library, the settings for threads (e.g., Pthreads) need to be the same for every
compilation unit. See the main Cmake script for the available compiler options.

# Python {#setup_python}

For users who are more comfortable to develop in Python, we also offer a Python interface.
The module file is created using [Boost Python](www.boost.org/doc/libs/release/libs/python/),
see @ref setup_build for details.

After building, the module file is located at `./python/module/genesis.so`. According to
[the Python documentation](https://docs.python.org/2/tutorial/modules.html#the-module-search-path),
there are several ways to make the module available:

 *  Copy or create a link to the module into the directory that contains the scripts you want to run.
    For example, call

        ln -s path/to/genesis/python/module/genesis.so .

    in that directory. Mind the dot at the end of that line.
 *  Extend the environment variable `PYTHONPATH`. For example, in your terminal configuration,
    add the line

        export PYTHONPATH+="path/to/genesis/python/module"

    On Ubuntu systems, this configuration file is usually located at `/home/user/.bashrc`.

You can test whether or not the module is available for import in Python with

~~~{.py}
import genesis
print genesis.utils.genesis_header()
~~~

We recommend to use an interactive Python shell like [IPython](https://ipython.org/) or
[bpython](http://bpython-interpreter.org/) in order to discover the functionality of genesis.

**Important remark:** At the moment, most, but not all functionality of genesis is exported to
Python. This is mainly due to the fact that the library is developed by a C++ advocate. Well, also
because there are some limits for the interoperability between Python and C++, e.g., regarding C++
concepts like streams, const correctness and templates. This make exporting certain functions
more difficult than others.

If you find yourself in need of a function of genesis that is not yet available via the Python
interface, please
[open an issue on our GitHub page](https://github.com/lczech/genesis/issues). This helps us in
prioritizing the most needed aspects.
