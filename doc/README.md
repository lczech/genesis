genesis documentation
=====================

Genesis is documented in three different scopes:

1. **api**

   The documentation of the application programming interface is intended for
   people who want to use the functions and classes as a library. It provides
   a reference for the classes, functions and other symbols defined in genesis.

   The API documentation is automatically generated from the doc-blocks within
   the C++ source code using doxygen.

2. **dev**

   This part of the documentation is intended for people who want to work on
   genesis and contribute to its codebase. It contains guidelines and coding
   standards as well as an introduction into the architecture.

3. **man**

   The manual is for people who simply want to use genesis for their purposes.
   It provides first steps and examples that work with the given functionality.

A precompiled version of the documents is provided with each distribution.

If you need a fresh compilation, all three documentations can be created by
calling `make` in this directory. This will generate the necessary documents and
create links to them for convenience. Requirements:
[doxygen](http://www.doxygen.org/) and [latex](http://www.latex-project.org).
