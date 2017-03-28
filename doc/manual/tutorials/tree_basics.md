Tree Basics {#tutorials_tree_basics}
===========

# Phylogenetic Trees {#tutorials_tree_basics_phylogenetic_trees}

Phylogenetic trees, or phylogenies, are a representation of the evolutionary history of species.
The leaf nodes usually represent extant species (which have a species `name` assigned), while the
inner nodes are their putative ancestors (usually without a name).
The edges that connect those nodes often have a `branch length` assigned, which indicates the
"amount of evolution" that happened between the two nodes at the ends of the edge.

The trees can be either rooted or unrooted. In a rooted tree, there is a top level root node, which
symbolizes the "origin" of the tree, that is, the common ancestor of all other nodes in the tree.
As the "origin" of evolution might not be clear in some cases, there are also unrooted trees,
which do not have a top level root node.

# Tree Structure {#tutorials_tree_basics_tree_structure}

In Genesis, both rooted and unrooted phylogenies are modeled in the @ref genesis::tree::Tree "Tree"
data structure, which represents an inherently unrooted tree. This data structure makes it possible
to inspect the tree from any given node in the same manner - that is, without the need to
distinguish between *parent* and *child* nodes. It also allows for multifurcations, i.e., Trees in
Genesis don't have to be binary/bifurctating. The notation of a root is simply established via a
pointer to a specific node in the tree, for cases where a root is needed.

The main elements of a @ref genesis::tree::Tree "Tree" are @ref genesis::tree::TreeLink "Links",
@ref genesis::tree::TreeNode "Nodes", and @ref genesis::tree::TreeEdge "Edges".
The purpose of Links is explained later in
@ref tutorials_tree_advanced_tree_structure_2 "Tree Structure Revisited"; for now, we will focus on
Nodes and Edges:

![Nodes and Edges of an unrooted multifurcating Tree.](tree/nodes_and_edges.png)

Genesis allows to store arbitrary data on the Nodes and Edges of a Tree. For many use cases, there
are two important variables:

 * Node names
 * Branch lengths

For this simple, typical use case, we offer the @ref genesis::tree::DefaultTree "DefaultTree".
It is an alias for a Tree that stores a @ref genesis::tree::DefaultNodeData::name "name" string
at each @ref genesis::tree::TreeNode "Node" and a @ref genesis::tree::DefaultEdgeData::branch_length
"branch_length" at each @ref genesis::tree::TreeEdge "Edge". For more information about storing
data on Trees, see @ref tutorials_tree_advanced_data_model "Data Model".

Now, it's time to dive into some code!

The examples in this tutorial assume that you use

~~~{.cpp}
#include "genesis/genesis.hpp"
using namespace genesis::tree;
~~~

at the beginning of your code.

# Reading and Writing {#tutorials_tree_basics_reading_writing}

Reading from a Newick file with node names and branch lengths is achieved via a
@ref genesis::tree::DefaultTreeNewickReader "DefaultTreeNewickReader":

~~~{.cpp}
// Read a Newick file into a Tree object.
Tree tree = DefaultTreeNewickReader().from_file( "path/to/tree.newick" );
~~~

It is also possible to read trees stored in strings. For example, the tree from above can be stored
in Newick like this

    (((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;

which uses node "L" as the root. Reading is then:

~~~{.cpp}
// Given a string containing a Newick tree, read it into a Tree object.
std::string newick = "(((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;";
Tree tree_2 = DefaultTreeNewickReader().from_string( newick );
~~~

Writing a Tree to Newick and PhyloXML works similarly, using
@ref genesis::tree::DefaultTreeNewickWriter "DefaultTreeNewickWriter" and
@ref genesis::tree::DefaultTreePhyloxmlWriter "DefaultTreePhyloxmlWriter":

~~~{.cpp}
// Write a Tree to a Newick file and a PhyloXML file.
DefaultTreeNewickWriter().to_file( tree, "path/to/tree.nw" );
DefaultTreePhyloxmlWriter().to_file( tree, "path/to/tree.phyloxml" );
~~~

For more details, particularly on how to read different data for the nodes and edges, see
@ref tutorials_tree_advanced_reading_writing_revisited "Reading and Writing Revisited".

# Inspecting a Tree {#tutorials_tree_basics_inspection}

Printing a simple overview of the topology for Tress can be done using
@ref genesis::tree::PrinterCompact "PrinterCompact":

~~~{.cpp}
// Print the tree topology including node names and branch lengths.
std::cout << PrinterCompact().print( tree );
~~~

For the tree from above, this yields

    L: 0.8
    ├── I: 0.8
    │   ├── C: 0.3
    │   │   ├── A: 0.2
    │   │   └── B: 0.3
    │   └── H: 0.25
    │       ├── D: 0.4
    │       ├── E: 0.45
    │       ├── F: 0.3
    │       └── G: 0.35
    ├── J: 0.4
    └── K: 0.5

It can also be customized to other than the default data types.
Furthermore, for inspecting the Tree data structure in more detail, we offer
@ref genesis::tree::PrinterDetailed "PrinterDetailed"  and
@ref genesis::tree::PrinterTable "PrinterTable". All of this however assumes that you first
read the @ref tutorials_tree_advanced "Tree Advanced" tutorial.

<!--

# node and edge iterators

for( auto& node : tree.nodes() ) {

}

# iterating / traversing

explain iterators etc
using links for specific directions of traversal
-->
