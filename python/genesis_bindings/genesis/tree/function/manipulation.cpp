#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/manipulation.hpp>
#include <genesis/tree/function/operators.hpp>
#include <genesis/tree/printer/compact.hpp>
#include <genesis/tree/printer/detailed.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree/subtree.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
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

void bind_genesis_tree_function_manipulation(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::make_unrooted(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) file:genesis/tree/function/manipulation.hpp line:365
	M("genesis::tree").def("make_unrooted", [](class genesis::tree::Tree & a0) -> void { return genesis::tree::make_unrooted(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("make_unrooted", (void (*)(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>)) &genesis::tree::make_unrooted, "Unroot a Tree.\n\n The tree is expected to be rooted, which is checked via is_rooted(). The function then removes\n this root by calling delete_linear_node(). See there for details, and for the usage of\n \n\n \n make_rooted()\n\nC++: genesis::tree::make_unrooted(class genesis::tree::Tree &, class std::function<void (class genesis::tree::TreeEdge &, class genesis::tree::TreeEdge &)>) --> void", pybind11::arg("tree"), pybind11::arg("adjust_edges"));

	// genesis::tree::change_rooting(class genesis::tree::Tree &, class genesis::tree::TreeLink &) file:genesis/tree/function/manipulation.hpp line:398
	M("genesis::tree").def("change_rooting", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeLink &)) &genesis::tree::change_rooting, "\"Reroot\" the Tree at the given TreeLink.\n\n The function sets the root of the tree to the node of the given link. This operation does not\n change the topology of the tree, but merely adjusts some internal properties. The main changes\n are that Tree::root_node() and Tree::root_link() will return the new root after calling this\n function, and that tree iterators will start traversing the tree from this new root by default.\n\n There are three internal changes made to the tree data structure:\n\n   * All primary and secondary ends of the edges on the path between the new root and the old\n     root are swapped. This is because the edges now need to point towards the new root.\n   * Similarly, all (primary) links of the nodes on that path are changed so that they point\n     towards the new root.\n   * Also, the (internal) root_link_index is changed to the new root link. This is used for\n     the functions Tree::root_node() and Tree::root_link().\n\n The difference between this function and change_rooting( Tree& tree, TreeNode const& )\n is that when specifying a specific link, this link is used as the (primary) link of the new root\n node. This way, algorithms and iterators (e.g., IteratorLevelorder) will start traversing the\n tree in the direction of this link by default.\n When specifying a node for rerooting instead, the primary link of that node is used,\n so that iterators start traversing the tree in the direction of the old root instead.\n For most applications, this does not make a difference. However, there might be cases where the\n start directing makes a difference. Thus, we offer both versions of this function.\n\n The link needs to be part of the tree, otherwise an exception is thrown.\n\nC++: genesis::tree::change_rooting(class genesis::tree::Tree &, class genesis::tree::TreeLink &) --> void", pybind11::arg("tree"), pybind11::arg("at_link"));

	// genesis::tree::change_rooting(class genesis::tree::Tree &, class genesis::tree::TreeNode &) file:genesis/tree/function/manipulation.hpp line:407
	M("genesis::tree").def("change_rooting", (void (*)(class genesis::tree::Tree &, class genesis::tree::TreeNode &)) &genesis::tree::change_rooting, "Reroot the Tree at the given TreeNode.\n\n See change_rooting( Tree&, TreeLink& ) for details.\n\n The node needs to be part of the tree, otherwise an exception is thrown.\n\nC++: genesis::tree::change_rooting(class genesis::tree::Tree &, class genesis::tree::TreeNode &) --> void", pybind11::arg("tree"), pybind11::arg("at_node"));

	// genesis::tree::LadderizeOrder file:genesis/tree/function/manipulation.hpp line:413
	pybind11::enum_<genesis::tree::LadderizeOrder>(M("genesis::tree"), "LadderizeOrder", "")
		.value("kSmallFirst", genesis::tree::LadderizeOrder::kSmallFirst)
		.value("kLargeFirst", genesis::tree::LadderizeOrder::kLargeFirst);

;

	// genesis::tree::ladderize(class genesis::tree::Tree &, enum genesis::tree::LadderizeOrder) file:genesis/tree/function/manipulation.hpp line:426
	M("genesis::tree").def("ladderize", [](class genesis::tree::Tree & a0) -> void { return genesis::tree::ladderize(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("ladderize", (void (*)(class genesis::tree::Tree &, enum genesis::tree::LadderizeOrder)) &genesis::tree::ladderize, "Ladderize a Tree, that is, order its subtrees by size.\n\n The function flips the TreeLink order of all internal TreeNode%s of the Tree so that always\n the smalles/largest subtree (in number of nodes) comes first when iterating the Tree.\n This assumes a rooting, as the direction of the subtree of a node is measured away from the root.\n\nC++: genesis::tree::ladderize(class genesis::tree::Tree &, enum genesis::tree::LadderizeOrder) --> void", pybind11::arg("tree"), pybind11::arg("order"));

	{ // genesis::tree::PrinterCompact file:genesis/tree/printer/compact.hpp line:82
		pybind11::class_<genesis::tree::PrinterCompact, std::shared_ptr<genesis::tree::PrinterCompact>> cl(M("genesis::tree"), "PrinterCompact", "Print a Tree in a compact form, i.e., each node and edge on one line.\n\n For example, given the Newick tree\n\n     (((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;\n\n and using the print( Tree const& ) function for \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::tree::PrinterCompact(); } ) );
		cl.def( pybind11::init( [](genesis::tree::PrinterCompact const &o){ return new genesis::tree::PrinterCompact(o); } ) );
		cl.def("assign", (class genesis::tree::PrinterCompact & (genesis::tree::PrinterCompact::*)(const class genesis::tree::PrinterCompact &)) &genesis::tree::PrinterCompact::operator=, "Deault copy assignment.\n\nC++: genesis::tree::PrinterCompact::operator=(const class genesis::tree::PrinterCompact &) --> class genesis::tree::PrinterCompact &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("print", (void (genesis::tree::PrinterCompact::*)(std::ostream &, const class genesis::tree::Tree &, const class std::function<std::string (const class genesis::tree::TreeNode &, const class genesis::tree::TreeEdge &)>)) &genesis::tree::PrinterCompact::print, "Print a compact representation of a Tree to an output stream, using a given\n function for output of the Node and Edge data.\n\n The  function parameter is used to customize the output per printed line:\n The function takes the current TreeNode and TreeEdge, and returns the desired string\n representation for that line. See the implementation of print( Tree const& ) for an\n example.\n\nC++: genesis::tree::PrinterCompact::print(std::ostream &, const class genesis::tree::Tree &, const class std::function<std::string (const class genesis::tree::TreeNode &, const class genesis::tree::TreeEdge &)>) --> void", pybind11::arg("out"), pybind11::arg("tree"), pybind11::arg("print_line"));
		cl.def("print", (std::string (genesis::tree::PrinterCompact::*)(const class genesis::tree::Tree &, const class std::function<std::string (const class genesis::tree::TreeNode &, const class genesis::tree::TreeEdge &)>)) &genesis::tree::PrinterCompact::print, "Print a compact representation of a Tree to a `std::string`, using a given\n function for output of the Node and Edge data.\n\n See\n \n\n\n\n\nC++: genesis::tree::PrinterCompact::print(const class genesis::tree::Tree &, const class std::function<std::string (const class genesis::tree::TreeNode &, const class genesis::tree::TreeEdge &)>) --> std::string", pybind11::arg("tree"), pybind11::arg("print_line"));
		cl.def("print", (std::string (genesis::tree::PrinterCompact::*)(const class genesis::tree::Tree &)) &genesis::tree::PrinterCompact::print, "Print a compact representation of a Tree to a `std::string`, for\n \n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::PrinterCompact::print(const class genesis::tree::Tree &) --> std::string", pybind11::arg("tree"));
		cl.def("limit", (class genesis::tree::PrinterCompact & (genesis::tree::PrinterCompact::*)(long)) &genesis::tree::PrinterCompact::limit, "Set the line limit.\n\n If set to a negative value, the full tree is printed.\n For any value greater than 0, only this many lines are printed, following a preorder\n traversal of the tree. If the tree contains more nodes than being printed,\n an additional line with ellipsis is printed, to indicate this fact.\n\nC++: genesis::tree::PrinterCompact::limit(long) --> class genesis::tree::PrinterCompact &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("limit", (long (genesis::tree::PrinterCompact::*)() const) &genesis::tree::PrinterCompact::limit, "Get the current line limit.\n\n See limit( long ) for details.\n\nC++: genesis::tree::PrinterCompact::limit() const --> long");
	}
	{ // genesis::tree::PrinterDetailed file:genesis/tree/printer/detailed.hpp line:58
		pybind11::class_<genesis::tree::PrinterDetailed, std::shared_ptr<genesis::tree::PrinterDetailed>> cl(M("genesis::tree"), "PrinterDetailed", "Print a text representation of the Tree, showing all nodes, edges and links\n with their indices.");
		cl.def( pybind11::init( [](){ return new genesis::tree::PrinterDetailed(); } ) );
		cl.def( pybind11::init( [](genesis::tree::PrinterDetailed const &o){ return new genesis::tree::PrinterDetailed(o); } ) );
		cl.def("assign", (class genesis::tree::PrinterDetailed & (genesis::tree::PrinterDetailed::*)(const class genesis::tree::PrinterDetailed &)) &genesis::tree::PrinterDetailed::operator=, "C++: genesis::tree::PrinterDetailed::operator=(const class genesis::tree::PrinterDetailed &) --> class genesis::tree::PrinterDetailed &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("print", (void (genesis::tree::PrinterDetailed::*)(std::ostream &, const class genesis::tree::Tree &) const) &genesis::tree::PrinterDetailed::print, "C++: genesis::tree::PrinterDetailed::print(std::ostream &, const class genesis::tree::Tree &) const --> void", pybind11::arg("out"), pybind11::arg("tree"));
		cl.def("print", (std::string (genesis::tree::PrinterDetailed::*)(const class genesis::tree::Tree &) const) &genesis::tree::PrinterDetailed::print, "C++: genesis::tree::PrinterDetailed::print(const class genesis::tree::Tree &) const --> std::string", pybind11::arg("tree"));
		cl.def("use_color", (bool (genesis::tree::PrinterDetailed::*)() const) &genesis::tree::PrinterDetailed::use_color, "C++: genesis::tree::PrinterDetailed::use_color() const --> bool");
		cl.def("use_color", (class genesis::tree::PrinterDetailed & (genesis::tree::PrinterDetailed::*)(bool)) &genesis::tree::PrinterDetailed::use_color, "C++: genesis::tree::PrinterDetailed::use_color(bool) --> class genesis::tree::PrinterDetailed &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
	}
	{ // genesis::tree::Subtree file:genesis/tree/tree/subtree.hpp line:69
		pybind11::class_<genesis::tree::Subtree, std::shared_ptr<genesis::tree::Subtree>> cl(M("genesis::tree"), "Subtree", "Reference to a subtree of a Tree.\n\n This class refers to a particular subtree of a Tree.\n Such a subtree contains all TreeNode%s and TreeEdge%s at one side of a TreeEdge,\n while leaving out the other side of it.\n\n The \"subtree\" of a leaf node is only that node itself. There are no empty subtrees,\n and there is no subtree that contains all of the Tree. That is, a Subtree is always a split\n (or bipartition) of the Tree into two parts, separated by an edge.\n\n A subtree is defined by the TreeLink that is part of the subtree,\n while its \n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init<const class genesis::tree::TreeLink &>(), pybind11::arg("link") );

		cl.def( pybind11::init<const class genesis::tree::TreeNode &>(), pybind11::arg("node") );

		cl.def( pybind11::init( [](const class genesis::tree::TreeEdge & a0){ return new genesis::tree::Subtree(a0); } ), "doc" , pybind11::arg("edge"));
		cl.def( pybind11::init<const class genesis::tree::TreeEdge &, bool>(), pybind11::arg("edge"), pybind11::arg("use_primary") );

		cl.def( pybind11::init( [](genesis::tree::Subtree const &o){ return new genesis::tree::Subtree(o); } ) );
		cl.def("assign", (class genesis::tree::Subtree & (genesis::tree::Subtree::*)(const class genesis::tree::Subtree &)) &genesis::tree::Subtree::operator=, "C++: genesis::tree::Subtree::operator=(const class genesis::tree::Subtree &) --> class genesis::tree::Subtree &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("link", (const class genesis::tree::TreeLink & (genesis::tree::Subtree::*)() const) &genesis::tree::Subtree::link, "Get the TreeLink that separates the subtree from the rest of the tree.\n\n This link belongs to the subtree, while its outer() does not.\n\nC++: genesis::tree::Subtree::link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (const class genesis::tree::TreeNode & (genesis::tree::Subtree::*)() const) &genesis::tree::Subtree::node, "Get the TreeNode at which the subtree starts.\n\nC++: genesis::tree::Subtree::node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (const class genesis::tree::TreeEdge & (genesis::tree::Subtree::*)() const) &genesis::tree::Subtree::edge, "Get the TreeEdge that separates the subtree from the rest of the tree.\n\nC++: genesis::tree::Subtree::edge() const --> const class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("invert", (void (genesis::tree::Subtree::*)()) &genesis::tree::Subtree::invert, "Invert/flip the subtree to represent the other (complementary) part of the tree.\n\nC++: genesis::tree::Subtree::invert() --> void");
	}
	// genesis::tree::equal(const class genesis::tree::Tree &, const class genesis::tree::Tree &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>) file:genesis/tree/function/operators.hpp line:189
	M("genesis::tree").def("equal", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>)) &genesis::tree::equal, "Compare two trees for equality given binary comparator functionals for their nodes and\n edges.\n\n This function does a preorder traversal of both trees in parallel and calls the comparator\n functionals for each position of the iterator. It returns true iff the comparators are true for\n every position.\n\n The comparator functionals can be either function pointers, function objects, or lambda\n expressions.\n\n As the traversal is done in parallel, the trees are also checked for equal topology:\n their elements (links, nodes, edges) have to be equal in size and the degree of each node during\n the traversal has to be identical in both trees. Those assumptions are made because two trees\n that do not have identical topology are never considered equal.\n\nC++: genesis::tree::equal(const class genesis::tree::Tree &, const class genesis::tree::Tree &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("node_comparator"), pybind11::arg("edge_comparator"));

	// genesis::tree::equal(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>) file:genesis/tree/function/operators.hpp line:202
	M("genesis::tree").def("equal", (bool (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>)) &genesis::tree::equal, "Compare all trees for equality given binary comparator functionals for their nodes and\n edges.\n\n \n \n\n\nC++: genesis::tree::equal(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, class std::function<bool (const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)>, class std::function<bool (const class genesis::tree::TreeEdge &, const class genesis::tree::TreeEdge &)>) --> bool", pybind11::arg("trees"), pybind11::arg("node_comparator"), pybind11::arg("edge_comparator"));

}
