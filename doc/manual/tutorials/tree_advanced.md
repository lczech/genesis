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

We use Links to define the topology of the tree - they connect Nodes and Edges with each other and link to neighbouring Links. Each Edge has two Links associated with it, and each Node has as many Links associated
as it has Edges (and hence, neighbouring Nodes) next to it.

![Tree structure with Links, Nodes and Edges.](tree/structure.png)

The yellow arrows in the figure are the connections between Links that define the topology.
Each Link has a @ref genesis::tree::TreeLink::next() "next()" Link, which is its neighbour within the same Node,
as well as an @ref genesis::tree::TreeLink::outer() "outer()" Link, which is its counterpart in the Node along the Edge that is associated with the Link. Hence, one can traverse the tree by following the Links:

    next().outer().next().outer() ...

This yields an Eulertour of the tree, and is in fact how the iterators and traversals presented before work.
Given a Link, by following all `next()` links until one reaches the given Link again, one can count the number of edges (degree) of the Node associated with the Link.
That is, Links that belong to leaf nodes of the tree have a `next()` link that points to themselves.

## Links, Nodes and Edges, and their Indices

The data structure hence uses Links to define the tree topology: The `next()` and `outer()` links allow to traverse the tree. The Nodes and Edges of the Tree on the other hand do not contain any topological information;
they are meant to provide an abstraction on top of the topology that is more useful in algorithms.

Each Link, Node, and Edge contains an`index()` property. For each of the three, the indices are consecutive zero-based numbers, that are stable for a given Tree. Two trees that are read from the same Newick file for example will have the same indices for their Links, Nodes, and Edges. Indices of given elements only change if parts of the tree are deleted. Note however that the order of indices is arbitrary and does not necessarily follow any traversal order.

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

## Rooting and Types of Nodes

Generally, all Nodes in Tree are the same: They


In order to switch from the Link-based topology view of the Tree to the Nodes and Edges,
each Link has its @ref genesis::tree::TreeLink::node() "node()" and @ref genesis::tree::TreeLink::edge() "edge()" associated with it. In the other direction, each Node has a @ref genesis::tree::TreeNode::primary_link() "primary_link()" or  @ref genesis::tree::TreeNode::link() "link()" (which is identical, but shorter to write) that gives the link that points towards the root of the Tree,
and each Edge has a @ref genesis::tree::TreeEdge::primary_link() "primary_link()" as well as a @ref genesis::tree::TreeEdge::secondary_link() "secondary_link()", which point towards and away from the root, respectively.


<!-- criterion to distinguish:

  * leaf node: has only one link
  * inner node:  more than one.
  * root node: established via index to the link. all primary and secondary pointers adhere to this.
  * (except for that, they are all the same internally.)

In the image above, we dont show node h and the leaf nodes next to it, because too messy.
the multifurcation its just more links in a circle -->

TODO

<!-- The Tree and all its associated classes strive to be highly flexible and highly efficient at
the same time. Unfortunately, this comes at the cost of code complexity. -->

# Data Model {#tutorials_tree_advanced_data_model}

So far, we have seen the use of Node and Edge data in the form of

    node.data<CommonNodeData>().name;
    edge.data<CommonEdgeData>().branch_length;

It is time to go into detail.

Each Node and each Edge has such a `data()` function, that needs a template parameter to know which particular type of data is associated with the node or edge.

TODO

<!-- base node and edge data, default data, etc

casting via `.data<>()` - recommend to store such pointers locally if needed more than once per
operation/loop body/traversal

this is a bit tedious, but type-safe.

Remark: we used templated trees over data types before. this solved the casting issue, but
introduces many others like code bloating, lots of different independent tree data structures
(casting does not work that way, but we need double templtated converter funciotns for both
lhs and rhd data types, etc), no on-line conversion (trees needed to be deeply copied in order to
change the data types). -->

# Reading and Writing Revisited {#tutorials_tree_advanced_reading_writing_revisited}

TODO

<!-- now that we know about the data model, let's see how to fill this with data from files
details about plug-in functionality etc -->

# Tree Manipulation

## High Level

TODO

<!-- add_new_node() -->

## Low Level

TODO

<!-- reset_outer() etc -->

# Indices


TODO distnaces etc
