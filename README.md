![genesis](/doc/logo/logo_readme.png?raw=true "genesis")

A toolkit for working with phylogenetic data.

[![Build Status](https://travis-ci.org/lczech/genesis.svg?branch=master)](https://travis-ci.org/lczech/genesis)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](http://www.gnu.org/licenses/gpl.html)
![Language](https://img.shields.io/badge/language-C%2B%2B11-lightgrey.svg)
![Language](https://img.shields.io/badge/language-python-lightgrey.svg)

Purpose
-------------------

The aim of this project is to provide an easy to use toolkit for working with phylogenetic data.
Currently, it supports:

 *  **Placements**. Working with evolutionary placements of short NGS reads on a
    reference phylogenetic tree is the main focus of genesis. The data is typically produced by
    tools like [RAxML-EPA](http://sco.h-its.org/exelixis/web/software/epa/index.html) or
    [pplacer](http://matsen.fhcrc.org/pplacer/) and usually stored in `jplace` files.
 *  **Trees**. Reading, annotating and writing phylogenetic trees is supported, mainly for
    visulization purposes.
 *  **Sequences**. Some basic functionality for reading, filtering and writing sequences in e.g.,
    `fasta` or `phylip` format is available.

The feature list will hopefully grow in the future.

Usage and Resources
-------------------

The functionality of genesis can be used in two ways:

 *  As a C++ library.
 *  As a Python module.

The **documentation** is available at [doc.genesis-lib.org](http://doc.genesis-lib.org/).
It contains a user manual with setup instructions and tutorials, as well as the full API reference.

For **user support**, please see our [support forum](http://support.genesis-lib.org/).
It is intended for questions on how to do things with genesis.

For **bug reports and feature requests**, please
[open an issue on our GitHub page](https://github.com/lczech/genesis/issues).
