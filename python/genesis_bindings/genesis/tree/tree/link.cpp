#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/operators.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <ios>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

void bind_genesis_tree_tree_link(std::function< pybind11::module &(std::string const &namespace_) > &M)
{

	extend_tree_functions(M("genesis::tree"));
	{ // genesis::tree::TreeLink file:genesis/tree/tree/link.hpp line:52
		pybind11::class_<genesis::tree::TreeLink, std::shared_ptr<genesis::tree::TreeLink>> cl(M("genesis::tree"), "TreeLink", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::TreeLink(); } ) );
		cl.def( pybind11::init<unsigned long, class genesis::tree::TreeLink *, class genesis::tree::TreeLink *, class genesis::tree::TreeNode *, class genesis::tree::TreeEdge *>(), pybind11::arg("index"), pybind11::arg("next"), pybind11::arg("outer"), pybind11::arg("node"), pybind11::arg("edge") );

		cl.def("index", (unsigned long (genesis::tree::TreeLink::*)() const) &genesis::tree::TreeLink::index, "Return the index of this Link.\n\nC++: genesis::tree::TreeLink::index() const --> unsigned long");
		cl.def("next", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)()) &genesis::tree::TreeLink::next, "Return the next TreeLink within the TreeNode of this link.\n\nC++: genesis::tree::TreeLink::next() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("prev", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)()) &genesis::tree::TreeLink::prev, "Return the previous TreeLink within the TreeNode of this link.\n\n The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.\n As this link first has to be found, this function is not as cheap as next().\n\nC++: genesis::tree::TreeLink::prev() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("outer", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)()) &genesis::tree::TreeLink::outer, "Return the TreeLink of the adjacent TreeNode.\n\nC++: genesis::tree::TreeLink::outer() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (class genesis::tree::TreeEdge & (genesis::tree::TreeLink::*)()) &genesis::tree::TreeLink::edge, "Return the TreeEdge of this TreeLink.\n\nC++: genesis::tree::TreeLink::edge() --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (class genesis::tree::TreeNode & (genesis::tree::TreeLink::*)()) &genesis::tree::TreeLink::node, "Return the TreeNode of this TreeLink.\n\nC++: genesis::tree::TreeLink::node() --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("reset_index", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)(unsigned long)) &genesis::tree::TreeLink::reset_index, "Reset the internal index of this TreeLink.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n The index is an invariant that needs to be kept, as it needs to match the index in the Tree\n container.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeLink::reset_index(unsigned long) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_next", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)(class genesis::tree::TreeLink *)) &genesis::tree::TreeLink::reset_next, "Reset the internal pointer to the next TreeLink of this TreeLink.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeLink::reset_next(class genesis::tree::TreeLink *) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_outer", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)(class genesis::tree::TreeLink *)) &genesis::tree::TreeLink::reset_outer, "Reset the internal pointer to the outer TreeLink of this TreeLink.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeLink::reset_outer(class genesis::tree::TreeLink *) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_node", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)(class genesis::tree::TreeNode *)) &genesis::tree::TreeLink::reset_node, "Reset the internal pointer to the TreeNode of this TreeLink.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeLink::reset_node(class genesis::tree::TreeNode *) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_edge", (class genesis::tree::TreeLink & (genesis::tree::TreeLink::*)(class genesis::tree::TreeEdge *)) &genesis::tree::TreeLink::reset_edge, "Reset the internal pointer to the TreeEdge of this TreeLink.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeLink::reset_edge(class genesis::tree::TreeEdge *) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));

		cl.def("__str__", [](genesis::tree::TreeLink const &o) -> std::string { std::ostringstream s; genesis::tree::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::tree::TreeNode file:genesis/tree/tree/node.hpp line:58
		pybind11::class_<genesis::tree::TreeNode, std::shared_ptr<genesis::tree::TreeNode>> cl(M("genesis::tree"), "TreeNode", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::TreeNode(); } ) );
		cl.def( pybind11::init<unsigned long, class genesis::tree::TreeLink *>(), pybind11::arg("index"), pybind11::arg("primary_link") );

		cl.def("data", (class genesis::tree::CommonNodeData & (genesis::tree::TreeNode::*)()) &genesis::tree::TreeNode::data<genesis::tree::CommonNodeData>, "C++: genesis::tree::TreeNode::data() --> class genesis::tree::CommonNodeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (class genesis::tree::SimpleNewickNodeData & (genesis::tree::TreeNode::*)()) &genesis::tree::TreeNode::data<genesis::tree::SimpleNewickNodeData>, "C++: genesis::tree::TreeNode::data() --> class genesis::tree::SimpleNewickNodeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (const class genesis::tree::CommonNodeData & (genesis::tree::TreeNode::*)() const) &genesis::tree::TreeNode::data<genesis::tree::CommonNodeData>, "C++: genesis::tree::TreeNode::data() const --> const class genesis::tree::CommonNodeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (const class genesis::tree::SimpleNewickNodeData & (genesis::tree::TreeNode::*)() const) &genesis::tree::TreeNode::data<genesis::tree::SimpleNewickNodeData>, "C++: genesis::tree::TreeNode::data() const --> const class genesis::tree::SimpleNewickNodeData &", pybind11::return_value_policy::reference_internal);
		cl.def("index", (unsigned long (genesis::tree::TreeNode::*)() const) &genesis::tree::TreeNode::index, "Return the index of this Node.\n\nC++: genesis::tree::TreeNode::index() const --> unsigned long");
		cl.def("primary_link", (class genesis::tree::TreeLink & (genesis::tree::TreeNode::*)()) &genesis::tree::TreeNode::primary_link, "Return the TreeLink that points towards the root.\n\nC++: genesis::tree::TreeNode::primary_link() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("link", (class genesis::tree::TreeLink & (genesis::tree::TreeNode::*)()) &genesis::tree::TreeNode::link, "Return the TreeLink that points towards the root.\n\n This is just an alias for primary_link(), that is shorter to use when needed\n frequently in an algorithm.\n\nC++: genesis::tree::TreeNode::link() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("primary_edge", (class genesis::tree::TreeEdge & (genesis::tree::TreeNode::*)()) &genesis::tree::TreeNode::primary_edge, "Return the TreeEdge that points towards the root.\n\nC++: genesis::tree::TreeNode::primary_edge() --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("has_data", (bool (genesis::tree::TreeNode::*)() const) &genesis::tree::TreeNode::has_data, "Return `true` if the TreeNode has a data object assigned to it.\n\nC++: genesis::tree::TreeNode::has_data() const --> bool");
		cl.def("reset_index", (class genesis::tree::TreeNode & (genesis::tree::TreeNode::*)(unsigned long)) &genesis::tree::TreeNode::reset_index, "Reset the internal index of this TreeNode.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n The index is an invariant that needs to be kept, as it needs to match the index in the Tree\n container.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeNode::reset_index(unsigned long) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_primary_link", (class genesis::tree::TreeNode & (genesis::tree::TreeNode::*)(class genesis::tree::TreeLink *)) &genesis::tree::TreeNode::reset_primary_link, "Reset the internal pointer to the TreeLink of this TreeNode.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeNode::reset_primary_link(class genesis::tree::TreeLink *) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));

		cl.def("__str__", [](genesis::tree::TreeNode const &o) -> std::string { std::ostringstream s; genesis::tree::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::tree::TreeEdge file:genesis/tree/tree/edge.hpp line:60
		pybind11::class_<genesis::tree::TreeEdge, std::shared_ptr<genesis::tree::TreeEdge>> cl(M("genesis::tree"), "TreeEdge", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::TreeEdge(); } ) );
		cl.def( pybind11::init<unsigned long, class genesis::tree::TreeLink *, class genesis::tree::TreeLink *>(), pybind11::arg("index"), pybind11::arg("primary_link"), pybind11::arg("secondary_link") );

		cl.def("data", (class genesis::tree::CommonEdgeData & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::data<genesis::tree::CommonEdgeData>, "C++: genesis::tree::TreeEdge::data() --> class genesis::tree::CommonEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (class genesis::tree::SimpleNewickEdgeData & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::data<genesis::tree::SimpleNewickEdgeData>, "C++: genesis::tree::TreeEdge::data() --> class genesis::tree::SimpleNewickEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (class genesis::placement::PlacementEdgeData & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::data<genesis::placement::PlacementEdgeData>, "C++: genesis::tree::TreeEdge::data() --> class genesis::placement::PlacementEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (const class genesis::tree::CommonEdgeData & (genesis::tree::TreeEdge::*)() const) &genesis::tree::TreeEdge::data<genesis::tree::CommonEdgeData>, "C++: genesis::tree::TreeEdge::data() const --> const class genesis::tree::CommonEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (const class genesis::tree::SimpleNewickEdgeData & (genesis::tree::TreeEdge::*)() const) &genesis::tree::TreeEdge::data<genesis::tree::SimpleNewickEdgeData>, "C++: genesis::tree::TreeEdge::data() const --> const class genesis::tree::SimpleNewickEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("data", (const class genesis::placement::PlacementEdgeData & (genesis::tree::TreeEdge::*)() const) &genesis::tree::TreeEdge::data<genesis::placement::PlacementEdgeData>, "C++: genesis::tree::TreeEdge::data() const --> const class genesis::placement::PlacementEdgeData &", pybind11::return_value_policy::reference_internal);
		cl.def("index", (unsigned long (genesis::tree::TreeEdge::*)() const) &genesis::tree::TreeEdge::index, "Return the index of this Edge.\n\nC++: genesis::tree::TreeEdge::index() const --> unsigned long");
		cl.def("primary_link", (class genesis::tree::TreeLink & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::primary_link, "Return the TreeLink of this TreeEdge that points towards the root.\n\nC++: genesis::tree::TreeEdge::primary_link() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("secondary_link", (class genesis::tree::TreeLink & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::secondary_link, "Return the TreeLink of this TreeEdge that points away from the root.\n\nC++: genesis::tree::TreeEdge::secondary_link() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("primary_node", (class genesis::tree::TreeNode & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::primary_node, "Return the TreeNode of this TreeEdge that points towards the root.\n\nC++: genesis::tree::TreeEdge::primary_node() --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("secondary_node", (class genesis::tree::TreeNode & (genesis::tree::TreeEdge::*)()) &genesis::tree::TreeEdge::secondary_node, "Return the TreeNode of this TreeEdge that points away from the root.\n\nC++: genesis::tree::TreeEdge::secondary_node() --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("has_data", (bool (genesis::tree::TreeEdge::*)() const) &genesis::tree::TreeEdge::has_data, "Return `true` if the TreeEdge has a data object assigned to it.\n\nC++: genesis::tree::TreeEdge::has_data() const --> bool");
		cl.def("reset_index", (class genesis::tree::TreeEdge & (genesis::tree::TreeEdge::*)(unsigned long)) &genesis::tree::TreeEdge::reset_index, "Reset the internal index of this TreeEdge.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n The index is an invariant that needs to be kept, as it needs to match the index in the Tree\n container.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeEdge::reset_index(unsigned long) --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_primary_link", (class genesis::tree::TreeEdge & (genesis::tree::TreeEdge::*)(class genesis::tree::TreeLink *)) &genesis::tree::TreeEdge::reset_primary_link, "Reset the internal pointer to the primary TreeLink of this TreeEdge.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeEdge::reset_primary_link(class genesis::tree::TreeLink *) --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("reset_secondary_link", (class genesis::tree::TreeEdge & (genesis::tree::TreeEdge::*)(class genesis::tree::TreeLink *)) &genesis::tree::TreeEdge::reset_secondary_link, "Reset the internal pointer to the secondary TreeLink of this TreeEdge.\n\n This is a helper function that needs to be used with care and only in cases where appropriate.\n\n This function exists to allow building and modifying a Tree without the need for many friend\n declarations. However, the function should rarely be needed outside of this context.\n\nC++: genesis::tree::TreeEdge::reset_secondary_link(class genesis::tree::TreeLink *) --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));

		cl.def("__str__", [](genesis::tree::TreeEdge const &o) -> std::string { std::ostringstream s; genesis::tree::operator<<(s, o); return s.str(); } );
	}
}
