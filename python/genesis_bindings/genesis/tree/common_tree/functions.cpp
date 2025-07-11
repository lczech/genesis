#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/functions.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/manipulation.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree/subtree.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_tree_common_tree_functions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::branch_lengths(const class genesis::tree::Tree &) file:genesis/tree/common_tree/functions.hpp line:156
	M("genesis::tree").def("branch_lengths", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::tree::Tree &)) &genesis::tree::branch_lengths, "Get a vector of all branch lengths of a Tree, index by the\n \n\n\n\nC++: genesis::tree::branch_lengths(const class genesis::tree::Tree &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("tree"));

	// genesis::tree::set_all_branch_lengths(class genesis::tree::Tree &, double) file:genesis/tree/common_tree/functions.hpp line:163
	M("genesis::tree").def("set_all_branch_lengths", [](class genesis::tree::Tree & a0) -> void { return genesis::tree::set_all_branch_lengths(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("set_all_branch_lengths", (void (*)(class genesis::tree::Tree &, double)) &genesis::tree::set_all_branch_lengths, "Set all branch lengths of a Tree to a given value.\n\n See also scale_all_branch_lengths() for a scaling function.\n\nC++: genesis::tree::set_all_branch_lengths(class genesis::tree::Tree &, double) --> void", pybind11::arg("tree"), pybind11::arg("length"));

	// genesis::tree::scale_all_branch_lengths(class genesis::tree::Tree &, double) file:genesis/tree/common_tree/functions.hpp line:174
	M("genesis::tree").def("scale_all_branch_lengths", [](class genesis::tree::Tree & a0) -> void { return genesis::tree::scale_all_branch_lengths(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("scale_all_branch_lengths", (void (*)(class genesis::tree::Tree &, double)) &genesis::tree::scale_all_branch_lengths, "Scale all branch lengths of a Tree by a given factor.\n\n This function simply multiplies all branch lengths with the given factor.\n See also set_all_branch_lengths() for setting the branch lengths to a value.\n\nC++: genesis::tree::scale_all_branch_lengths(class genesis::tree::Tree &, double) --> void", pybind11::arg("tree"), pybind11::arg("factor"));

	// genesis::tree::average_branch_length_tree(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) file:genesis/tree/common_tree/functions.hpp line:191
	M("genesis::tree").def("average_branch_length_tree", (class genesis::tree::Tree (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &)) &genesis::tree::average_branch_length_tree, "Return a Tree where the branch lengths are the average of the Tree%s in the given\n vector of Tree%s or TreeSet, given that they all have the same topology.\n\n The function works only under the following conditions:\n\n     * All trees must have the same topology.\n     * The TreeType must provide a data member `branch_length` for the edges.\n\n Otherwise, the function throws an `std::runtime_error`. It does not check for node names,\n but the returned tree will contain the names of the first tree in the set.\n\nC++: genesis::tree::average_branch_length_tree(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) --> class genesis::tree::Tree", pybind11::arg("tset"));

	// genesis::tree::minimal_tree_topology() file:genesis/tree/function/manipulation.hpp line:67
	M("genesis::tree").def("minimal_tree_topology", (class genesis::tree::Tree (*)()) &genesis::tree::minimal_tree_topology, "Create a minimal Tree that can be used with manipulation functions such as add_new_node()\n or add_new_leaf_node() to build a custom tree.\n\n A minimal tree for our purposes consists of two TreeNode%s, a TreeEdge between them,\n and the respective TreeLink%s.\n\n Note that the resulting tree does not have any data types at its nodes and edges.\n See minimal_tree() for a version of this function that also creates these data types.\n\nC++: genesis::tree::minimal_tree_topology() --> class genesis::tree::Tree");

	// genesis::tree::add_new_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) file:genesis/tree/function/manipulation.hpp line:131
	M("genesis::tree").def("add_new_node", (class genesis::tree::TreeNode & (*)(class genesis::tree::Tree &, class genesis::tree::TreeNode &)) &genesis::tree::add_new_node, "Add a new \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::add_new_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_node"));

	// genesis::tree::add_new_node(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:173
	M("genesis::tree").def("add_new_node", [](class genesis::tree::Tree & a0, class genesis::tree::TreeEdge & a1) -> genesis::tree::TreeNode & { return genesis::tree::add_new_node(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"));
	M("genesis::tree").def("add_new_node", (class genesis::tree::TreeNode & (*)(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::add_new_node, "Add a new \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::add_new_node(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"), pybind11::arg("adjust_edges"));

	// genesis::tree::add_new_leaf_node(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:212
	M("genesis::tree").def("add_new_leaf_node", [](class genesis::tree::Tree & a0, class genesis::tree::TreeEdge & a1) -> genesis::tree::TreeNode & { return genesis::tree::add_new_leaf_node(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"));
	M("genesis::tree").def("add_new_leaf_node", (class genesis::tree::TreeNode & (*)(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::add_new_leaf_node, "Add a new \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::add_new_leaf_node(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"), pybind11::arg("adjust_edges"));

	// genesis::tree::delete_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) file:genesis/tree/function/manipulation.hpp line:235
	M("genesis::tree").def("delete_node", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeNode &)) &genesis::tree::delete_node, "Delete a TreeNode from a Tree.\n\n This function is a simple wrapper for the specialized deletion functions:\n\n  * If the node is a leaf, delete_leaf_node() is called.\n  * If it is a linear node, delete_linear_node() is called. Note that in this case, the edges\n    are not adjusted.\n  * In all other cases, delete_subtree() is called, deleting the Subtree of the given\n     away from the root.\n\n See these other functions for details.\n\nC++: genesis::tree::delete_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) --> void", pybind11::arg("tree"), pybind11::arg("target_node"));

	// genesis::tree::delete_leaf_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) file:genesis/tree/function/manipulation.hpp line:242
	M("genesis::tree").def("delete_leaf_node", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeNode &)) &genesis::tree::delete_leaf_node, "Delete a leaf TreeNode.\n\n If the deleted node is the root, the root is reset to the node where the leaf is attached.\n\nC++: genesis::tree::delete_leaf_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &) --> void", pybind11::arg("tree"), pybind11::arg("target_node"));

	// genesis::tree::delete_linear_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:259
	M("genesis::tree").def("delete_linear_node", [](class genesis::tree::Tree & a0, class genesis::tree::TreeNode & a1) -> void { return genesis::tree::delete_linear_node(a0, a1); }, "", pybind11::arg("tree"), pybind11::arg("target_node"));
	M("genesis::tree").def("delete_linear_node", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeNode &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::delete_linear_node, "Delete a \"linear\" TreeNode from a Tree, that is, a node with two neighbours.\n\n Such nodes for example occur as the root node in properly rooted trees. The deletion of this\n node leads to its two edges becoming one. The edge that is deleted is the one further away\n from the root of the tree, while the other edge remains. If the  itself is the root,\n the edge that is being deleted is the one that is not at the primary link of the node.\n In that case, the root is also reset to the node adjacent to the primary link of the \n\n As one edge is deleted, it might be neccessary to update the data of the other, for example,\n to adjust its branch length by adding up both of them. The  functional\n can be used to achivve this. By default, no adjustments are done.\n For an example of a similar function, see\n \n\n\n\nC++: genesis::tree::delete_linear_node(class genesis::tree::Tree &, class genesis::tree::TreeNode &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> void", pybind11::arg("tree"), pybind11::arg("target_node"), pybind11::arg("adjust_edges"));

	// genesis::tree::delete_subtree(class genesis::tree::Tree &, const class genesis::tree::Subtree &) file:genesis/tree/function/manipulation.hpp line:276
	M("genesis::tree").def("delete_subtree", (void (*)(class genesis::tree::Tree &, const class genesis::tree::Subtree &)) &genesis::tree::delete_subtree, "Delete a complete Subtree from a Tree.\n\n The function deletes a Subtree, including the edge where it is attached.\n This works for almost all possible subtrees (including leaf-only subtrees), with one notable\n exception: A subtree that contains all of the tree but one leaf cannot be deleted.\n In that case, the remaining tree would be just a leaf, with no link and no edge,\n which is not a valid tree.\n\n If the subtree contains the root node, the root is reset to the node where the subtree is attached.\n\nC++: genesis::tree::delete_subtree(class genesis::tree::Tree &, const class genesis::tree::Subtree &) --> void", pybind11::arg("tree"), pybind11::arg("subtree"));

	// genesis::tree::delete_edge(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)>) file:genesis/tree/function/manipulation.hpp line:290
	M("genesis::tree").def("delete_edge", [](class genesis::tree::Tree & a0, class genesis::tree::TreeEdge & a1) -> void { return genesis::tree::delete_edge(a0, a1); }, "", pybind11::arg("tree"), pybind11::arg("target_edge"));
	M("genesis::tree").def("delete_edge", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)>)) &genesis::tree::delete_edge, "Delete a TreeEdge from a Tree, that is, contract the two TreeNode%s at its ends into one TreeNode.\n\n This can be understood as contracting the tree at an edge, pulling both nodes at the ends of the\n edge together and merging them into one node with all subtrees of the original nodes combined.\n The TreeNode that remains is the one closer to the root. The other TreeNode is deleted, and all\n its outgoing branches are re-attached to the reamining node. If that other (to-be-deleted) TreeNode\n is a leaf, it is simply fully removed.\n\n Using the  function, the properties of these two TreeNode%s can be adjusted before\n deletion, for example transferring some data from the deleted node to the remaining node.\n\nC++: genesis::tree::delete_edge(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)>) --> void", pybind11::arg("tree"), pybind11::arg("target_edge"), pybind11::arg("adjust_nodes"));

	// genesis::tree::delete_zero_branch_length_edges(class genesis::tree::Tree &, bool) file:genesis/tree/function/manipulation.hpp line:309
	M("genesis::tree").def("delete_zero_branch_length_edges", [](class genesis::tree::Tree & a0) -> void { return genesis::tree::delete_zero_branch_length_edges(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("delete_zero_branch_length_edges", (void (*)(class genesis::tree::Tree &, bool)) &genesis::tree::delete_zero_branch_length_edges, "Delete (contract) all branches of a CommonTree that have branch length zero.\n\n See delete_edge() for details on the underlying function. We here additonally adjust node names\n so that leaf node names are transferred to remaining inner nodes as far as possible.\n\n By default, the function does not delete edges that lead to leaf nodes, even if those edges\n have a branch length of 0. If  is set to `true` however, these edges\n are deleted as well, and their leaf node names (if present) are transferred to their parent\n node if possible. As a node can only have one name, that means that nodes that have multiple\n attached leaf nodes will only get one of the leaf node names (depending on the ordering of edge\n indices), while the other leaf node names will be deleted with their respetive nodes.\n\nC++: genesis::tree::delete_zero_branch_length_edges(class genesis::tree::Tree &, bool) --> void", pybind11::arg("tree"), pybind11::arg("include_leaf_edges"));

	// genesis::tree::make_rooted(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:332
	M("genesis::tree").def("make_rooted", [](class genesis::tree::Tree & a0, class genesis::tree::TreeEdge & a1) -> genesis::tree::TreeNode & { return genesis::tree::make_rooted(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"));
	M("genesis::tree").def("make_rooted", (class genesis::tree::TreeNode & (*)(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::make_rooted, "Root a Tree at a given TreeEdge.\n\n The function expects an unrooted tree with a top-level tri- or multifurcation,\n which is checked via is_rooted().\n It then adds a new \n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::make_rooted(class genesis::tree::Tree &, class genesis::tree::TreeEdge &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("target_edge"), pybind11::arg("adjust_edges"));

	// genesis::tree::make_rooted(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:351
	M("genesis::tree").def("make_rooted", [](class genesis::tree::Tree & a0) -> genesis::tree::TreeNode & { return genesis::tree::make_rooted(a0); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"));
	M("genesis::tree").def("make_rooted", (class genesis::tree::TreeNode & (*)(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::make_rooted, "Root a Tree on the first TreeEdge of its current top level TreeNode.\n\n The tree is expected to be unrooted. The function then roots the tree on the edge that is\n at the primary link of the current top level trifurcation. That is, a new node is inserted on\n that edge, see \n\n\n\n\n\n\n\n\n\nC++: genesis::tree::make_rooted(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("adjust_edges"));

}
