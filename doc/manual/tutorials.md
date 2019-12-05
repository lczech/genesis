Tutorials {#tutorials}
===========

<!-- # Getting Started {#tutorials_getting_started} -->

The easiest way to get started with genesis is to follow the tutorials. They are split into
different modules, which are listed below.

In order to walk through a tutorial, we recommend using the `./apps` directory.
See @ref setup_apps for setup instructions. Once you have set everything up, you can create a new
`*.cpp` file in the `./apps` directory and compile it using `make update`.

You find the code listings of the tutorials in

    ./doc/code/tutorials/

Those files can be copied to the `./apps` directory for testing them. They contain the examples
from the tutorials, plus all needed headers and namespace declarations (which are omitted in the
tutorials to keep them simple).

# Tutorials Chapters

  * @subpage tutorials_placement_basics : For working with phylogenetic placement data, e.g. in `jplace` format.
  * @subpage tutorials_placement_analyses : Conducting typical analyses of placement data.
  * @subpage tutorials_tree_basics : Provides a flexible structure and functions for phylogenetic trees.
  * @subpage tutorials_tree_advanced : More details about the data structure and its data.
  * @subpage tutorials_tree_visualization : Visualization of trees, such as drawing them to `svg` files.
  * @subpage tutorials_sequence : Works with genetic sequences in e.g. `fasta` or `phylip` format.
  * @subpage tutorials_taxonomy : Works with taxonomies and taxonomic databases.
  * @subpage tutorials_colors : Working with colors, for visualization of trees and others.
  <!-- * @subpage tutorials_utils: Provides auxiliary tools for e.g., file system, logging, math, texts, colors, etc. -->

For more code examples, see the @ref demos page.

<!--
# What Next?

  * inspect the [Namespaces](namespaces.html) list and [Classes](annotated.html) list.
  * look at code, write your own apps and programs.
  * ask at support.
  * contribute code.
  * Do science!
  * Save the world!
-->
