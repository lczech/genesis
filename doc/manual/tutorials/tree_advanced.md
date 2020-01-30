Tree Advanced {#tutorials_tree_advanced}
===========

# Tree Structure Revisited {#tutorials_tree_advanced_tree_structure_2}

The introduction in @ref tutorials_tree_basics "Tree Basics" gave an overview of how to do some basic
tasks with trees. For most needs however, a deeper understanding of the underlying data structure is
needed, which we provide here.

As stated before, the elements of a @ref genesis::tree::Tree "Tree" are:

  * @ref genesis::tree::TreeLink "Links"
  * @ref genesis::tree::TreeNode "Nodes"
  * @ref genesis::tree::TreeEdge "Edges"

The Nodes and Edges are an abstraction on top of the tree topology that is used to store data and to
give unique identifiers in algorithms. However, the topology itself is defined via Links - they connect
Nodes and Edges with each other and link to neighbouring Links. Each Edge has two Links associated with it,
and each Node has as many Links associated as it has Edges (and hence, neighbouring Nodes) next to it.

![Tree structure with Links, Nodes and Edges.](tree/structure.png)

The yellow arrows in the figure are the connections between Links that define the topology.
Each Link has a @ref genesis::tree::TreeLink::next() "next()" Link, which is its neighbour within the same Node,
as well as an @ref genesis::tree::TreeLink::outer() "outer()" Link, which is its counterpart in the Node along the Edge that is associated with the Link. Hence, one can traverse the tree by following the Links:

    next().outer().next().outer() ...

This yields an Eulertour of the tree, and is in fact how the iterators and traversals presented before work.
Given a Link, by following all `next()` links until one reaches the given Link again, one can count the number of edges (degree) of the Node associated with the Link.
In particular, Links that belong to leaf nodes of the tree have a `next()` link that points to themselves.

## Switching between Links, Nodes and Edges {#tutorials_tree_advanced_switching}

The @ref genesis::tree::Tree "Tree" data structure hence uses Links to define the tree topology: The `next()` and `outer()` links allow to traverse the tree. The Nodes and Edges of the Tree on the other hand do not contain any topological information;
they are meant to provide an abstraction on top of the topology that is more useful in algorithms.

One can switch from a Link to its associated Edge and Node via the @ref genesis::tree::TreeLink::edge() "edge()" and @ref genesis::tree::TreeLink::node() "node()" functions. To get from a Node to one of its Links, use @ref genesis::tree::TreeNode::primary_link() "primary_link()", which will give the Link that is in the direction towards the root. For Edges, we also distinguish between the directions towards and away from the root; they offer functions @ref genesis::tree::TreeEdge::primary_link() "primary_link()" and
@ref genesis::tree::TreeEdge::secondary_link() "secondary_link()", respectively.

By having the direction towards the root captured in the tree topology this way, algorithms can leverage this information. For example, to quickly find the path towards to root from a given node, one can use:

    node --> primary link --> outer link --> node --> primary link --> ...

Of course, the root of the tree can also be directly accessed via @ref genesis::tree::Tree::root_node() "root_node()". Note that trees in genesis are internally always rooted in the sense that there is such a node towards all primary links lead. However, whether the tree is rooted in the biological sense simply depends on whether the root node has two or more neighbors. See below for details.

## Indices of Links, Nodes, and Edges {#tutorials_tree_advanced_indices}

Each Link, Node, and Edge contains an `index()` property. For each of the three, the indices are consecutive zero-based numbers, that are stable for a given Tree. Two trees that are read from the same Newick file for example will have the same indices for their Links, Nodes, and Edges. Indices of given elements only change if parts of the tree are deleted. Note however that the order of indices is arbitrary and does not necessarily follow any traversal order.

These indices hence can be used to refer to elements of the Tree, for example via
@ref genesis::tree::Tree::node_at() "Tree::node_at()", instead of storing pointers to them.
Furthermore, they are extensively used in genesis as indices in vectors and matrices.
For example, a pairwise distance matrix between nodes in a Tree can use their indices as indices in the matrix,
as done in node_branch_length_distance_matrix().

The Tree also has accessors such as @ref genesis::tree::Tree::links() "Tree::links()",
@ref genesis::tree::Tree::nodes() "Tree::nodes()", and @ref genesis::tree::Tree::edges() "Tree::edges()",
that allow to iterate all the elements in their index order. This is the fastest way of iteration,
and particularly useful to fill index-based vectors and matrices:

~~~{.cpp}
// Prepare a vector of all node names.
auto names = std::vector<std::string>( tree.node_count() );

// Traverse the Nodes of a Tree, and fill the vector.
for( auto const& node : tree.nodes() ) {
    names[ node.index() ] = node.data<CommonNodeData>().name;
}
~~~

The Nodes and Edges of the Tree are also able to store arbitrary extra data, as explained below in
@ref tutorials_tree_advanced_data_model "Data Model".

## Rooting and Types of Nodes {#tutorials_tree_advanced_rooting}

As mentioned above, trees in genesis are always rooted in the sense that there is one "starting" node from which tree traversals begin (unless a specific node to start from is provided). However, they are not necessarily rooted in the biological sense.

![Tree rooting.](tree/tree_rooting.png)

The tree in (a) above is unrooted in the biologocal sense, and drawn that way. However, the green node serves as a so-called top-level trifurcation, that is, it is the root node for the tree structure in genesis.
On the other hand, the tree in (b) is rooted on an edge, and hence has a top-level bifurcation that serves as the root.

In genesis, both trees are treated completely the same - they are not identical, as (b) has one additional node (its green root node), but they both just use the normal Tree datastructure.
A rooted and an unrooted tree can be distinguished by the number of neighbouring nodes of the root: in (a), there are three neighbours, in (b) there are only two. This is for example how the @ref genesis::tree::is_rooted() "is_rooted()" function works.

Apart from that distinction, all nodes in a tree are treated the same. The only ways in which the root information is stored in the tree is via the @ref genesis::tree::Tree::root_link() "root_link()" of the tree, as well as the implicit information provided via the primary links of the nodes in the tree.

<!--
In order to switch from the Link-based topology view of the Tree to the Nodes and Edges,
each Link has its @ref genesis::tree::TreeLink::node() "node()" and @ref genesis::tree::TreeLink::edge() "edge()" associated with it. In the other direction, each Node has a @ref genesis::tree::TreeNode::primary_link() "primary_link()" or  @ref genesis::tree::TreeNode::link() "link()" (which is identical, but shorter to write) that gives the link that points towards the root of the Tree,
and each Edge has a @ref genesis::tree::TreeEdge::primary_link() "primary_link()" as well as a @ref genesis::tree::TreeEdge::secondary_link() "secondary_link()", which point towards and away from the root, respectively.
-->

<!-- criterion to distinguish:

  * leaf node: has only one link
  * inner node:  more than one.
  * root node: established via index to the link. all primary and secondary pointers adhere to this.
  * (except for that, they are all the same internally.)

In the image above, we dont show node h and the leaf nodes next to it, because too messy.
the multifurcation its just more links in a circle -->

<!-- The Tree and all its associated classes strive to be highly flexible and highly efficient at
the same time. Unfortunately, this comes at the cost of code complexity. -->

# Data Model {#tutorials_tree_advanced_data_model}

So far, we have seen the use of Node and Edge data in the form of

    node.data<CommonNodeData>().name;
    edge.data<CommonEdgeData>().branch_length;

Each Node and each Edge has such a `data<>()` function, that needs a template parameter to know which particular type of data is associated with the node or edge. The nodes and edges can hold any `data<>` that is derived from @ref genesis::tree::BaseNodeData "BaseNodeData" and @ref genesis::tree::BaseEdgeData "BaseEdgeData", respectively.

The most common example of such data classes are @ref genesis::tree::CommonNodeData "CommonNodeData" and @ref genesis::tree::CommonEdgeData "CommonEdgeData", which provide the defaults for most standard use cases. They provide taxon names, and branch lenghts, as shown in the code snipped above. Both serve as an example of how to correctly derive from the base classes (which do not hold any data) in order to create custom data classes for the nodes and edges.

<!--
Remark: we used templated trees over data types before. this solved the casting issue, but
introduces many others like code bloating, lots of different independent tree data structures
(casting does not work that way, but we need double templtated converter funciotns for both
lhs and rhd data types, etc), no on-line conversion (trees needed to be deeply copied in order to
change the data types).
-->

# Reading and Writing Revisited {#tutorials_tree_advanced_reading_writing_revisited}

In the Tree Basics @ref tutorials_tree_basics_reading_writing "Reading and Writing" section,
we have seen how to read a tree from a Newick file:

~~~{.cpp}
// Read a Newick file into a Tree object.
Tree tree = CommonTreeNewickReader().read( from_file( "path/to/tree.newick" ));
~~~

With the above @ref tutorials_tree_advanced_data_model "Data Model" section,
we can now better understand how the tree reading works:
The instance of @ref genesis::tree::CommonTreeNewickReader "CommonTreeNewickReader" that is used in the code snipped is a specialization of the general @ref genesis::tree::NewickReader "NewickReader" class, which is able to parse `newick` files. The specialization then adds a layer on top that understands the taxa names and the branch lengths that are present in a typical `newick` tree, creates a tree with @ref genesis::tree::CommonNodeData "CommonNodeData" and @ref genesis::tree::CommonEdgeData "CommonEdgeData" data types at the nodes and edges, and fills them with the respective data found in the input `newick` tree.

We use this abstraction of "basic tree reader" plus "special layer for the actual data" in order to allow for trees that accommodate different types of data and meta-data. In particular, the `newick` file format only specifies taxon names and branch lengths in the standard. However, often there is additional data such as bootstrap support values, or edge numbers as used in phylogenetic placement, that are not part of `newick` itself. Instead, these extensions "misuse" features such as comments (denoted as square brackets `[]` in newick strings) to store such additional data. Hence, the semantics of such data is not clear if one is given a `newick` tree without further information.

For example, the tree

    ((C,D)[&!color=#009966],(A,(B,X)[&bs=82,!color=#009966])[&bs=70],E);

contains some information about `color` and `bs` (bootstrap support) that are non-standard. Consequently, our reader allows to customize how such data is interpreted and stored in the resulting @ref genesis::tree::Tree "Tree" object.

Note that by default, the Tree does not support color or bootstrap support values - if needed, these could be implemented as new node and edge data types, as explained in the @ref tutorials_tree_advanced_data_model "Data Model" section. Then, in order to store data at the respective nodes and edges when reading a tree, the @ref genesis::tree::NewickReader "NewickReader" has several plugin points for functions that are called whenever data for nodes and edges is parsed while reading the tree. See
@ref genesis::tree::CommonTreeNewickReaderPlugin "CommonTreeNewickReaderPlugin" for examples of how this can be implemented.
<!-- @ref genesis::tree::NewickReader "NewickReader", @ref genesis::tree::CommonTreeNewickReader "CommonTreeNewickReader", and-->

This technique works the same for writing trees. There, the plugin functions for @ref genesis::tree::NewickWriter "NewickWriter" and @ref genesis::tree::PhyloxmlWriter "PhyloxmlWriter" allow to customize how data that is stored in a @ref genesis::tree::Tree "Tree" object is turned into strings to be stored in the output files.

<b>Remark:</b> See <a href="https://doi.org/10.1093/molbev/msx055">Czech <i>et al.</i> 2017</a> for a comprehensive review on the issue of tree data storage in the Newick file format.

# Tree Manipulation {#tutorials_tree_advanced_manipulation}

We also provide some simple tree manipulation methods for basic tasks. Complex tree moves (SPR, TBR, NNI, or similar topological rearrangement methods) are currently *not* implemented.

## High Level

A good starting point to explore tree manipulation is @ref genesis::tree::minimal_tree_topology() "minimal_tree_topology()", which returns a minimal tree with two nodes, connected by an edge.
The function sets up the basic objects, including the respective links, and connects them to each other.
A similar function is @ref genesis::tree::minimal_tree() "minimal_tree()", which additionally sets the data of the nodes and edges.

Given a Tree, one can then use functions such as @ref genesis::tree::add_new_node() "add_new_node()" (in multiple variants) and @ref genesis::tree::delete_node() "delete_node()" and its variants for basic high-level tree manipulation.
Functions for rooting (@ref genesis::tree::make_rooted() "make_rooted()"), unrooting (@ref genesis::tree::make_unrooted() "make_unrooted()") and rerooting (@ref genesis::tree::change_rooting() "change_rooting()") are also provided.

<!-- add_new_node() -->

## Low Level

All the above functions internally use (quite extensive) manipulation of the pointers in the Nodes, Edges, and Links to achieve the desired effects. In other words, they reset where `next()` and `outer()` Links point to, etc. This looks quite involved at first glance, but using the exemplary figure above, one can track the changes and write their own tree manipulation functions as needed. The exemplary high-level functions listed above should suffice as code examples to get started.

<!-- reset_outer() etc -->
