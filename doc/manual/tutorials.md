Tutorials {#tutorials}
===========

<!-- # Getting Started {#tutorials_getting_started} -->

The easiest way to get started with genesis is to follow the tutorials. They are split into
different modules, which are listed below.

In order to walk through a tutorial, we recommend using the `./apps` directory.
See @ref setup_apps for setup instructions. Once you have set everything up, you can create a new
`*.cpp` file in the `./apps` directory and compile it using `make update`.

You find the code listings of the tutorials in

    ./doc/tutorials/

Those files can be copied to the `./apps` directory for testing them. They contain the examples
from the tutorials, plus all needed headers and namespace declarations (which are omitted in the
tutorials to keep them simple).

# Module Tutorials

The functionality of genesis is divided into different modules:

 Module             | Purpose
--------------------|-------------------------------------------------------------------------------
@subpage placement  | For working with evolutionary placement data, e.g. in `jplace` format.
@subpage tree       | Provides a flexible structure and functions for phylogenetic trees.
@subpage sequence   | Works with sequences in e.g. `fasta` or `phylip` format.
@subpage utils      | Provides auxiliary tools for e.g., file system, logging, math, texts, colors, etc.

See there for more details.

For more code examples, see the @ref demos page.
