#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/functions.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/drawing/circular_layout.hpp>
#include <genesis/tree/drawing/heat_tree.hpp>
#include <genesis/tree/drawing/layout_base.hpp>
#include <genesis/tree/drawing/rectangular_layout.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/map.hpp>
#include <genesis/utils/color/normalization.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/formats/svg/document.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/object.hpp>
#include <genesis/utils/io/base_output_target.hpp>
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

// genesis::tree::CircularLayout file:genesis/tree/drawing/circular_layout.hpp line:47
struct PyCallBack_genesis_tree_CircularLayout : public genesis::tree::CircularLayout {
	using genesis::tree::CircularLayout::CircularLayout;

	class genesis::utils::SvgDocument to_svg_document_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::tree::CircularLayout *>(this), "to_svg_document_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<class genesis::utils::SvgDocument>::value) {
				static pybind11::detail::override_caster_t<class genesis::utils::SvgDocument> caster;
				return pybind11::detail::cast_ref<class genesis::utils::SvgDocument>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class genesis::utils::SvgDocument>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"LayoutBase::to_svg_document_\"");
	}
};

// genesis::tree::RectangularLayout file:genesis/tree/drawing/rectangular_layout.hpp line:47
struct PyCallBack_genesis_tree_RectangularLayout : public genesis::tree::RectangularLayout {
	using genesis::tree::RectangularLayout::RectangularLayout;

	class genesis::utils::SvgDocument to_svg_document_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::tree::RectangularLayout *>(this), "to_svg_document_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<class genesis::utils::SvgDocument>::value) {
				static pybind11::detail::override_caster_t<class genesis::utils::SvgDocument> caster;
				return pybind11::detail::cast_ref<class genesis::utils::SvgDocument>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<class genesis::utils::SvgDocument>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"LayoutBase::to_svg_document_\"");
	}
};

void bind_genesis_tree_drawing_heat_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &) file:genesis/tree/drawing/heat_tree.hpp line:77
	M("genesis::tree").def("heat_tree", (class genesis::utils::SvgDocument (*)(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &)) &genesis::tree::heat_tree, "C++: genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &) --> class genesis::utils::SvgDocument", pybind11::arg("params"), pybind11::arg("matrix_color_map"), pybind11::arg("matrix_color_norm"), pybind11::arg("tree_color_map"), pybind11::arg("tree_color_norm"));

	{ // genesis::tree::CircularLayout file:genesis/tree/drawing/circular_layout.hpp line:47
		pybind11::class_<genesis::tree::CircularLayout, std::shared_ptr<genesis::tree::CircularLayout>, PyCallBack_genesis_tree_CircularLayout, genesis::tree::LayoutBase> cl(M("genesis::tree"), "CircularLayout", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::CircularLayout(); }, [](){ return new PyCallBack_genesis_tree_CircularLayout(); } ) );
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0){ return new genesis::tree::CircularLayout(a0); }, [](const class genesis::tree::Tree & a0){ return new PyCallBack_genesis_tree_CircularLayout(a0); } ), "doc");
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0, const enum genesis::tree::LayoutType & a1){ return new genesis::tree::CircularLayout(a0, a1); }, [](const class genesis::tree::Tree & a0, const enum genesis::tree::LayoutType & a1){ return new PyCallBack_genesis_tree_CircularLayout(a0, a1); } ), "doc");
		cl.def( pybind11::init<const class genesis::tree::Tree &, const enum genesis::tree::LayoutType, bool>(), pybind11::arg("orig_tree"), pybind11::arg("drawing_type"), pybind11::arg("ladderize") );

		cl.def( pybind11::init( [](PyCallBack_genesis_tree_CircularLayout const &o){ return new PyCallBack_genesis_tree_CircularLayout(o); } ) );
		cl.def( pybind11::init( [](genesis::tree::CircularLayout const &o){ return new genesis::tree::CircularLayout(o); } ) );
		cl.def("assign", (class genesis::tree::CircularLayout & (genesis::tree::CircularLayout::*)(const class genesis::tree::CircularLayout &)) &genesis::tree::CircularLayout::operator=, "C++: genesis::tree::CircularLayout::operator=(const class genesis::tree::CircularLayout &) --> class genesis::tree::CircularLayout &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("radius", (class genesis::tree::CircularLayout & (genesis::tree::CircularLayout::*)(const double)) &genesis::tree::CircularLayout::radius, "C++: genesis::tree::CircularLayout::radius(const double) --> class genesis::tree::CircularLayout &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("radius", (double (genesis::tree::CircularLayout::*)() const) &genesis::tree::CircularLayout::radius, "C++: genesis::tree::CircularLayout::radius() const --> double");
	}
	{ // genesis::tree::RectangularLayout file:genesis/tree/drawing/rectangular_layout.hpp line:47
		pybind11::class_<genesis::tree::RectangularLayout, std::shared_ptr<genesis::tree::RectangularLayout>, PyCallBack_genesis_tree_RectangularLayout, genesis::tree::LayoutBase> cl(M("genesis::tree"), "RectangularLayout", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::RectangularLayout(); }, [](){ return new PyCallBack_genesis_tree_RectangularLayout(); } ) );
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0){ return new genesis::tree::RectangularLayout(a0); }, [](const class genesis::tree::Tree & a0){ return new PyCallBack_genesis_tree_RectangularLayout(a0); } ), "doc");
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0, const enum genesis::tree::LayoutType & a1){ return new genesis::tree::RectangularLayout(a0, a1); }, [](const class genesis::tree::Tree & a0, const enum genesis::tree::LayoutType & a1){ return new PyCallBack_genesis_tree_RectangularLayout(a0, a1); } ), "doc");
		cl.def( pybind11::init<const class genesis::tree::Tree &, const enum genesis::tree::LayoutType, bool>(), pybind11::arg("orig_tree"), pybind11::arg("drawing_type"), pybind11::arg("ladderize") );

		cl.def( pybind11::init( [](PyCallBack_genesis_tree_RectangularLayout const &o){ return new PyCallBack_genesis_tree_RectangularLayout(o); } ) );
		cl.def( pybind11::init( [](genesis::tree::RectangularLayout const &o){ return new genesis::tree::RectangularLayout(o); } ) );
		cl.def("assign", (class genesis::tree::RectangularLayout & (genesis::tree::RectangularLayout::*)(const class genesis::tree::RectangularLayout &)) &genesis::tree::RectangularLayout::operator=, "C++: genesis::tree::RectangularLayout::operator=(const class genesis::tree::RectangularLayout &) --> class genesis::tree::RectangularLayout &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("width", (class genesis::tree::RectangularLayout & (genesis::tree::RectangularLayout::*)(const double)) &genesis::tree::RectangularLayout::width, "C++: genesis::tree::RectangularLayout::width(const double) --> class genesis::tree::RectangularLayout &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("width", (double (genesis::tree::RectangularLayout::*)() const) &genesis::tree::RectangularLayout::width, "C++: genesis::tree::RectangularLayout::width() const --> double");
		cl.def("height", (class genesis::tree::RectangularLayout & (genesis::tree::RectangularLayout::*)(const double)) &genesis::tree::RectangularLayout::height, "C++: genesis::tree::RectangularLayout::height(const double) --> class genesis::tree::RectangularLayout &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("height", (double (genesis::tree::RectangularLayout::*)() const) &genesis::tree::RectangularLayout::height, "C++: genesis::tree::RectangularLayout::height() const --> double");
	}
	// genesis::tree::node_names(const class genesis::tree::Tree &, bool) file:genesis/tree/common_tree/functions.hpp line:62
	M("genesis::tree").def("node_names", [](const class genesis::tree::Tree & a0) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::tree::node_names(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("node_names", (class std::vector<std::string, class std::allocator<std::string > > (*)(const class genesis::tree::Tree &, bool)) &genesis::tree::node_names, "Returns a list of all TreeNode names of a Tree.\n\n If `leaves_only` is set to true, nodes names of inner nodes are not included.\n Unnamed nodes (`node.data.name == \"\"`) are always excluded.\n The result is not sorted, and names are as given in the Tree (including possible duplicates).\n\n The provided Tree needs to have TreeNode%s with data types deriveed from CommonNodeData.\n\nC++: genesis::tree::node_names(const class genesis::tree::Tree &, bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("tree"), pybind11::arg("leaves_only"));

	// genesis::tree::node_names(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool) file:genesis/tree/common_tree/functions.hpp line:74
	M("genesis::tree").def("node_names", [](const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > & a0) -> std::vector<std::string, class std::allocator<std::string > > { return genesis::tree::node_names(a0); }, "", pybind11::arg("tree_set"));
	M("genesis::tree").def("node_names", (class std::vector<std::string, class std::allocator<std::string > > (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool)) &genesis::tree::node_names, "Returns a set of all TreeNode names of a vector of Tree%s or a TreeSet.\n\n The function returns the set of all names of all Tree%s in the set. See\n \n\n\n\n\nC++: genesis::tree::node_names(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool) --> class std::vector<std::string, class std::allocator<std::string > >", pybind11::arg("tree_set"), pybind11::arg("leaves_only"));

	// genesis::tree::find_node(const class genesis::tree::Tree &, const std::string &, bool, bool) file:genesis/tree/common_tree/functions.hpp line:86
	M("genesis::tree").def("find_node", [](const class genesis::tree::Tree & a0, const std::string & a1) -> const genesis::tree::TreeNode * { return genesis::tree::find_node(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"));
	M("genesis::tree").def("find_node", [](const class genesis::tree::Tree & a0, const std::string & a1, bool const & a2) -> const genesis::tree::TreeNode * { return genesis::tree::find_node(a0, a1, a2); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"), pybind11::arg("throw_on_failure"));
	M("genesis::tree").def("find_node", (const class genesis::tree::TreeNode * (*)(const class genesis::tree::Tree &, const std::string &, bool, bool)) &genesis::tree::find_node, "Finds a Node, given its name.\n\n If not found, by default, `nullptr` is returned. If however  is set to `true`,\n an exception is thrown instead. This is useful if the continuation of the calling function\n does not make sense without having found the node.\n\nC++: genesis::tree::find_node(const class genesis::tree::Tree &, const std::string &, bool, bool) --> const class genesis::tree::TreeNode *", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"), pybind11::arg("throw_on_failure"), pybind11::arg("replace_underscores"));

	// genesis::tree::find_node(class genesis::tree::Tree &, const std::string &, bool, bool) file:genesis/tree/common_tree/functions.hpp line:98
	M("genesis::tree").def("find_node", [](class genesis::tree::Tree & a0, const std::string & a1) -> genesis::tree::TreeNode * { return genesis::tree::find_node(a0, a1); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"));
	M("genesis::tree").def("find_node", [](class genesis::tree::Tree & a0, const std::string & a1, bool const & a2) -> genesis::tree::TreeNode * { return genesis::tree::find_node(a0, a1, a2); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"), pybind11::arg("throw_on_failure"));
	M("genesis::tree").def("find_node", (class genesis::tree::TreeNode * (*)(class genesis::tree::Tree &, const std::string &, bool, bool)) &genesis::tree::find_node, "Finds a Node, given its name. If not found, nullptr is returned.\n\n  find_node( Tree const&,std::string const&, bool, bool )\n\nC++: genesis::tree::find_node(class genesis::tree::Tree &, const std::string &, bool, bool) --> class genesis::tree::TreeNode *", pybind11::return_value_policy::reference_internal, pybind11::arg("tree"), pybind11::arg("name"), pybind11::arg("throw_on_failure"), pybind11::arg("replace_underscores"));

	// genesis::tree::find_nodes(const class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool) file:genesis/tree/common_tree/functions.hpp line:112
	M("genesis::tree").def("find_nodes", [](const class genesis::tree::Tree & a0, const class std::vector<std::string, class std::allocator<std::string > > & a1) -> std::vector<const class genesis::tree::TreeNode *, class std::allocator<const class genesis::tree::TreeNode *> > { return genesis::tree::find_nodes(a0, a1); }, "", pybind11::arg("tree"), pybind11::arg("node_names"));
	M("genesis::tree").def("find_nodes", [](const class genesis::tree::Tree & a0, const class std::vector<std::string, class std::allocator<std::string > > & a1, bool const & a2) -> std::vector<const class genesis::tree::TreeNode *, class std::allocator<const class genesis::tree::TreeNode *> > { return genesis::tree::find_nodes(a0, a1, a2); }, "", pybind11::arg("tree"), pybind11::arg("node_names"), pybind11::arg("throw_on_failure"));
	M("genesis::tree").def("find_nodes", (class std::vector<const class genesis::tree::TreeNode *, class std::allocator<const class genesis::tree::TreeNode *> > (*)(const class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool)) &genesis::tree::find_nodes, "Find TreeNode%s in a Tree, given their name.\n\n If a particular node is not found, by default, the respective entry is a `nullptr`.\n If however  is set to `true`, an exception is thrown instead. This is useful\n if the continuation of the calling function does not make sense without having found the node.\n\nC++: genesis::tree::find_nodes(const class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool) --> class std::vector<const class genesis::tree::TreeNode *, class std::allocator<const class genesis::tree::TreeNode *> >", pybind11::arg("tree"), pybind11::arg("node_names"), pybind11::arg("throw_on_failure"), pybind11::arg("replace_underscores"));

	// genesis::tree::find_nodes(class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool) file:genesis/tree/common_tree/functions.hpp line:124
	M("genesis::tree").def("find_nodes", [](class genesis::tree::Tree & a0, const class std::vector<std::string, class std::allocator<std::string > > & a1) -> std::vector<class genesis::tree::TreeNode *, class std::allocator<class genesis::tree::TreeNode *> > { return genesis::tree::find_nodes(a0, a1); }, "", pybind11::arg("tree"), pybind11::arg("node_names"));
	M("genesis::tree").def("find_nodes", [](class genesis::tree::Tree & a0, const class std::vector<std::string, class std::allocator<std::string > > & a1, bool const & a2) -> std::vector<class genesis::tree::TreeNode *, class std::allocator<class genesis::tree::TreeNode *> > { return genesis::tree::find_nodes(a0, a1, a2); }, "", pybind11::arg("tree"), pybind11::arg("node_names"), pybind11::arg("throw_on_failure"));
	M("genesis::tree").def("find_nodes", (class std::vector<class genesis::tree::TreeNode *, class std::allocator<class genesis::tree::TreeNode *> > (*)(class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool)) &genesis::tree::find_nodes, "Find TreeNode%s in a Tree, given their name.\n\n  find_nodes( Tree const&, std::vector<std::string> const&, bool, bool )\n\nC++: genesis::tree::find_nodes(class genesis::tree::Tree &, const class std::vector<std::string, class std::allocator<std::string > > &, bool, bool) --> class std::vector<class genesis::tree::TreeNode *, class std::allocator<class genesis::tree::TreeNode *> >", pybind11::arg("tree"), pybind11::arg("node_names"), pybind11::arg("throw_on_failure"), pybind11::arg("replace_underscores"));

	// genesis::tree::length(const class genesis::tree::Tree &) file:genesis/tree/common_tree/functions.hpp line:138
	M("genesis::tree").def("length", (double (*)(const class genesis::tree::Tree &)) &genesis::tree::length, "Get the length of the tree, i.e., the sum of all branch lengths.\n\nC++: genesis::tree::length(const class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::height(const class genesis::tree::Tree &) file:genesis/tree/common_tree/functions.hpp line:144
	M("genesis::tree").def("height", (double (*)(const class genesis::tree::Tree &)) &genesis::tree::height, "Get the height of the tree, i.e., the longest distance from the root to a leaf,\n measured using the branch_length.\n\nC++: genesis::tree::height(const class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::diameter(const class genesis::tree::Tree &) file:genesis/tree/common_tree/functions.hpp line:150
	M("genesis::tree").def("diameter", (double (*)(const class genesis::tree::Tree &)) &genesis::tree::diameter, "Get the diameter of the tree, i.e., the longest distance between any two nodes,\n measured using the branch_length.\n\nC++: genesis::tree::diameter(const class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

}
