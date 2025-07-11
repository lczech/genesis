#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/drawing/heat_tree.hpp>
#include <genesis/tree/drawing/layout_base.hpp>
#include <genesis/tree/drawing/layout_tree.hpp>
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
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/document.hpp>
#include <genesis/utils/formats/svg/group.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/object.hpp>
#include <genesis/utils/formats/svg/text.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

// genesis::tree::LayoutNodeData file:genesis/tree/drawing/layout_tree.hpp line:80
struct PyCallBack_genesis_tree_LayoutNodeData : public genesis::tree::LayoutNodeData {
	using genesis::tree::LayoutNodeData::LayoutNodeData;

};

// genesis::tree::LayoutEdgeData file:genesis/tree/drawing/layout_tree.hpp line:163
struct PyCallBack_genesis_tree_LayoutEdgeData : public genesis::tree::LayoutEdgeData {
	using genesis::tree::LayoutEdgeData::LayoutEdgeData;

};

// genesis::tree::LayoutBase file:genesis/tree/drawing/layout_base.hpp line:91
struct PyCallBack_genesis_tree_LayoutBase : public genesis::tree::LayoutBase {
	using genesis::tree::LayoutBase::LayoutBase;

	class genesis::utils::SvgDocument to_svg_document_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::tree::LayoutBase *>(this), "to_svg_document_");
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

void bind_genesis_tree_drawing_layout_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::LayoutNodeData file:genesis/tree/drawing/layout_tree.hpp line:80
		pybind11::class_<genesis::tree::LayoutNodeData, std::shared_ptr<genesis::tree::LayoutNodeData>, PyCallBack_genesis_tree_LayoutNodeData, genesis::tree::CommonNodeData> cl(M("genesis::tree"), "LayoutNodeData", "Data class for ::LayoutTreeNode%s.\n\n It is derived from tree::CommonNodeData, so that it already contains a\n \n\n");
		cl.def_readwrite("distance", &genesis::tree::LayoutNodeData::distance);
		cl.def_readwrite("spreading", &genesis::tree::LayoutNodeData::spreading);
		cl.def_readwrite("parent_index", &genesis::tree::LayoutNodeData::parent_index);
		cl.def_readwrite("shape", &genesis::tree::LayoutNodeData::shape);
		cl.def_readwrite("spacer_stroke", &genesis::tree::LayoutNodeData::spacer_stroke);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::LayoutNodeData, struct std::default_delete<class genesis::tree::LayoutNodeData> > (*)()) &genesis::tree::LayoutNodeData::create, "C++: genesis::tree::LayoutNodeData::create() --> class std::unique_ptr<class genesis::tree::LayoutNodeData, struct std::default_delete<class genesis::tree::LayoutNodeData> >");
	}
	{ // genesis::tree::LayoutEdgeData file:genesis/tree/drawing/layout_tree.hpp line:163
		pybind11::class_<genesis::tree::LayoutEdgeData, std::shared_ptr<genesis::tree::LayoutEdgeData>, PyCallBack_genesis_tree_LayoutEdgeData, genesis::tree::CommonEdgeData> cl(M("genesis::tree"), "LayoutEdgeData", "Data class for LayoutTreeEdge%s.\n\n It is derived from tree::CommonEdgeData, so that it already contains a\n \n\n");
		cl.def_readwrite("spreading_stroke", &genesis::tree::LayoutEdgeData::spreading_stroke);
		cl.def_readwrite("distance_stroke", &genesis::tree::LayoutEdgeData::distance_stroke);
		cl.def_readwrite("shape", &genesis::tree::LayoutEdgeData::shape);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::LayoutEdgeData, struct std::default_delete<class genesis::tree::LayoutEdgeData> > (*)()) &genesis::tree::LayoutEdgeData::create, "C++: genesis::tree::LayoutEdgeData::create() --> class std::unique_ptr<class genesis::tree::LayoutEdgeData, struct std::default_delete<class genesis::tree::LayoutEdgeData> >");
	}
	// genesis::tree::LayoutShape file:genesis/tree/drawing/layout_base.hpp line:50
	pybind11::enum_<genesis::tree::LayoutShape>(M("genesis::tree"), "LayoutShape", "Shape of the tree for drawing, either circular or rectangular.")
		.value("kCircular", genesis::tree::LayoutShape::kCircular)
		.value("kRectangular", genesis::tree::LayoutShape::kRectangular);

;

	// genesis::tree::LayoutType file:genesis/tree/drawing/layout_base.hpp line:62
	pybind11::enum_<genesis::tree::LayoutType>(M("genesis::tree"), "LayoutType", "Type of tree for drawing, either phylogram or cladogram.\n\n A phylogram uses and shows branch lengths, while a cladegram aligns all leaf nodes to each\n other, and adjusts inner nodes accordingly.")
		.value("kPhylogram", genesis::tree::LayoutType::kPhylogram)
		.value("kCladogram", genesis::tree::LayoutType::kCladogram);

;

	// genesis::tree::LayoutSpreading file:genesis/tree/drawing/layout_base.hpp line:80
	pybind11::enum_<genesis::tree::LayoutSpreading>(M("genesis::tree"), "LayoutSpreading", "Spreading of the nodes of a tree for drawing.\n\n In tree drawing, one axis is usually used for the branch lengths (or at least, for distancing\n nodes from each other in a cladegram), while the other axis does not have a biological meaning.\n It is instead used to spread out the nodes so that the tree is actually drawn in a plane instead\n of just a line.\n\n Using this setting, the spreading can be controlled: Default is to spread out the leaves evenly,\n giving the typical tree layout. Sometimes however it is necessary to also make space for inner\n nodes. This is what the other options are for (with or without the root as a special case).")
		.value("kLeafNodesOnly", genesis::tree::LayoutSpreading::kLeafNodesOnly)
		.value("kAllNodesButRoot", genesis::tree::LayoutSpreading::kAllNodesButRoot)
		.value("kAllNodes", genesis::tree::LayoutSpreading::kAllNodes);

;

	{ // genesis::tree::LayoutBase file:genesis/tree/drawing/layout_base.hpp line:91
		pybind11::class_<genesis::tree::LayoutBase, std::shared_ptr<genesis::tree::LayoutBase>, PyCallBack_genesis_tree_LayoutBase> cl(M("genesis::tree"), "LayoutBase", "");
		cl.def( pybind11::init( [](){ return new PyCallBack_genesis_tree_LayoutBase(); } ) );
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0){ return new PyCallBack_genesis_tree_LayoutBase(a0); } ), "doc");
		cl.def( pybind11::init( [](const class genesis::tree::Tree & a0, const enum genesis::tree::LayoutType & a1){ return new PyCallBack_genesis_tree_LayoutBase(a0, a1); } ), "doc");
		cl.def( pybind11::init<const class genesis::tree::Tree &, const enum genesis::tree::LayoutType, bool>(), pybind11::arg("orig_tree"), pybind11::arg("drawing_type"), pybind11::arg("ladderize") );

		cl.def(pybind11::init<PyCallBack_genesis_tree_LayoutBase const &>());
		cl.def("assign", (class genesis::tree::LayoutBase & (genesis::tree::LayoutBase::*)(const class genesis::tree::LayoutBase &)) &genesis::tree::LayoutBase::operator=, "C++: genesis::tree::LayoutBase::operator=(const class genesis::tree::LayoutBase &) --> class genesis::tree::LayoutBase &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("tree", [](genesis::tree::LayoutBase &o, const class genesis::tree::Tree & a0) -> void { return o.tree(a0); }, "", pybind11::arg("orig_tree"));
		cl.def("tree", (void (genesis::tree::LayoutBase::*)(const class genesis::tree::Tree &, bool)) &genesis::tree::LayoutBase::tree, "C++: genesis::tree::LayoutBase::tree(const class genesis::tree::Tree &, bool) --> void", pybind11::arg("orig_tree"), pybind11::arg("ladderize"));
		cl.def("tree", (const class genesis::tree::Tree & (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::tree, "C++: genesis::tree::LayoutBase::tree() const --> const class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal);
		cl.def("set_edge_strokes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgStroke &)) &genesis::tree::LayoutBase::set_edge_strokes, "C++: genesis::tree::LayoutBase::set_edge_strokes(const struct genesis::utils::SvgStroke &) --> void", pybind11::arg("stroke"));
		cl.def("set_edge_strokes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &)) &genesis::tree::LayoutBase::set_edge_strokes, "C++: genesis::tree::LayoutBase::set_edge_strokes(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &) --> void", pybind11::arg("strokes"));
		cl.def("set_edge_spreading_strokes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgStroke &)) &genesis::tree::LayoutBase::set_edge_spreading_strokes, "C++: genesis::tree::LayoutBase::set_edge_spreading_strokes(const struct genesis::utils::SvgStroke &) --> void", pybind11::arg("stroke"));
		cl.def("set_edge_spreading_strokes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &)) &genesis::tree::LayoutBase::set_edge_spreading_strokes, "C++: genesis::tree::LayoutBase::set_edge_spreading_strokes(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &) --> void", pybind11::arg("strokes"));
		cl.def("set_edge_distance_strokes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgStroke &)) &genesis::tree::LayoutBase::set_edge_distance_strokes, "C++: genesis::tree::LayoutBase::set_edge_distance_strokes(const struct genesis::utils::SvgStroke &) --> void", pybind11::arg("stroke"));
		cl.def("set_edge_distance_strokes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &)) &genesis::tree::LayoutBase::set_edge_distance_strokes, "C++: genesis::tree::LayoutBase::set_edge_distance_strokes(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &) --> void", pybind11::arg("strokes"));
		cl.def("set_label_spacer_strokes", [](genesis::tree::LayoutBase &o, const struct genesis::utils::SvgStroke & a0) -> void { return o.set_label_spacer_strokes(a0); }, "", pybind11::arg("stroke"));
		cl.def("set_label_spacer_strokes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgStroke &, enum genesis::tree::LayoutSpreading)) &genesis::tree::LayoutBase::set_label_spacer_strokes, "C++: genesis::tree::LayoutBase::set_label_spacer_strokes(const struct genesis::utils::SvgStroke &, enum genesis::tree::LayoutSpreading) --> void", pybind11::arg("stroke"), pybind11::arg("spreading"));
		cl.def("set_label_spacer_strokes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &)) &genesis::tree::LayoutBase::set_label_spacer_strokes, "C++: genesis::tree::LayoutBase::set_label_spacer_strokes(const class std::vector<struct genesis::utils::SvgStroke, class std::allocator<struct genesis::utils::SvgStroke> > &) --> void", pybind11::arg("strokes"));
		cl.def("set_edge_shapes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgGroup &)) &genesis::tree::LayoutBase::set_edge_shapes, "C++: genesis::tree::LayoutBase::set_edge_shapes(const struct genesis::utils::SvgGroup &) --> void", pybind11::arg("shape"));
		cl.def("set_edge_shapes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgGroup, class std::allocator<struct genesis::utils::SvgGroup> > &)) &genesis::tree::LayoutBase::set_edge_shapes, "C++: genesis::tree::LayoutBase::set_edge_shapes(const class std::vector<struct genesis::utils::SvgGroup, class std::allocator<struct genesis::utils::SvgGroup> > &) --> void", pybind11::arg("shapes"));
		cl.def("set_node_shapes", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgGroup &)) &genesis::tree::LayoutBase::set_node_shapes, "C++: genesis::tree::LayoutBase::set_node_shapes(const struct genesis::utils::SvgGroup &) --> void", pybind11::arg("shape"));
		cl.def("set_node_shapes", (void (genesis::tree::LayoutBase::*)(const class std::vector<struct genesis::utils::SvgGroup, class std::allocator<struct genesis::utils::SvgGroup> > &)) &genesis::tree::LayoutBase::set_node_shapes, "C++: genesis::tree::LayoutBase::set_node_shapes(const class std::vector<struct genesis::utils::SvgGroup, class std::allocator<struct genesis::utils::SvgGroup> > &) --> void", pybind11::arg("shapes"));
		cl.def("to_svg_document", (class genesis::utils::SvgDocument (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::to_svg_document, "C++: genesis::tree::LayoutBase::to_svg_document() const --> class genesis::utils::SvgDocument");
		cl.def("type", (void (genesis::tree::LayoutBase::*)(const enum genesis::tree::LayoutType)) &genesis::tree::LayoutBase::type, "C++: genesis::tree::LayoutBase::type(const enum genesis::tree::LayoutType) --> void", pybind11::arg("drawing_type"));
		cl.def("type", (enum genesis::tree::LayoutType (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::type, "C++: genesis::tree::LayoutBase::type() const --> enum genesis::tree::LayoutType");
		cl.def("inner_node_spreading", (void (genesis::tree::LayoutBase::*)(enum genesis::tree::LayoutSpreading)) &genesis::tree::LayoutBase::inner_node_spreading, "C++: genesis::tree::LayoutBase::inner_node_spreading(enum genesis::tree::LayoutSpreading) --> void", pybind11::arg("value"));
		cl.def("inner_node_spreading", (enum genesis::tree::LayoutSpreading (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::inner_node_spreading, "C++: genesis::tree::LayoutBase::inner_node_spreading() const --> enum genesis::tree::LayoutSpreading");
		cl.def("align_labels", (void (genesis::tree::LayoutBase::*)(bool)) &genesis::tree::LayoutBase::align_labels, "Align the taxon labels/node names/texts to each other.\n\n If set to `true`, the taxon names are aligned with each other. This is particularly interesting\n in phylograms, where they otherwise are placed close to their nodes. In cladograms, the tip\n nodes are aligned to each other anyway, so there is not much of a difference there.\n\n In order to still be able to see which label belongs to which node, a label spacer line is\n drawn for each node, using LayoutNodeData::spacer_stroke. These strokes can be set with\n set_label_spacer_strokes().\n\n Note also the extra_spacer() setting, which modifies how much longer the spacer line is\n than the exact alignment. This setting is useful to create some extra space between the\n tree and the aligned labels.\n\nC++: genesis::tree::LayoutBase::align_labels(bool) --> void", pybind11::arg("value"));
		cl.def("align_labels", (bool (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::align_labels, "C++: genesis::tree::LayoutBase::align_labels() const --> bool");
		cl.def("extra_spacer", (void (genesis::tree::LayoutBase::*)(double)) &genesis::tree::LayoutBase::extra_spacer, "Set some extra space betwen the tree nodes and their labels when align_labels()\n is set to `true`.\n\nC++: genesis::tree::LayoutBase::extra_spacer(double) --> void", pybind11::arg("value"));
		cl.def("extra_spacer", (double (genesis::tree::LayoutBase::*)() const) &genesis::tree::LayoutBase::extra_spacer, "C++: genesis::tree::LayoutBase::extra_spacer() const --> double");
		cl.def("text_template", (void (genesis::tree::LayoutBase::*)(const struct genesis::utils::SvgText &)) &genesis::tree::LayoutBase::text_template, "C++: genesis::tree::LayoutBase::text_template(const struct genesis::utils::SvgText &) --> void", pybind11::arg("tt"));
		cl.def("text_template", (struct genesis::utils::SvgText & (genesis::tree::LayoutBase::*)()) &genesis::tree::LayoutBase::text_template, "C++: genesis::tree::LayoutBase::text_template() --> struct genesis::utils::SvgText &", pybind11::return_value_policy::reference_internal);
	}
	{ // genesis::tree::HeatTreeParameters file:genesis/tree/drawing/heat_tree.hpp line:52
		pybind11::class_<genesis::tree::HeatTreeParameters, std::shared_ptr<genesis::tree::HeatTreeParameters>> cl(M("genesis::tree"), "HeatTreeParameters", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::HeatTreeParameters(); } ) );
		cl.def( pybind11::init( [](genesis::tree::HeatTreeParameters const &o){ return new genesis::tree::HeatTreeParameters(o); } ) );
		cl.def_readwrite("tree", &genesis::tree::HeatTreeParameters::tree);
		cl.def_readwrite("type", &genesis::tree::HeatTreeParameters::type);
		cl.def_readwrite("ladderize", &genesis::tree::HeatTreeParameters::ladderize);
		cl.def_readwrite("color_per_branch", &genesis::tree::HeatTreeParameters::color_per_branch);
		cl.def_readwrite("stroke", &genesis::tree::HeatTreeParameters::stroke);
		cl.def_readwrite("matrix", &genesis::tree::HeatTreeParameters::matrix);
		cl.def_readwrite("column_labels", &genesis::tree::HeatTreeParameters::column_labels);
		cl.def_readwrite("matrix_as_bmp", &genesis::tree::HeatTreeParameters::matrix_as_bmp);
		cl.def("assign", (struct genesis::tree::HeatTreeParameters & (genesis::tree::HeatTreeParameters::*)(const struct genesis::tree::HeatTreeParameters &)) &genesis::tree::HeatTreeParameters::operator=, "C++: genesis::tree::HeatTreeParameters::operator=(const struct genesis::tree::HeatTreeParameters &) --> struct genesis::tree::HeatTreeParameters &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &) file:genesis/tree/drawing/heat_tree.hpp line:67
	M("genesis::tree").def("heat_tree", (class genesis::utils::SvgDocument (*)(const struct genesis::tree::HeatTreeParameters &)) &genesis::tree::heat_tree, "C++: genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &) --> class genesis::utils::SvgDocument", pybind11::arg("params"));

	// genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &) file:genesis/tree/drawing/heat_tree.hpp line:71
	M("genesis::tree").def("heat_tree", (class genesis::utils::SvgDocument (*)(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &)) &genesis::tree::heat_tree, "C++: genesis::tree::heat_tree(const struct genesis::tree::HeatTreeParameters &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &) --> class genesis::utils::SvgDocument", pybind11::arg("params"), pybind11::arg("matrix_color_map"), pybind11::arg("matrix_color_norm"));

}
