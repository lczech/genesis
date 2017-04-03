Tree Advanced {#tutorials_tree_advanced}
===========

# Tree Structure Revisited {#tutorials_tree_advanced_tree_structure_2}

![Tree structure with Links, Nodes and Edges.](tree/structure.png)

The yellow, blue and black parts are the elements of the tree:

## Links, Nodes and Edges

TODO

## Types of Nodes

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
