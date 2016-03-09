![genesis](/doc/logo/logo_readme.png?raw=true "genesis")

A toolkit for working with phylogenetic data.

[![Build Status](https://travis-ci.org/lczech/genesis.svg?branch=master)](https://travis-ci.org/lczech/genesis)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](http://www.gnu.org/licenses/gpl.html)
![Language](https://img.shields.io/badge/language-C%2B%2B11-lightgrey.svg)
![Language](https://img.shields.io/badge/language-python-lightgrey.svg)

Introduction
------------

The aim of this project is to provide an easy to use toolkit for working with phylogenetic data.
Currently, it supports:

 *  **Placements**. Working with evolutionary placements of short NGS reads on a
    reference phylogenetic tree is the main focus of genesis. The data is typically produced by
    tools like [RAxML-EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html) or
    [pplacer](http://matsen.fhcrc.org/pplacer/) and usually stored in `jplace` files.
 *  **Trees**: Reading, annotating and writing phylogenetic trees is supported, mainly for
    visulization purposes.
 *  **Sequences**. Some basic functionality for reading, filtering and writing sequences in e.g.,
    `fasta` or `phylip` format is available.

The feature list will hopefully grow in the future.

Usage
-----

The library provided by genesis can be used in two ways, depending on the users preferences and
needs:

 *  As a C++ library.
 *  As a Python module.

The code is written as a C++11 library. It can thus be used in custom projects by including
the necessary headers and linking against the binary (either shared or static, see section Build).
Furthermore, for quickly prototyping small applications that use genesis as their main
library, have a look at the `./apps` directory.

In addition to C++, the classes and functions are (mostly) exported to Python. The genesis module
can be imported and used as any other Python code.

Documentation
-------------

The genesis documentation is available at [doc.genesis-lib.org](http://doc.genesis-lib.org/).

Build
-----

To build all of genesis, call

    make

in the main directory. This requires [make](https://www.gnu.org/software/make/) and
[cmake](https://cmake.org/) 2.6 or higher.
It will build all parts of genesis for which the requirements are fulfilled.

Requirements:

 *  Building the C++ library binaries (shared or static) requires a fairly up-to-date C++11
    compiler. We currently use [clang](http://clang.llvm.org/) 3.6.0, but it works with
    [gcc](https://gcc.gnu.org/) as well.
 *  Building the Python module requires [Boost Python](www.boost.org/doc/libs/release/libs/python/)
    1.41 or higher. Frankly, this is a huge requirement, but currently this is the way it is.
    You might need to compile Boost Python with the `-fPIC` flag to work properly.
 *  If you also want to build the tests, you need [Google Test](https://github.com/google/googletest).
    We internally run the tests before every release, so you don't need to. Have a look at the
    `./test` directory for more information.

The build files are written to the `./build` directory, and the final binaries to the `./bin`
directory.

If you add new files to be compiled (either an app, some lib function, python bindings, or tests),
you can call `make update` to include those files without need for recompiling everything.

Issues & Bugs
-------------

Please report any issues, bugs and feature requests to the
[issues](https://github.com/lczech/genesis/issues) page.

References
----------

Please refer to the following articles for more information on phylogenetic placement of short
reads:

> S. Berger, D. Krompass, and A. Stamatakis,
> **Performance, accuracy, and web server for evolutionary placement of short sequence reads
> under maximum likelihood**,
> *Syst. Biol., vol. 60, no. 3, pp. 291–302, 2011*.

> F. A. Matsen, R. B. Kodner, and E. V. Armbrust,
> **pplacer: linear time maximum-likelihood and Bayesian phylogenetic placement of sequences
> onto a fixed reference tree**,
> *BMC Bioinformatics, vol. 11, no. 1, p. 538, 2010*.

> F. A. Matsen, N. G. Hoffman, A. Gallagher, and A. Stamatakis,
> **A format for phylogenetic placements**,
> *PLoS One, vol. 7, no. 2, pp. 1–4, Jan. 2012*.
