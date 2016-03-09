The documentation of genesis consists of two main parts:
The **Manual** and the **API documentation**.

## Manual

The @subpage manual contains tutorials, code examples and high level explanations of the
functionality of genesis. It is intended for end users who want to use existing scripts and
applications, and for getting started with genesis.

## API documentation

The documentation of the application programming interface (API) is intended for users who want
to use genesis as a library or toolkit for their own applications.
It provides detailed references for the classes, functions and other symbols defined in genesis:

 *  [Namespaces](namespaces.html)
 *  [Classes](annotated.html)
 *  [Source Files](files.html)

As the API documentation is generated from the doc-blocks in the C++ source code, it is mainly
oriented towards the C++ interface. It is however also suitable for the Python interface.




@page manual Manual

The functionality of genesis is divided into different modules:

 Module             | Purpose
--------------------|-------------------------------------------------------------------------------
@subpage placement  | For working with evolutionary placement data, e.g. in `jplace` format.
@subpage tree       | Provides a flexible structure and functions for phylogenetic trees.
@subpage sequence   | Works with sequences in e.g. `fasta` or `fhylip` format.
@subpage utils      | Provides auxiliary tools for e.g., file system, logging, math, texts, colors, etc.

See there for more details.
