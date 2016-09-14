genesis documentation
=====================

This directory contains the files needed for the tutorials and demos.
The genesis documentation is available at [doc.genesis-lib.org](http://doc.genesis-lib.org/).

Also, the files that are used to generate the documentation with doxygen are stored here.
The documentation of genesis contains both, the user manual with tutorials and code examples,
and the API Reference with more details about classes and functions:

 *  The files of the user manual are written in
    [doxygen-flavored markdown format](https://www.stack.nl/~dimitri/doxygen/manual/markdown.html)
    and thus human-readable. They are stored in the subdirectory `manual`.
 *  The API Reference is generated from the doc-blocks in the C++ source code.

In order to generate the full documentation, [doxygen](http://www.doxygen.org/) 1.8.6 and
[graphviz](http://www.graphviz.org/) need to be installed.
The files can then be generated into the subdirectory `html` by calling `make` in this directory.
A symlink to the index page is also created for convenience.
