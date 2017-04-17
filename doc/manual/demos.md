Demos {#demos}
===========

The demo programs are located in

    ./doc/demos/

They are intended to show some of the functionality of genesis in a ready-to-use setting. You can
also use them as starting point for developing your own applications. Be however aware that the
demos only contain basic error checks for the user input, for reasons of simplicity.

The simplest way to test and use them is to copy the needed files to the `./apps` directory and
compile them using `make update`. See @ref setup_apps for setup instructions.

For the C++ demos, start by inspecting the `main` function. From there, all other functions
of the demos are called.

## Available demo programs

The following demo programs are available:

 *  @subpage demos_visualize_placements "Visualize Placements"

    Takes one or more `jplace` file(s) and visualizes the distribution of
    @ref genesis::placement::Pquery "Pqueries" on the reference tree (that is, the number of
    placements per branch). For this, it uses color coding and outputs a Nexus file.

 *  @subpage demos_extract_clade_placements "Extract Clade Placements"

    Takes a `jplace` file and a mapping of the taxa of its reference tree to some clade names.
    It outputs a `jplace` file for each clade, where each file contains all
    @ref genesis::placement::Pquery "Pqueries" that fell into that clade.

 *  @subpage demos_compare_jplace_files "Compare Jplace Files"

    Takes two `jplace` files with the same reference tree and compares the placement of each
    @ref genesis::placement::Pquery "Pquery". It outputs tables that show which and how Pqueries
    differ in their placement on the tree.

 *  @subpage demos_labelled_tree "Labelled Tree"

    Takes a `jplace` file and optionally a `Newick` tree (for example, with additional bootstrap
    support values) and produces a `Newick` tree where each
    @ref genesis::placement::PqueryPlacement "Placement" is turned intoan individual branch on the
    original tree.

 * @subpage demos_placement_histograms "Placement Histograms"

    Takes a path to either a `jplace` file, or a directory containing `jplace` files,
    and builds histograms of the the placement mass (according to the
    `like_weight_ratio`, LWR) of each placement and of the Expected Distance between Placement
    Locations (EDPL).

See there for more details.
