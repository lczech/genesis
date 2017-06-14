Labelled Tree {#demos_labelled_tree}
===========

@section Summary

Takes a `jplace` file and optionally a `Newick` tree (for example, with additional bootstrap
support values) and produces a `Newick` tree where each
@ref genesis::placement::PqueryPlacement "Placement" is turned into an individual branch on the
original tree.

This demo is located at

    genesis/doc/demos/labelled_tree.cpp

@section Details

The program takes either two or three arguments:

    ./labelled_tree <jplace_file> [<newick_tree_file>] <output_newick_tree_file>

It then produces a labelled tree for the given `jplace` @ref genesis::placement::Sample "Sample".
That is, the resulting tree contains a leaf node for each
@ref genesis::placement::PqueryPlacement "Placement" of the Sample.
See @ref genesis::placement::labelled_tree( Sample const&, bool, std::string const& ) "labelled_tree()"
for details.

If the optional `<newick_tree_file>` is given, this tree is used instead of the tree contained in
the `jplace` file. It has to be topologically identical to the `jplace` tree, but can contain
additional values like bootstrap support values. Currently, we only support such values that are
stored as inner node names. This is for example given if a bootstrap tree of RAxML is used.
When working with trees containing such values at inner nodes, also be aware of issues that might
arise with other programs. See https://academic.oup.com/mbe/article-lookup/doi/10.1093/molbev/msx055 for details.

In the beginning of the main function of the program, there are two settings `fully_resolve` and
`name_prefix`, according to the two optional arguments of
@ref genesis::placement::labelled_tree( Sample const&, bool, std::string const& ) "labelled_tree()",
that can be changed if needed.
