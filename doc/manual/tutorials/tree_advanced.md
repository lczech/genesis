Tree Advanced {#tutorials_tree_advanced}
===========

# Tree Structure Revisited {#tutorials_tree_advanced_tree_structure_2}

![Tree structure with Links, Nodes and Edges.](tree/structure.png)

<!--
The yellow, blue and black things makes up the tree.

criterion to distinguish:
leaf node: has only one link
inner node:  more than one.
except for that, same.

we dont show node h and the leaf nodes next to it, because messy.
the multifurcation its just more links in a circle
-->

<!-- The Tree and all its associated classes strive to be highly flexible and highly efficient at
the same time. Unfortunately, this comes at the cost of code complexity. -->

<!-- ## Nodes

Each node... -->

# Data Model {#tutorials_tree_advanced_data_model}

base node and edge data, default data, etc

casting via .data<>() - recommend to store such pointers locally if needed more than once per
operation/loop body/traversal

is tedious, but typesafe.

remark: we had templated trees over data types before. this solved the casting issue, but
introduces many others like code bloating, lots of different independent tree data structures
(casting does not work that way, but we need double templtated converter funciotns for both
lhs and rhd data types, etc)

# Reading and Writing Revisited {#tutorials_tree_advanced_reading_writing_revisited}

now that we know about the data model, let's see how to fill this with data from files
details about plug-in functionality etc

<!--
# Tree Manipulation

## High Level

add_new_node()

## Low Level

reset_outer() etc
-->
