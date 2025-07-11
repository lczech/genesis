#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/newick_reader.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/functions.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_tree_common_tree_newick_reader(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::CommonTreeNewickReader file:genesis/tree/common_tree/newick_reader.hpp line:326
		pybind11::class_<genesis::tree::CommonTreeNewickReader, std::shared_ptr<genesis::tree::CommonTreeNewickReader>, genesis::tree::NewickReader, genesis::tree::CommonTreeNewickReaderPlugin> cl(M("genesis::tree"), "CommonTreeNewickReader", "Read default Newick trees, i.e., trees with names and branch lengths.\n\n This class is a convenience wrapper that combines a NewickReader with a\n CommonTreeNewickReaderPlugin. It is intended to be used for standard use cases, and produces a\n Tree with CommonNodeData and CommonEdgeData at its nodes and edges.\n\n It is also possible to register additional plugins on top of this class.\n\n Behind the curtain, this class derives from both NewickReader and CommonTreeNewickReaderPlugin.\n This is a bit ugly, but we use it for simplicity. This allows to use an instance as if it was\n a reader (i.e., call `from_...` functions), but also change the plugin settings in a natural\n way.");
		cl.def( pybind11::init( [](){ return new genesis::tree::CommonTreeNewickReader(); } ) );
		cl.def( pybind11::init( [](genesis::tree::CommonTreeNewickReader const &o){ return new genesis::tree::CommonTreeNewickReader(o); } ) );
		cl.def("assign", (class genesis::tree::CommonTreeNewickReader & (genesis::tree::CommonTreeNewickReader::*)(const class genesis::tree::CommonTreeNewickReader &)) &genesis::tree::CommonTreeNewickReader::operator=, "C++: genesis::tree::CommonTreeNewickReader::operator=(const class genesis::tree::CommonTreeNewickReader &) --> class genesis::tree::CommonTreeNewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::tree::is_leaf(const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:58
	M("genesis::tree").def("is_leaf", (bool (*)(const class genesis::tree::TreeLink &)) &genesis::tree::is_leaf, "Return true iff the node of the given link is a leaf node.\n\nC++: genesis::tree::is_leaf(const class genesis::tree::TreeLink &) --> bool", pybind11::arg("link"));

	// genesis::tree::is_leaf(const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:63
	M("genesis::tree").def("is_leaf", (bool (*)(const class genesis::tree::TreeNode &)) &genesis::tree::is_leaf, "Return whether the node is a leaf/tip.\n\nC++: genesis::tree::is_leaf(const class genesis::tree::TreeNode &) --> bool", pybind11::arg("node"));

	// genesis::tree::is_leaf(const class genesis::tree::TreeEdge &) file:genesis/tree/function/functions.hpp line:68
	M("genesis::tree").def("is_leaf", (bool (*)(const class genesis::tree::TreeEdge &)) &genesis::tree::is_leaf, "Return true iff the secondary node (outwards) of the given edge is a leaf node.\n\nC++: genesis::tree::is_leaf(const class genesis::tree::TreeEdge &) --> bool", pybind11::arg("edge"));

	// genesis::tree::is_inner(const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:73
	M("genesis::tree").def("is_inner", (bool (*)(const class genesis::tree::TreeLink &)) &genesis::tree::is_inner, "Return true iff the node of the given link is an inner node.\n\nC++: genesis::tree::is_inner(const class genesis::tree::TreeLink &) --> bool", pybind11::arg("link"));

	// genesis::tree::is_inner(const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:78
	M("genesis::tree").def("is_inner", (bool (*)(const class genesis::tree::TreeNode &)) &genesis::tree::is_inner, "Return whether the node is an inner node.\n\nC++: genesis::tree::is_inner(const class genesis::tree::TreeNode &) --> bool", pybind11::arg("node"));

	// genesis::tree::is_inner(const class genesis::tree::TreeEdge &) file:genesis/tree/function/functions.hpp line:83
	M("genesis::tree").def("is_inner", (bool (*)(const class genesis::tree::TreeEdge &)) &genesis::tree::is_inner, "Return true iff the secondary node (outwards) of the given edge is an inner node.\n\nC++: genesis::tree::is_inner(const class genesis::tree::TreeEdge &) --> bool", pybind11::arg("edge"));

	// genesis::tree::is_root(const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:88
	M("genesis::tree").def("is_root", (bool (*)(const class genesis::tree::TreeLink &)) &genesis::tree::is_root, "Return whether the link belongs to the root node of its Tree.\n\nC++: genesis::tree::is_root(const class genesis::tree::TreeLink &) --> bool", pybind11::arg("link"));

	// genesis::tree::is_root(const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:93
	M("genesis::tree").def("is_root", (bool (*)(const class genesis::tree::TreeNode &)) &genesis::tree::is_root, "Return whether the node is the root of its Tree.\n\nC++: genesis::tree::is_root(const class genesis::tree::TreeNode &) --> bool", pybind11::arg("node"));

	// genesis::tree::degree(const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:99
	M("genesis::tree").def("degree", (unsigned long (*)(const class genesis::tree::TreeLink &)) &genesis::tree::degree, "Return the degree of the node for a given TreeLink,\n i.e. how many neighbouring nodes it has.\n\nC++: genesis::tree::degree(const class genesis::tree::TreeLink &) --> unsigned long", pybind11::arg("link"));

	// genesis::tree::degree(const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:104
	M("genesis::tree").def("degree", (unsigned long (*)(const class genesis::tree::TreeNode &)) &genesis::tree::degree, "Return the degree of the node, i.e. how many neighbouring nodes it has.\n\nC++: genesis::tree::degree(const class genesis::tree::TreeNode &) --> unsigned long", pybind11::arg("node"));

	// genesis::tree::max_degree(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:117
	M("genesis::tree").def("max_degree", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::max_degree, "Return the highest degree of the Nodes of a Tree.\n\n If the Tree is empty, `0` is returned.\n\nC++: genesis::tree::max_degree(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::is_bifurcating(const class genesis::tree::Tree &, bool) file:genesis/tree/function/functions.hpp line:133
	M("genesis::tree").def("is_bifurcating", [](const class genesis::tree::Tree & a0) -> bool { return genesis::tree::is_bifurcating(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("is_bifurcating", (bool (*)(const class genesis::tree::Tree &, bool)) &genesis::tree::is_bifurcating, "Return whether the Tree is bifurcating.\n\n A tree is bifurcating iff all inner nodes have exactly three neighbouring nodes.\n The only exception is the root node, which for rooted trees only has two neighbors.\n Thus, this node is allowed to have two (for rooted trees) or three (for trees with a top-level\n trifurcation instead of an actual root) neighbors. In order to test which of these is the case,\n use is_rooted().\n\n If  is set to `true`, the definition is a bit broadened and also allows other nodes with\n two neighbors. Such nodes do not have a furcation at all, and thus \"sit\" in between two other\n nodes. This case is however rare in practice, but can happen e.g., when wrongly rerooting the\n tree (in case that the old root is not deleted), or on trees that are created from a taxonomy.\n\nC++: genesis::tree::is_bifurcating(const class genesis::tree::Tree &, bool) --> bool", pybind11::arg("tree"), pybind11::arg("loose"));

	// genesis::tree::is_binary(const class genesis::tree::Tree &, bool) file:genesis/tree/function/functions.hpp line:138
	M("genesis::tree").def("is_binary", [](const class genesis::tree::Tree & a0) -> bool { return genesis::tree::is_binary(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("is_binary", (bool (*)(const class genesis::tree::Tree &, bool)) &genesis::tree::is_binary, "Alias for is_bifurcating().\n\nC++: genesis::tree::is_binary(const class genesis::tree::Tree &, bool) --> bool", pybind11::arg("tree"), pybind11::arg("loose"));

	// genesis::tree::is_rooted(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:143
	M("genesis::tree").def("is_rooted", (bool (*)(const class genesis::tree::Tree &)) &genesis::tree::is_rooted, "Return whether the Tree is rooted, that is, whether the root node has two neighbors.\n\nC++: genesis::tree::is_rooted(const class genesis::tree::Tree &) --> bool", pybind11::arg("tree"));

	// genesis::tree::leaf_node_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:148
	M("genesis::tree").def("leaf_node_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::leaf_node_count, "Count the number of leaf Nodes of a Tree.\n\nC++: genesis::tree::leaf_node_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::inner_node_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:153
	M("genesis::tree").def("inner_node_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::inner_node_count, "Count the number of inner Nodes.\n\nC++: genesis::tree::inner_node_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::node_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:158
	M("genesis::tree").def("node_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::node_count, "Return the number of Nodes of a Tree. Same as Tree::node_count().\n\nC++: genesis::tree::node_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::leaf_edge_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:163
	M("genesis::tree").def("leaf_edge_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::leaf_edge_count, "Return the number of Edges of a Tree that lead to a leaf Node.\n\nC++: genesis::tree::leaf_edge_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::inner_edge_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:168
	M("genesis::tree").def("inner_edge_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::inner_edge_count, "Return the number of Edges of a Tree that do not lead to a leaf Node.\n\nC++: genesis::tree::inner_edge_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::edge_count(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:173
	M("genesis::tree").def("edge_count", (unsigned long (*)(const class genesis::tree::Tree &)) &genesis::tree::edge_count, "Return the number of Edges of a Tree. Same as Tree::edge_count().\n\nC++: genesis::tree::edge_count(const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("tree"));

	// genesis::tree::inner_edge_indices(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:179
	M("genesis::tree").def("inner_edge_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::inner_edge_indices, "Get a list of the \n\n\n\nC++: genesis::tree::inner_edge_indices(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::leaf_edge_indices(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:185
	M("genesis::tree").def("leaf_edge_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::leaf_edge_indices, "Get a list of the \n\n\n\nC++: genesis::tree::leaf_edge_indices(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::inner_node_indices(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:191
	M("genesis::tree").def("inner_node_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::inner_node_indices, "Get a list of the \n\n\n\nC++: genesis::tree::inner_node_indices(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::leaf_node_indices(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:197
	M("genesis::tree").def("leaf_node_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::leaf_node_indices, "Get a list of the \n\n\n\nC++: genesis::tree::leaf_node_indices(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::edge_sides(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:220
	M("genesis::tree").def("edge_sides", (class genesis::utils::Matrix<signed char> (*)(const class genesis::tree::Tree &)) &genesis::tree::edge_sides, "Create a \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::edge_sides(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<signed char>", pybind11::arg("tree"));

	// genesis::tree::node_root_direction_matrix(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:230
	M("genesis::tree").def("node_root_direction_matrix", (class genesis::utils::Matrix<signed char> (*)(const class genesis::tree::Tree &)) &genesis::tree::node_root_direction_matrix, "Calculate a Matrix that indicates the nodes on the root side of a given node.\n\n The row and column indices of the Matrix represent TreeNode indices.\n Each element of the Matrix indicates whether the column node is in the subtree of the row node\n that contains the root (value `1`), or in a subtree that does not contain the root (value `-1`),\n while the diagonale contains `0`.\n\nC++: genesis::tree::node_root_direction_matrix(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<signed char>", pybind11::arg("tree"));

	// genesis::tree::subtree_size(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:241
	M("genesis::tree").def("subtree_size", (unsigned long (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &)) &genesis::tree::subtree_size, "Return the size of the subtree defined by the given TreeLink, measured in number of nodes.\n\nC++: genesis::tree::subtree_size(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) --> unsigned long", pybind11::arg("tree"), pybind11::arg("link"));

	// genesis::tree::subtree_sizes(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:259
	M("genesis::tree").def("subtree_sizes", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &)) &genesis::tree::subtree_sizes, "Calculate the sizes of all subtrees as seen from the given TreeNode.\n\n The function returns a vector with as many elements as the Tree has nodes. The vector is indexed\n using the TreeNode::index() values.\n\n Each value in the vector tells the size (in number of nodes) of the subtree of the correnspinding\n node, as seen from the given starting node, and excluding that starting node.\n\n In methaphorical words, the given starting node is used as a hook where the tree is suspended\n from, so that it hangs down. A subtree is then the part of the tree that \"hangs down\" from a\n certain node. We then count the number of nodes in each of those subtrees (that is,\n we examine the subtree starting at each node of the tree).\n For the starting node, the count is always the number of nodes of the tree minus one (because the\n node is not counted itself).\n\nC++: genesis::tree::subtree_sizes(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"), pybind11::arg("node"));

}
