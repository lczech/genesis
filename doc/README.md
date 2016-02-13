genesis api documentation
=========================

This directory contains the api documentation of genesis.

Scope
-----

The documentation of the application programming interface (api) is intended for people who want to
use the functions and classes as a library or toolkit for their own applications.
It provides a reference for the classes, functions and other symbols defined in genesis.
It is suitable for both the C++ and Python interfaces.

High-level documentation for end users who want to use existing scripts and applications is instead
provided via the [genesis wiki](https://github.com/lczech/genesis/wiki).
The wiki also contains information for developers who want to work on genesis and contribute to its
codebase.

Build
-----

The api documentation is generated from the doc-blocks within the C++ source code using doxygen.
The files can be created by calling `make` in this directory. This will generate the necessary
documents and create links to them for convenience. The api documentation is written to html and xml format. This requires [doxygen](http://www.doxygen.org/) 1.8.6 and
[graphviz](http://www.graphviz.org/) to be installed on your system.
