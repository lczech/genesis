#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/attribute_tree/keyed_newick_reader.hpp>
#include <genesis/tree/bipartition/rf.hpp>
#include <genesis/tree/common_tree/distances.hpp>
#include <genesis/tree/common_tree/edge_color.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/distances.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <unordered_map>
#include <utility>
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

void bind_genesis_tree_attribute_tree_keyed_newick_reader(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::KeyedAttributeTreeNewickReader file:genesis/tree/attribute_tree/keyed_newick_reader.hpp line:469
		pybind11::class_<genesis::tree::KeyedAttributeTreeNewickReader, std::shared_ptr<genesis::tree::KeyedAttributeTreeNewickReader>, genesis::tree::NewickReader, genesis::tree::CommonTreeNewickReaderPlugin, genesis::tree::KeyedAttributeTreeNewickReaderPlugin> cl(M("genesis::tree"), "KeyedAttributeTreeNewickReader", "Read default Newick trees, i.e., trees with names and branch lengths.\n\n This class is a convenience wrapper that combines a NewickReader with a\n KeyedAttributeTreeNewickReaderPlugin. It is intended to be used for standard use cases, and\n produces a Tree with AttributeTreeNodeData and AttributeTreeEdgeData at its nodes and edges.\n\n It is also possible to register additional plugins on top of this class.");
		cl.def( pybind11::init( [](){ return new genesis::tree::KeyedAttributeTreeNewickReader(); } ) );
		cl.def( pybind11::init( [](genesis::tree::KeyedAttributeTreeNewickReader const &o){ return new genesis::tree::KeyedAttributeTreeNewickReader(o); } ) );
		cl.def("assign", (class genesis::tree::KeyedAttributeTreeNewickReader & (genesis::tree::KeyedAttributeTreeNewickReader::*)(const class genesis::tree::KeyedAttributeTreeNewickReader &)) &genesis::tree::KeyedAttributeTreeNewickReader::operator=, "C++: genesis::tree::KeyedAttributeTreeNewickReader::operator=(const class genesis::tree::KeyedAttributeTreeNewickReader &) --> class genesis::tree::KeyedAttributeTreeNewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::tree::node_branch_length_distance_matrix(const class genesis::tree::Tree &) file:genesis/tree/common_tree/distances.hpp line:52
	M("genesis::tree").def("node_branch_length_distance_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::tree::Tree &)) &genesis::tree::node_branch_length_distance_matrix, "Return a distance matrix containing pairwise distances between all Nodes, using the\n branch_length of the Edges as distance measurement.\n\n The elements of the matrix are indexed using node().index().\n\nC++: genesis::tree::node_branch_length_distance_matrix(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<double>", pybind11::arg("tree"));

	// genesis::tree::node_branch_length_distance_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeNode *) file:genesis/tree/common_tree/distances.hpp line:66
	M("genesis::tree").def("node_branch_length_distance_vector", [](const class genesis::tree::Tree & a0) -> std::vector<double, class std::allocator<double> > { return genesis::tree::node_branch_length_distance_vector(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("node_branch_length_distance_vector", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeNode *)) &genesis::tree::node_branch_length_distance_vector, "Return a vector containing the distance of all nodes with respect to the given start node,\n where distance is measured in the sum of branch lengths between the nodes.\n\n The vector is indexed using the node().index() for every node. Its elements give the distance of\n each node with respect to the given start node. The distance is the sum of branch lengths of the\n edges visited on the path between the two nodes.\n\n If no Node pointer is provided, the root is taken as node.\n\nC++: genesis::tree::node_branch_length_distance_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeNode *) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("tree"), pybind11::arg("node"));

	// genesis::tree::edge_branch_length_distance_matrix(const class genesis::tree::Tree &) file:genesis/tree/common_tree/distances.hpp line:71
	M("genesis::tree").def("edge_branch_length_distance_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::tree::Tree &)) &genesis::tree::edge_branch_length_distance_matrix, "C++: genesis::tree::edge_branch_length_distance_matrix(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<double>", pybind11::arg("tree"));

	// genesis::tree::edge_branch_length_distance_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge *) file:genesis/tree/common_tree/distances.hpp line:75
	M("genesis::tree").def("edge_branch_length_distance_vector", [](const class genesis::tree::Tree & a0) -> std::vector<double, class std::allocator<double> > { return genesis::tree::edge_branch_length_distance_vector(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("edge_branch_length_distance_vector", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge *)) &genesis::tree::edge_branch_length_distance_vector, "C++: genesis::tree::edge_branch_length_distance_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge *) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("tree"), pybind11::arg("edge"));

	// genesis::tree::deepest_distance(const class genesis::tree::Tree &) file:genesis/tree/common_tree/distances.hpp line:87
	M("genesis::tree").def("deepest_distance", (double (*)(const class genesis::tree::Tree &)) &genesis::tree::deepest_distance, "Return the longest distance from any point in the tree (on the edges) to any leaf.\n\nC++: genesis::tree::deepest_distance(const class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::closest_leaf_distance_vector(const class genesis::tree::Tree &) file:genesis/tree/common_tree/distances.hpp line:100
	M("genesis::tree").def("closest_leaf_distance_vector", (class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > > (*)(const class genesis::tree::Tree &)) &genesis::tree::closest_leaf_distance_vector, "Return a vector containing the closest leaf node for each node, using the branch_length\n as distance measure.\n\n The vector is indexed using the node().index() for every node. Its value contains an std::pair,\n where the first element is a TreeNode* to the closest leaf node of the node at the index,\n measured using the branch_length; the second element of the pair is the distance value itself.\n Thus, leaf nodes will have a pointer to themselves and a distance value of 0.\n\n See also furthest_leaf_distance_vector().\n\nC++: genesis::tree::closest_leaf_distance_vector(const class genesis::tree::Tree &) --> class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > >", pybind11::arg("tree"));

	// genesis::tree::closest_leaf_distance_vector(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &) file:genesis/tree/common_tree/distances.hpp line:104
	M("genesis::tree").def("closest_leaf_distance_vector", (class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > > (*)(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &)) &genesis::tree::closest_leaf_distance_vector, "C++: genesis::tree::closest_leaf_distance_vector(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &) --> class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > >", pybind11::arg("tree"), pybind11::arg("node_branch_length_distance_mat"));

	// genesis::tree::furthest_leaf_distance_vector(const class genesis::tree::Tree &) file:genesis/tree/common_tree/distances.hpp line:112
	M("genesis::tree").def("furthest_leaf_distance_vector", (class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > > (*)(const class genesis::tree::Tree &)) &genesis::tree::furthest_leaf_distance_vector, "Opposite of closest_leaf_distance_vector().\n\nC++: genesis::tree::furthest_leaf_distance_vector(const class genesis::tree::Tree &) --> class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > >", pybind11::arg("tree"));

	// genesis::tree::furthest_leaf_distance_vector(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &) file:genesis/tree/common_tree/distances.hpp line:116
	M("genesis::tree").def("furthest_leaf_distance_vector", (class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > > (*)(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &)) &genesis::tree::furthest_leaf_distance_vector, "C++: genesis::tree::furthest_leaf_distance_vector(const class genesis::tree::Tree &, const class genesis::utils::Matrix<double> &) --> class std::vector<struct std::pair<const class genesis::tree::TreeNode *, double>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, double> > >", pybind11::arg("tree"), pybind11::arg("node_branch_length_distance_mat"));

	// genesis::tree::edge_color_branch_length_gradient(const class genesis::tree::Tree &, bool) file:genesis/tree/common_tree/edge_color.hpp line:54
	M("genesis::tree").def("edge_color_branch_length_gradient", [](const class genesis::tree::Tree & a0) -> std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > { return genesis::tree::edge_color_branch_length_gradient(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("edge_color_branch_length_gradient", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (*)(const class genesis::tree::Tree &, bool)) &genesis::tree::edge_color_branch_length_gradient, "C++: genesis::tree::edge_color_branch_length_gradient(const class genesis::tree::Tree &, bool) --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >", pybind11::arg("tree"), pybind11::arg("zero_based"));

	// genesis::tree::node_path_length_matrix(const class genesis::tree::Tree &) file:genesis/tree/function/distances.hpp line:62
	M("genesis::tree").def("node_path_length_matrix", (class genesis::utils::Matrix<unsigned long> (*)(const class genesis::tree::Tree &)) &genesis::tree::node_path_length_matrix, "Return a matrix containing the pairwise depth of all nodes of the tree.\n\n See \n\n\n\n\n\nC++: genesis::tree::node_path_length_matrix(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<unsigned long>", pybind11::arg("tree"));

	// genesis::tree::node_path_length_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) file:genesis/tree/function/distances.hpp line:71
	M("genesis::tree").def("node_path_length_vector", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &)) &genesis::tree::node_path_length_vector, "Return a vector containing the depth of all nodes with respect to the given start node.\n\n The vector is indexed using the node().index() for every node. Its elements give the depth of\n each node with respect to the given start node. The depth is the number of edges visited on the\n path between two nodes (0 for itself, 1 for immediate neighbours, etc).\n\nC++: genesis::tree::node_path_length_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"), pybind11::arg("node"));

	// genesis::tree::node_path_length_vector(const class genesis::tree::Tree &) file:genesis/tree/function/distances.hpp line:83
	M("genesis::tree").def("node_path_length_vector", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &)) &genesis::tree::node_path_length_vector, "Return a vector containing the depth of all nodes with respect to the root node.\n\n This function calls and returns the value of\n \n\n\n\n\nC++: genesis::tree::node_path_length_vector(const class genesis::tree::Tree &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"));

	// genesis::tree::edge_path_length_matrix(const class genesis::tree::Tree &) file:genesis/tree/function/distances.hpp line:91
	M("genesis::tree").def("edge_path_length_matrix", (class genesis::utils::Matrix<unsigned long> (*)(const class genesis::tree::Tree &)) &genesis::tree::edge_path_length_matrix, "C++: genesis::tree::edge_path_length_matrix(const class genesis::tree::Tree &) --> class genesis::utils::Matrix<unsigned long>", pybind11::arg("tree"));

	// genesis::tree::edge_path_length_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &) file:genesis/tree/function/distances.hpp line:95
	M("genesis::tree").def("edge_path_length_vector", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &)) &genesis::tree::edge_path_length_vector, "C++: genesis::tree::edge_path_length_vector(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("tree"), pybind11::arg("edge"));

	// genesis::tree::closest_leaf_depth_vector(const class genesis::tree::Tree &) file:genesis/tree/function/distances.hpp line:120
	M("genesis::tree").def("closest_leaf_depth_vector", (class std::vector<struct std::pair<const class genesis::tree::TreeNode *, unsigned long>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, unsigned long> > > (*)(const class genesis::tree::Tree &)) &genesis::tree::closest_leaf_depth_vector, "Returns a vector containing the closest leaf node for each node, measured in number of\n edges between them and its depth (number of edges between them).\n\n The vector is indexed using the node().index() for every node. Its value contains an std::pair,\n where the first element is a NodeType* to the closest leaf node (with respect to its depth) and\n the second element its depth with respect to the node at the given index of the vector. The depth\n is the number of edges visited on the path between two nodes (0 for itself, 1 for immediate\n neighbours, etc).\n\n Thus, leaf nodes will have a pointer to themselves and a depth value of 0, and for all other\n nodes the depth will be the number of edges between it and the closest leaf node.\n\n There might be more than one leaf with the same depth to a given node. In this case, an\n arbitrary one is used.\n\nC++: genesis::tree::closest_leaf_depth_vector(const class genesis::tree::Tree &) --> class std::vector<struct std::pair<const class genesis::tree::TreeNode *, unsigned long>, class std::allocator<struct std::pair<const class genesis::tree::TreeNode *, unsigned long> > >", pybind11::arg("tree"));

	// genesis::tree::rf_taxon_name_map(const class genesis::tree::Tree &) file:genesis/tree/bipartition/rf.hpp line:61
	M("genesis::tree").def("rf_taxon_name_map", (class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > (*)(const class genesis::tree::Tree &)) &genesis::tree::rf_taxon_name_map, "Get a mapping from taxon names to unique IDs.\n\n The IDs are consecutive, starting at 0. Only tip/leaf names are processed,\n which need to be unique and non-empty.\n\n The mappning can then be used for unique identification of taxa, which is needed for the\n RF distance calculation functions.\n\nC++: genesis::tree::rf_taxon_name_map(const class genesis::tree::Tree &) --> class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > >", pybind11::arg("tree"));

}
