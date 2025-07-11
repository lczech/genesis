#include <genesis/placement/placement_tree.hpp>
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

void bind_genesis_tree_function_functions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::subtree_sizes(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:267
	M("genesis::tree").def("subtree_sizes", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::subtree_sizes, "Calculate the sizes of all subtrees as seen from the root of the tree.\n\n See \n\n\n\nC++: genesis::tree::subtree_sizes(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::subtree_max_path_height(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) file:genesis/tree/function/functions.hpp line:273
	M("genesis::tree").def("subtree_max_path_height", (unsigned long (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &)) &genesis::tree::subtree_max_path_height, "Calculate the height of a subtree, that is, the maximum path length to a leaf of that\n subtree, measured in edges between the link and the leaf.\n\nC++: genesis::tree::subtree_max_path_height(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) --> unsigned long", pybind11::arg("tree"), pybind11::arg("link"));

	// genesis::tree::subtree_max_path_heights(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:275
	M("genesis::tree").def("subtree_max_path_heights", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &)) &genesis::tree::subtree_max_path_heights, "C++: genesis::tree::subtree_max_path_heights(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"), pybind11::arg("node"));

	// genesis::tree::subtree_max_path_heights(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:276
	M("genesis::tree").def("subtree_max_path_heights", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::subtree_max_path_heights, "C++: genesis::tree::subtree_max_path_heights(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::sign_matrix(const class genesis::tree::Tree &, bool) file:genesis/tree/function/functions.hpp line:325
	M("genesis::tree").def("sign_matrix", [](const class genesis::tree::Tree & a0) -> genesis::utils::Matrix<signed char> { return genesis::tree::sign_matrix(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("sign_matrix", (class genesis::utils::Matrix<signed char> (*)(const class genesis::tree::Tree &, bool)) &genesis::tree::sign_matrix, "Compute the sign matrix or Sequential Binary Partition (SBP) of a Tree.\n\n The Tree has to be rooted and strictly bifurcating. Then, we can compute a matrix that tells for\n each node a relative ordering of the other nodes. Say we have the tree:\n\n            /----------- T3\n      /-----| N2\n     |      \\----------- T2\n     | N1\n     |\n      \\----------- T1\n\n This yields the (compressed) sign matrix:\n\n Taxa | T1 | T2 | T3\n :--- |---:|---:|---:\n N1   | +1 | -1 | -1\n N2   |  0 | +1 | -1\n\n That is, all nodes in the subtree of the first child of a node get assigned a `+1`,\n and all nodes in the subtree of the second child get a `-1`. The remaining nodes (the rest of\n the tree, including the node itself) get assigned a `0`.\n\n This was introduced as Sequential Binary Partition (SBP) in [1], and called Sign Matrix in [2].\n See there for more details.\n\n By default, the matrix dimensions are `n*n` with `n` being the number of nodes in the tree.\n That is, all node indices are used. This is easiest to work with in the context of other\n functions that used node indices. However, the matrix contains a lot of zeros.\n\n If however  is `true`, the matrix is not indexed by the node indices.\n Instead, the rows only contain inner nodes, and the columns only contain leaf nodes,\n as shown in the table above. That means that the sign of inner nodes is not available in the matrix.\n Use inner_node_indices() to get the node indices that correspond to the rows of the matrix,\n and use leaf_node_indices() for the node indices that correspond to the columns.\n\n > [1] V. Pawlowsky-Glahn and J. J. Egozcue,\n > \"Exploring Compositional Data with the CoDa-Dendrogram,\"\n > Austrian J. Stat., vol. 40, no. 1&2, pp. 103–113, 2011.\n > https://ajs.or.at/index.php/ajs/article/view/vol40,%20no1&2%20-%2011\n\n > [2] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,\n > \"A phylogenetic transform enhances analysis of compositional microbiota data,\"\n > Elife, vol. 6, p. e21887, Feb. 2017.\n > https://elifesciences.org/articles/21887\n\nC++: genesis::tree::sign_matrix(const class genesis::tree::Tree &, bool) --> class genesis::utils::Matrix<signed char>", pybind11::arg("tree"), pybind11::arg("compressed"));

	// genesis::tree::path_to_root(const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:339
	M("genesis::tree").def("path_to_root", (class std::vector<const class genesis::tree::TreeLink *, class std::allocator<const class genesis::tree::TreeLink *> > (*)(const class genesis::tree::TreeNode &)) &genesis::tree::path_to_root, "Helper function that finds all TreeLink%s between a given TreeNode and the root of the\n Tree.\n\n Both the \n\n\n\n\nC++: genesis::tree::path_to_root(const class genesis::tree::TreeNode &) --> class std::vector<const class genesis::tree::TreeLink *, class std::allocator<const class genesis::tree::TreeLink *> >", pybind11::arg("node"));

	// genesis::tree::lowest_common_ancestor(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:344
	M("genesis::tree").def("lowest_common_ancestor", (const class genesis::tree::TreeNode & (*)(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)) &genesis::tree::lowest_common_ancestor, "Return the lowest common ancestor of two TreeNode%s.\n\nC++: genesis::tree::lowest_common_ancestor(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("node_a"), pybind11::arg("node_b"));

	// genesis::tree::lowest_common_ancestor(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) file:genesis/tree/function/functions.hpp line:349
	M("genesis::tree").def("lowest_common_ancestor", (class genesis::tree::TreeNode & (*)(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)) &genesis::tree::lowest_common_ancestor, "Return the lowest common ancestor of two TreeNode%s.\n\nC++: genesis::tree::lowest_common_ancestor(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("node_a"), pybind11::arg("node_b"));

	// genesis::tree::lowest_common_ancestors(const class genesis::tree::Tree &) file:genesis/tree/function/functions.hpp line:361
	M("genesis::tree").def("lowest_common_ancestors", (class genesis::utils::Matrix<unsigned long> (*)(const class genesis::tree::Tree &)) &genesis::tree::lowest_common_ancestors, "Return the lowest common ancestor of each pair of TreeNode%s for a given tree,\n in form of a \n\n\n\n\n\n\n\n\n\nC++: genesis::tree::lowest_common_ancestors(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<unsigned long>", pybind11::arg("tree"));

}
