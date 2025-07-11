#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/newick_reader.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/reader.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
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

// genesis::tree::CommonNodeData file:genesis/tree/common_tree/tree.hpp line:79
struct PyCallBack_genesis_tree_CommonNodeData : public genesis::tree::CommonNodeData {
	using genesis::tree::CommonNodeData::CommonNodeData;

};

// genesis::tree::CommonEdgeData file:genesis/tree/common_tree/tree.hpp line:144
struct PyCallBack_genesis_tree_CommonEdgeData : public genesis::tree::CommonEdgeData {
	using genesis::tree::CommonEdgeData::CommonEdgeData;

};

void bind_genesis_tree_common_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::CommonNodeData file:genesis/tree/common_tree/tree.hpp line:79
		pybind11::class_<genesis::tree::CommonNodeData, std::shared_ptr<genesis::tree::CommonNodeData>, PyCallBack_genesis_tree_CommonNodeData> cl(M("genesis::tree"), "CommonNodeData", "Common class containing the commonly needed data for tree nodes.\n\n The Tree class can use all data for its nodes that derive from BaseNodeData.\n In most cases, nodes will contain the node's name and edges will contain a branch length.\n\n This class is a default for node data, and can be derived from if a node name is wanted. It is\n however not necessary -- so if there is no need for node names, you can provide your own,\n class for tree, as long as it also derives from BaseNodeData.");
		cl.def_readwrite("name", &genesis::tree::CommonNodeData::name);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::CommonNodeData, struct std::default_delete<class genesis::tree::CommonNodeData> > (*)()) &genesis::tree::CommonNodeData::create, "C++: genesis::tree::CommonNodeData::create() --> class std::unique_ptr<class genesis::tree::CommonNodeData, struct std::default_delete<class genesis::tree::CommonNodeData> >");
	}
	{ // genesis::tree::CommonEdgeData file:genesis/tree/common_tree/tree.hpp line:144
		pybind11::class_<genesis::tree::CommonEdgeData, std::shared_ptr<genesis::tree::CommonEdgeData>, PyCallBack_genesis_tree_CommonEdgeData> cl(M("genesis::tree"), "CommonEdgeData", "Common class containing the commonly needed data for tree edges.\n\n This class is the equivalent of CommonNodeData for the tree edges. It stores a branch length\n for each edge. For more information, see CommonNodeData.\n\n If you for example do not need a single branch length, but multiple ones (e.g., when working with\n partitions), simply do not derive from this class, but provide your own implementation.");
		cl.def_readwrite("branch_length", &genesis::tree::CommonEdgeData::branch_length);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::CommonEdgeData, struct std::default_delete<class genesis::tree::CommonEdgeData> > (*)()) &genesis::tree::CommonEdgeData::create, "C++: genesis::tree::CommonEdgeData::create() --> class std::unique_ptr<class genesis::tree::CommonEdgeData, struct std::default_delete<class genesis::tree::CommonEdgeData> >");
	}
	{ // genesis::tree::NewickBrokerElement file:genesis/tree/formats/newick/element.hpp line:60
		pybind11::class_<genesis::tree::NewickBrokerElement, std::shared_ptr<genesis::tree::NewickBrokerElement>> cl(M("genesis::tree"), "NewickBrokerElement", "Store the information for one element of a Newick tree.\n\n Most of the class' members are public, as it is intended to serve an an intermediate data\n exchange format, so different callers might need to modify its content.\n However, this means paying attention when working with the data, as it can\n be changed from anywhere.\n\n See NewickBroker class for a description of this intermediate format.");
		cl.def( pybind11::init( [](){ return new genesis::tree::NewickBrokerElement(); } ) );
		cl.def( pybind11::init<long>(), pybind11::arg("depth") );

		cl.def( pybind11::init<const std::string &, long>(), pybind11::arg("name"), pybind11::arg("depth") );

		cl.def( pybind11::init( [](genesis::tree::NewickBrokerElement const &o){ return new genesis::tree::NewickBrokerElement(o); } ) );
		cl.def_readwrite("name", &genesis::tree::NewickBrokerElement::name);
		cl.def_readwrite("values", &genesis::tree::NewickBrokerElement::values);
		cl.def_readwrite("tags", &genesis::tree::NewickBrokerElement::tags);
		cl.def_readwrite("comments", &genesis::tree::NewickBrokerElement::comments);
		cl.def_readwrite("depth", &genesis::tree::NewickBrokerElement::depth);
		cl.def("assign", (struct genesis::tree::NewickBrokerElement & (genesis::tree::NewickBrokerElement::*)(const struct genesis::tree::NewickBrokerElement &)) &genesis::tree::NewickBrokerElement::operator=, "C++: genesis::tree::NewickBrokerElement::operator=(const struct genesis::tree::NewickBrokerElement &) --> struct genesis::tree::NewickBrokerElement &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("rank", (long (genesis::tree::NewickBrokerElement::*)() const) &genesis::tree::NewickBrokerElement::rank, "Returns the rank (number of immediate children) of this node.\n\n NewickBroker::assign_ranks() has to be called before using this function. Otherwise,\n this function will throw an std::logic_error.\n\nC++: genesis::tree::NewickBrokerElement::rank() const --> long");
		cl.def("is_root", (bool (genesis::tree::NewickBrokerElement::*)() const) &genesis::tree::NewickBrokerElement::is_root, "Return whether this is the root node of the tree.\n\nC++: genesis::tree::NewickBrokerElement::is_root() const --> bool");
		cl.def("is_leaf", (bool (genesis::tree::NewickBrokerElement::*)() const) &genesis::tree::NewickBrokerElement::is_leaf, "Return whether this is a leaf node.\n\nC++: genesis::tree::NewickBrokerElement::is_leaf() const --> bool");
		cl.def("is_inner", (bool (genesis::tree::NewickBrokerElement::*)() const) &genesis::tree::NewickBrokerElement::is_inner, "Return whether this is an inner node, i.e., not a leaf node.\n\nC++: genesis::tree::NewickBrokerElement::is_inner() const --> bool");
	}
	{ // genesis::tree::NewickBroker file:genesis/tree/formats/newick/broker.hpp line:106
		pybind11::class_<genesis::tree::NewickBroker, std::shared_ptr<genesis::tree::NewickBroker>> cl(M("genesis::tree"), "NewickBroker", "Stores a Newick tree in an intermediate format that can be further processed into a Tree.\n\n The NewickBroker is a transitional format between the Newick formatted tree and a Tree object.\n It is neccessary to have this intermediate step, because the ordering of nodes in the Newick\n format differs from our requirements. Newick trees starts with leaf nodes, while our internal\n representation is build up from the root.\n\n The NewickBroker is organized as a stack, where the root of the tree is at the top/front. Then\n follow the nodes in a preorder manner, where each node is of type NewickBrokerElement.\n\n The topology of the tree is represented via a depth attribute of each node: Two subsequent nodes\n are siblings (belong to the same parent node), if they have the same depth. If the second node\n has a depth that is one higher than the first ones, it is it's child (depth thus cannot increase\n more than one between two nodes). Lastly, if the depth of the second node is smaller than the\n first ones, it belongs to a different subtree.\n\n For example, the following tree in Newick format:\n\n     ((A,((B,C,D)E,F)G)H,((I,J,K)L,M,N)O,P,Q)R;\n\n would be stored as the following stack:\n\n     R Rank(4)\n         Q (Leaf)\n         P (Leaf)\n         O Rank(3)\n             N (Leaf)\n             M (Leaf)\n             L Rank(3)\n                 K (Leaf)\n                 J (Leaf)\n                 I (Leaf)\n         H Rank(2)\n             G Rank(2)\n                 F (Leaf)\n                 E Rank(3)\n                     D (Leaf)\n                     C (Leaf)\n                     B (Leaf)\n             A (Leaf)\n\n Here, the rank represents the number of immediate children of this node. Leaf nodes have no\n children and thus rank zero.\n\n Every function modifiying the content of the broker is required to leave it in a valid state,\n meaning:\n\n  *  The top node has to be the root of the tree, which is equivalent to having depth zero. This\n     is also true for trees rooted on a leaf.\n  *  The nesting of the nodes has to be correct, so the depth cannot increase more than one per\n     node when going down the tree.\n  *  The attribute NewickBrokerElement::is_leaf of the NewickBrokerElements can be set (for\n     exmaple when parsing a Newick tree) and then be used to validate the integrity of the tree\n     using validate(). However, the attribute is not further used -- see its description for more\n     on this.\n %");
		cl.def( pybind11::init( [](){ return new genesis::tree::NewickBroker(); } ) );
		cl.def( pybind11::init( [](genesis::tree::NewickBroker const &o){ return new genesis::tree::NewickBroker(o); } ) );
		cl.def("assign", (class genesis::tree::NewickBroker & (genesis::tree::NewickBroker::*)(const class genesis::tree::NewickBroker &)) &genesis::tree::NewickBroker::operator=, "C++: genesis::tree::NewickBroker::operator=(const class genesis::tree::NewickBroker &) --> class genesis::tree::NewickBroker &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("clear", (void (genesis::tree::NewickBroker::*)()) &genesis::tree::NewickBroker::clear, "Deletes all nodes from the broker.\n\nC++: genesis::tree::NewickBroker::clear() --> void");
		cl.def("push_top", (void (genesis::tree::NewickBroker::*)(const struct genesis::tree::NewickBrokerElement &)) &genesis::tree::NewickBroker::push_top, "C++: genesis::tree::NewickBroker::push_top(const struct genesis::tree::NewickBrokerElement &) --> void", pybind11::arg("node"));
		cl.def("push_bottom", (void (genesis::tree::NewickBroker::*)(const struct genesis::tree::NewickBrokerElement &)) &genesis::tree::NewickBroker::push_bottom, "C++: genesis::tree::NewickBroker::push_bottom(const struct genesis::tree::NewickBrokerElement &) --> void", pybind11::arg("node"));
		cl.def("pop_top", (void (genesis::tree::NewickBroker::*)()) &genesis::tree::NewickBroker::pop_top, "C++: genesis::tree::NewickBroker::pop_top() --> void");
		cl.def("pop_bottom", (void (genesis::tree::NewickBroker::*)()) &genesis::tree::NewickBroker::pop_bottom, "C++: genesis::tree::NewickBroker::pop_bottom() --> void");
		cl.def("assign_ranks", (void (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::assign_ranks, "Iterate over the tree and assign ranks (= number of immediate children) to all nodes.\n\n This function is for example needed to check whether it is a bifurcating/binary tree, or to check\n how many leaves and inner nodes the tree has. Thus, it is usually called after the broker is\n filled with data.\n\nC++: genesis::tree::NewickBroker::assign_ranks() const --> void");
		cl.def("leaf_count", (unsigned long (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::leaf_count, "Returns the number of leaf nodes in the tree. assign_ranks() has to be called first.\n\nC++: genesis::tree::NewickBroker::leaf_count() const --> unsigned long");
		cl.def("inner_count", (unsigned long (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::inner_count, "C++: genesis::tree::NewickBroker::inner_count() const --> unsigned long");
		cl.def("node_count", (unsigned long (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::node_count, "Alias for size().\n\nC++: genesis::tree::NewickBroker::node_count() const --> unsigned long");
		cl.def("max_rank", (long (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::max_rank, "Returns the highest rank of the nodes in the tree. assign_ranks() has to be called first.\n\nC++: genesis::tree::NewickBroker::max_rank() const --> long");
		cl.def("is_bifurcating", (bool (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::is_bifurcating, "C++: genesis::tree::NewickBroker::is_bifurcating() const --> bool");
		cl.def("empty", (bool (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::empty, "Returns whether the stack is empty.\n\nC++: genesis::tree::NewickBroker::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::size, "Returns the size of the stack, i.e. the number of nodes stored in the broker.\n\nC++: genesis::tree::NewickBroker::size() const --> unsigned long");
		cl.def("__getitem__", (struct genesis::tree::NewickBrokerElement & (genesis::tree::NewickBroker::*)(unsigned long)) &genesis::tree::NewickBroker::operator[], "Provides index based array access to the nodes.\n\n This also allows to iterate over them using:\n\n     NewickBroker tb;\n     for (size_t i = 0; i < tb.size(); ++i) {\n        NewickBrokerElement* tn = tb[i];\n        std::cout << tn.name << std::endl;\n     }\n\n Caveat: this operator does no boundary check. If you need this check,\n use at() instead.\n\nC++: genesis::tree::NewickBroker::operator[](unsigned long) --> struct genesis::tree::NewickBrokerElement &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("at", (struct genesis::tree::NewickBrokerElement & (genesis::tree::NewickBroker::*)(unsigned long)) &genesis::tree::NewickBroker::at, "Provides index based array access to the nodes, doing a boundary check first.\n\n In out of bounds cases, an exception is thrown.\n\nC++: genesis::tree::NewickBroker::at(unsigned long) --> struct genesis::tree::NewickBrokerElement &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("top", (struct genesis::tree::NewickBrokerElement & (genesis::tree::NewickBroker::*)()) &genesis::tree::NewickBroker::top, "Returns a reference to the top node of the tree stack.\n\n Usually, the top element is the root of the tree (i.e., it has depth zero). Only when called\n during the broker is being filled with nodes (for example, while parsing a Newick tree),\n the top element is not the root.\n\n Calling this function on an empty() broker causes undefined behavior.\n\nC++: genesis::tree::NewickBroker::top() --> struct genesis::tree::NewickBrokerElement &", pybind11::return_value_policy::reference_internal);
		cl.def("bottom", (struct genesis::tree::NewickBrokerElement & (genesis::tree::NewickBroker::*)()) &genesis::tree::NewickBroker::bottom, "Returns a reference to the bottom node of the tree stack.\n\n Calling this function on an empty() broker causes undefined behavior.\n\nC++: genesis::tree::NewickBroker::bottom() --> struct genesis::tree::NewickBrokerElement &", pybind11::return_value_policy::reference_internal);
		cl.def("validate", (bool (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::validate, "Returns true iff the tree is valid. assign_ranks() has to be called first.\n\n A valid tree in a NewickBroker has to fullfill those criteria:\n\n  *  The depth (nesting level) of the nodes cannot increase more than one level between nodes,\n     as this would imply a non-existing node with a depth in between. However, it can arbitrarily\n     decrease, as this simply means the end of a subtree.\n  *  Furthermore, rank 1 is not valid, as this represents a node that is not furcating in any way.\n %\n\nC++: genesis::tree::NewickBroker::validate() const --> bool");
		cl.def("dump", (std::string (genesis::tree::NewickBroker::*)() const) &genesis::tree::NewickBroker::dump, "Return a readable string representation of the elements of the NewickBroker.\n\nC++: genesis::tree::NewickBroker::dump() const --> std::string");
	}
	{ // genesis::tree::CommonTreeNewickReaderPlugin file:genesis/tree/common_tree/newick_reader.hpp line:50
		pybind11::class_<genesis::tree::CommonTreeNewickReaderPlugin, std::shared_ptr<genesis::tree::CommonTreeNewickReaderPlugin>> cl(M("genesis::tree"), "CommonTreeNewickReaderPlugin", "Provide a set of plugin functions for NewickReader to read a #CommonTree.");
		cl.def( pybind11::init( [](){ return new genesis::tree::CommonTreeNewickReaderPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::tree::CommonTreeNewickReaderPlugin const &o){ return new genesis::tree::CommonTreeNewickReaderPlugin(o); } ) );
		cl.def("assign", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(const class genesis::tree::CommonTreeNewickReaderPlugin &)) &genesis::tree::CommonTreeNewickReaderPlugin::operator=, "C++: genesis::tree::CommonTreeNewickReaderPlugin::operator=(const class genesis::tree::CommonTreeNewickReaderPlugin &) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("default_branch_length", (double (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length, "Get the default branch length used when there is none given for an edge.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length() const --> double");
		cl.def("default_branch_length", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(double)) &genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length, "Set the default branch length used when there is none given for an edge.\n\n By default, this is `1.0`.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_branch_length(double) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("default_leaf_name", (const std::string & (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name, "Get the default named used when there is none given for a leaf node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("default_leaf_name", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(const std::string &)) &genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name, "Set the default named used when there is none given for a leaf node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_leaf_name(const std::string &) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("default_inner_name", (const std::string & (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name, "Get the default named used when there is none given for an inner node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("default_inner_name", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(const std::string &)) &genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name, "Set the default named used when there is none given for an inner node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_inner_name(const std::string &) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("default_root_name", (const std::string & (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::default_root_name, "Get the default named used when there is none given for the root node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_root_name() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("default_root_name", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(const std::string &)) &genesis::tree::CommonTreeNewickReaderPlugin::default_root_name, "Set the default named used when there is none given for the root node.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::default_root_name(const std::string &) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("set_default_names", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(const std::string &)) &genesis::tree::CommonTreeNewickReaderPlugin::set_default_names, "Shorthand to set the default names for leaf, inner and root node at once, to one\n value.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::set_default_names(const std::string &) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("use_default_names", (bool (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::use_default_names, "Return whether currently default names are activated in this plugin.\n\n See the setter use_default_names( bool ) for details.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::use_default_names() const --> bool");
		cl.def("use_default_names", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(bool)) &genesis::tree::CommonTreeNewickReaderPlugin::use_default_names, "Set whether to replace unnamed nodes with a default name.\n\n Common is `false`. In this case, nodes without names in the Newick tree are simply unnamed,\n i.e., their name is the emptry string.\n\n If set to `true`, unnamed nodes are named using one of the default names:\n\n  * Leaf nodes are named using default_leaf_name().\n  * Inner nodes are named using default_inner_name().\n  * The root node is named using default_root_name().\n\n These default names can be changed by using default_leaf_name( std::string const& ),\n default_inner_name( std::string const& ) and default_root_name( std::string const& ),\n or by using set_default_names( std::string const& ) to set all three at once.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::use_default_names(bool) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("replace_name_underscores", (bool (genesis::tree::CommonTreeNewickReaderPlugin::*)() const) &genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores, "Return whether currently this plugin replaces underscores with spaces.\n\n See the setter replace_name_underscores( bool ) for details.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores() const --> bool");
		cl.def("replace_name_underscores", (class genesis::tree::CommonTreeNewickReaderPlugin & (genesis::tree::CommonTreeNewickReaderPlugin::*)(bool)) &genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores, "Set whether to replace all underscores ('_') in names with spaces (' ').\n\n This is demanded by the original definition of the Newick format, see\n http://evolution.genetics.washington.edu/phylip/newicktree.html\n However, because under most common circumstances, it is more confusing than helpful, we\n decided to deactivate this by default. Thus, the default for this setting is `false`.\n\nC++: genesis::tree::CommonTreeNewickReaderPlugin::replace_name_underscores(bool) --> class genesis::tree::CommonTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("element_to_node", (void (genesis::tree::CommonTreeNewickReaderPlugin::*)(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeNode &) const) &genesis::tree::CommonTreeNewickReaderPlugin::element_to_node, "C++: genesis::tree::CommonTreeNewickReaderPlugin::element_to_node(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeNode &) const --> void", pybind11::arg("element"), pybind11::arg("node"));
		cl.def("element_to_edge", (void (genesis::tree::CommonTreeNewickReaderPlugin::*)(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const) &genesis::tree::CommonTreeNewickReaderPlugin::element_to_edge, "C++: genesis::tree::CommonTreeNewickReaderPlugin::element_to_edge(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const --> void", pybind11::arg("element"), pybind11::arg("edge"));
		cl.def("register_with", (void (genesis::tree::CommonTreeNewickReaderPlugin::*)(class genesis::tree::NewickReader &) const) &genesis::tree::CommonTreeNewickReaderPlugin::register_with, "C++: genesis::tree::CommonTreeNewickReaderPlugin::register_with(class genesis::tree::NewickReader &) const --> void", pybind11::arg("reader"));
	}
}
