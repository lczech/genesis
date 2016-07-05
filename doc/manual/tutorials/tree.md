Tree {#tree}
===========

# Tree Structure {#tree_representation}

Phylogenetic trees are a representation of the evolutionary history of species. The leaf nodes
usually represent extant species (which have a species name assigned), while the inner nodes are
their putative ancestors (usually without a name).
The edges that connect those nodes often have a `branch length` assigned, which indicates the
"amount of evolution" that happened between the two nodes at the ends of the edge.

The trees can be either rooted or unrooted. In a rooted tree, there is a top level root node, which
symbolizes the "origin" of the tree, that is, the common ancestor of all other nodes in the tree.
As the "origin" of evolution might not be clear in some cases, there are also unrooted trees,
that do not have a top level root node.

In genesis, both cases are stored in the @ref genesis::tree::Tree "Tree data structure",
which represents an unrooted tree. This data structure makes it possible to inspect the tree from
any given node in the same manner - that is, without the need to distinguish between *parent* and
*child* nodes.

The main elements of a @ref genesis::tree::Tree "Tree" are @ref genesis::tree::TreeLink "Links",
@ref genesis::tree::TreeNode "Nodes", and @ref genesis::tree::TreeEdge "Edges".

![Tree structure.](tree_structure.png)

<!-- The yellow, blue and black things makes up the tree. -->

<!-- ## Nodes

Each node... -->
