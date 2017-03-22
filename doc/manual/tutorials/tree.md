Tree {#tutorials_tree}
===========

# Phylogenetic Trees {#tutorials_tree_phylogenetic_trees}

Phylogenetic trees are a representation of the evolutionary history of species. The leaf nodes
usually represent extant species (which have a species name assigned), while the inner nodes are
their putative ancestors (usually without a name).
The edges that connect those nodes often have a `branch length` assigned, which indicates the
"amount of evolution" that happened between the two nodes at the ends of the edge.

The trees can be either rooted or unrooted. In a rooted tree, there is a top level root node, which
symbolizes the "origin" of the tree, that is, the common ancestor of all other nodes in the tree.
As the "origin" of evolution might not be clear in some cases, there are also unrooted trees,
which do not have a top level root node.

# Tree Structure {#tutorials_tree_tree_structure}

In genesis, both cases are stored in a @ref genesis::tree::Tree "Tree" data structure,
which represents an inherently unrooted tree. This data structure makes it possible to inspect the
tree from any given node in the same manner - that is, without the need to distinguish between
*parent* and *child* nodes.

The main elements of a @ref genesis::tree::Tree "Tree" are @ref genesis::tree::TreeLink "Links",
@ref genesis::tree::TreeNode "Nodes", and @ref genesis::tree::TreeEdge "Edges".

![Tree structure.](tree/structure.png)

<!-- The yellow, blue and black things makes up the tree. -->

<!-- The Tree and all its associated classes strive to be highly flexible and highly efficient at
the same time. Unfortunately, this comes at the cost of code complexity. -->

<!-- ## Nodes

Each node... -->

<!--
# iterating / traversing

explain iterators etc

# data model

base node and edge data, default data, etc

# reading and writing revisited

now that we know about the data model, let's see how to fill this with data from files
details about plug-in functionality etc

# Tree Manipulation

## High Level

add_new_node()

## Low Level

reset_outer() etc
-->
