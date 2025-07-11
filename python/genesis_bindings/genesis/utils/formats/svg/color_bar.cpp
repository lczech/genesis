#include <functional>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/map.hpp>
#include <genesis/utils/color/normalization.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/color_bar.hpp>
#include <genesis/utils/formats/svg/document.hpp>
#include <genesis/utils/formats/svg/gradient.hpp>
#include <genesis/utils/formats/svg/group.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/image.hpp>
#include <genesis/utils/formats/svg/matrix.hpp>
#include <genesis/utils/formats/svg/object.hpp>
#include <genesis/utils/formats/svg/shapes.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <ios>
#include <iterator>
#include <map>
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

void bind_genesis_utils_formats_svg_color_bar(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgColorBarSettings file:genesis/utils/formats/svg/color_bar.hpp line:59
		pybind11::class_<genesis::utils::SvgColorBarSettings, std::shared_ptr<genesis::utils::SvgColorBarSettings>> cl(M("genesis::utils"), "SvgColorBarSettings", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgColorBarSettings(); } ) );

		pybind11::enum_<genesis::utils::SvgColorBarSettings::Direction>(cl, "Direction", "")
			.value("kTopToBottom", genesis::utils::SvgColorBarSettings::Direction::kTopToBottom)
			.value("kBottomToTop", genesis::utils::SvgColorBarSettings::Direction::kBottomToTop)
			.value("kLeftToRight", genesis::utils::SvgColorBarSettings::Direction::kLeftToRight)
			.value("kRightToLeft", genesis::utils::SvgColorBarSettings::Direction::kRightToLeft);

		cl.def_readwrite("direction", &genesis::utils::SvgColorBarSettings::direction);
		cl.def_readwrite("width", &genesis::utils::SvgColorBarSettings::width);
		cl.def_readwrite("height", &genesis::utils::SvgColorBarSettings::height);
		cl.def_readwrite("with_tickmarks", &genesis::utils::SvgColorBarSettings::with_tickmarks);
		cl.def_readwrite("with_labels", &genesis::utils::SvgColorBarSettings::with_labels);
		cl.def_readwrite("num_ticks", &genesis::utils::SvgColorBarSettings::num_ticks);
		cl.def_readwrite("text_size", &genesis::utils::SvgColorBarSettings::text_size);
	}
	// genesis::utils::make_svg_color_bar(const struct genesis::utils::SvgColorBarSettings &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const std::string &) file:genesis/utils/formats/svg/color_bar.hpp line:92
	M("genesis::utils").def("make_svg_color_bar", [](const struct genesis::utils::SvgColorBarSettings & a0, const class genesis::utils::ColorMap & a1, const class genesis::utils::ColorNormalization & a2) -> std::pair<struct genesis::utils::SvgGradientLinear, struct genesis::utils::SvgGroup> { return genesis::utils::make_svg_color_bar(a0, a1, a2); }, "", pybind11::arg("settings"), pybind11::arg("map"), pybind11::arg("norm"));
	M("genesis::utils").def("make_svg_color_bar", (struct std::pair<struct genesis::utils::SvgGradientLinear, struct genesis::utils::SvgGroup> (*)(const struct genesis::utils::SvgColorBarSettings &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const std::string &)) &genesis::utils::make_svg_color_bar, "C++: genesis::utils::make_svg_color_bar(const struct genesis::utils::SvgColorBarSettings &, const class genesis::utils::ColorMap &, const class genesis::utils::ColorNormalization &, const std::string &) --> struct std::pair<struct genesis::utils::SvgGradientLinear, struct genesis::utils::SvgGroup>", pybind11::arg("settings"), pybind11::arg("map"), pybind11::arg("norm"), pybind11::arg("id"));

	// genesis::utils::make_svg_color_list(const class genesis::utils::ColorMap &, const class std::vector<std::string, class std::allocator<std::string > > &) file:genesis/utils/formats/svg/color_bar.hpp line:99
	M("genesis::utils").def("make_svg_color_list", (struct genesis::utils::SvgGroup (*)(const class genesis::utils::ColorMap &, const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::make_svg_color_list, "C++: genesis::utils::make_svg_color_list(const class genesis::utils::ColorMap &, const class std::vector<std::string, class std::allocator<std::string > > &) --> struct genesis::utils::SvgGroup", pybind11::arg("map"), pybind11::arg("labels"));

	// genesis::utils::make_svg_color_list(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, const class std::vector<std::string, class std::allocator<std::string > > &) file:genesis/utils/formats/svg/color_bar.hpp line:104
	M("genesis::utils").def("make_svg_color_list", (struct genesis::utils::SvgGroup (*)(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::make_svg_color_list, "C++: genesis::utils::make_svg_color_list(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &, const class std::vector<std::string, class std::allocator<std::string > > &) --> struct genesis::utils::SvgGroup", pybind11::arg("colors"), pybind11::arg("labels"));

	{ // genesis::utils::SvgDocument file:genesis/utils/formats/svg/document.hpp line:50
		pybind11::class_<genesis::utils::SvgDocument, std::shared_ptr<genesis::utils::SvgDocument>> cl(M("genesis::utils"), "SvgDocument", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgDocument(); } ) );
		cl.def( pybind11::init( [](genesis::utils::SvgDocument const &o){ return new genesis::utils::SvgDocument(o); } ) );

		pybind11::enum_<genesis::utils::SvgDocument::Overflow>(cl, "Overflow", "")
			.value("kNone", genesis::utils::SvgDocument::Overflow::kNone)
			.value("kVisible", genesis::utils::SvgDocument::Overflow::kVisible)
			.value("kHidden", genesis::utils::SvgDocument::Overflow::kHidden)
			.value("kScroll", genesis::utils::SvgDocument::Overflow::kScroll)
			.value("kAuto", genesis::utils::SvgDocument::Overflow::kAuto)
			.value("kInherit", genesis::utils::SvgDocument::Overflow::kInherit);

		cl.def_readwrite("margin", &genesis::utils::SvgDocument::margin);
		cl.def_readwrite("overflow", &genesis::utils::SvgDocument::overflow);
		cl.def_readwrite("defs", &genesis::utils::SvgDocument::defs);
		cl.def_readwrite("background_color", &genesis::utils::SvgDocument::background_color);
		cl.def("assign", (class genesis::utils::SvgDocument & (genesis::utils::SvgDocument::*)(const class genesis::utils::SvgDocument &)) &genesis::utils::SvgDocument::operator=, "C++: genesis::utils::SvgDocument::operator=(const class genesis::utils::SvgDocument &) --> class genesis::utils::SvgDocument &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgDocument::*)() const) &genesis::utils::SvgDocument::bounding_box, "C++: genesis::utils::SvgDocument::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", (void (genesis::utils::SvgDocument::*)(std::ostream &) const) &genesis::utils::SvgDocument::write, "C++: genesis::utils::SvgDocument::write(std::ostream &) const --> void", pybind11::arg("out"));
		cl.def("write", (void (genesis::utils::SvgDocument::*)(class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::utils::SvgDocument::write, "C++: genesis::utils::SvgDocument::write(class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("target"));
		cl.def("add", (class genesis::utils::SvgDocument & (genesis::utils::SvgDocument::*)(const class genesis::utils::SvgObject &)) &genesis::utils::SvgDocument::add, "Add an SvgObject to the document.\n\n Returns the SvgDocument in order to allow for a fluent interface.\n\nC++: genesis::utils::SvgDocument::add(const class genesis::utils::SvgObject &) --> class genesis::utils::SvgDocument &", pybind11::return_value_policy::reference_internal, pybind11::arg("object"));
		cl.def("__lshift__", (class genesis::utils::SvgDocument & (genesis::utils::SvgDocument::*)(const class genesis::utils::SvgObject &)) &genesis::utils::SvgDocument::operator<<, "Shortcut operator for add(), which allows an even more fluent interface.\n\nC++: genesis::utils::SvgDocument::operator<<(const class genesis::utils::SvgObject &) --> class genesis::utils::SvgDocument &", pybind11::return_value_policy::reference_internal, pybind11::arg("object"));
	}
	{ // genesis::utils::SvgImage file:genesis/utils/formats/svg/image.hpp line:47
		pybind11::class_<genesis::utils::SvgImage, std::shared_ptr<genesis::utils::SvgImage>> cl(M("genesis::utils"), "SvgImage", "");
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("href") );

		cl.def( pybind11::init<const std::string &, const struct genesis::utils::SvgPoint &>(), pybind11::arg("href"), pybind11::arg("position") );

		cl.def( pybind11::init<const std::string &, const struct genesis::utils::SvgPoint &, const struct genesis::utils::SvgSize &>(), pybind11::arg("href"), pybind11::arg("position"), pybind11::arg("size") );

		cl.def( pybind11::init<const std::string &, double, double, double, double>(), pybind11::arg("href"), pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("w"), pybind11::arg("h") );

		cl.def( pybind11::init( [](genesis::utils::SvgImage const &o){ return new genesis::utils::SvgImage(o); } ) );

		pybind11::enum_<genesis::utils::SvgImage::ImageRendering>(cl, "ImageRendering", "Rendering modes for SVG embedded images.\n\n Note that not all are supported by all viewers. Experimentation for the desired output\n format/medium/viewer/editor is needed here, unfortunately.")
			.value("kNone", genesis::utils::SvgImage::ImageRendering::kNone)
			.value("kAuto", genesis::utils::SvgImage::ImageRendering::kAuto)
			.value("kOptimizeSpeed", genesis::utils::SvgImage::ImageRendering::kOptimizeSpeed)
			.value("kOptimizeQuality", genesis::utils::SvgImage::ImageRendering::kOptimizeQuality)
			.value("kSmooth", genesis::utils::SvgImage::ImageRendering::kSmooth)
			.value("kCrispEdges", genesis::utils::SvgImage::ImageRendering::kCrispEdges)
			.value("kPixelated", genesis::utils::SvgImage::ImageRendering::kPixelated)
			.value("kInherit", genesis::utils::SvgImage::ImageRendering::kInherit)
			.value("kInitial", genesis::utils::SvgImage::ImageRendering::kInitial)
			.value("kUnset", genesis::utils::SvgImage::ImageRendering::kUnset);

		cl.def_readwrite("id", &genesis::utils::SvgImage::id);
		cl.def_readwrite("href", &genesis::utils::SvgImage::href);
		cl.def_readwrite("position", &genesis::utils::SvgImage::position);
		cl.def_readwrite("size", &genesis::utils::SvgImage::size);
		cl.def_readwrite("transform", &genesis::utils::SvgImage::transform);
		cl.def_readwrite("rendering", &genesis::utils::SvgImage::rendering);
		cl.def("assign", (struct genesis::utils::SvgImage & (genesis::utils::SvgImage::*)(const struct genesis::utils::SvgImage &)) &genesis::utils::SvgImage::operator=, "C++: genesis::utils::SvgImage::operator=(const struct genesis::utils::SvgImage &) --> struct genesis::utils::SvgImage &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgImage::*)() const) &genesis::utils::SvgImage::bounding_box, "C++: genesis::utils::SvgImage::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgImage const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgImage const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgImage::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgImage::write, "C++: genesis::utils::SvgImage::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgMatrixSettings file:genesis/utils/formats/svg/matrix.hpp line:56
		pybind11::class_<genesis::utils::SvgMatrixSettings, std::shared_ptr<genesis::utils::SvgMatrixSettings>> cl(M("genesis::utils"), "SvgMatrixSettings", "");
		cl.def( pybind11::init( [](genesis::utils::SvgMatrixSettings const &o){ return new genesis::utils::SvgMatrixSettings(o); } ) );
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgMatrixSettings(); } ) );
		cl.def_readwrite("pixel_width", &genesis::utils::SvgMatrixSettings::pixel_width);
		cl.def_readwrite("pixel_height", &genesis::utils::SvgMatrixSettings::pixel_height);
		cl.def_readwrite("width_overlap", &genesis::utils::SvgMatrixSettings::width_overlap);
		cl.def_readwrite("height_overlap", &genesis::utils::SvgMatrixSettings::height_overlap);
		cl.def_readwrite("label_template", &genesis::utils::SvgMatrixSettings::label_template);
		cl.def_readwrite("column_label_rotation", &genesis::utils::SvgMatrixSettings::column_label_rotation);
		cl.def("assign", (struct genesis::utils::SvgMatrixSettings & (genesis::utils::SvgMatrixSettings::*)(const struct genesis::utils::SvgMatrixSettings &)) &genesis::utils::SvgMatrixSettings::operator=, "C++: genesis::utils::SvgMatrixSettings::operator=(const struct genesis::utils::SvgMatrixSettings &) --> struct genesis::utils::SvgMatrixSettings &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &, const class std::vector<std::string, class std::allocator<std::string > > &) file:genesis/utils/formats/svg/matrix.hpp line:82
	M("genesis::utils").def("make_svg_matrix", (struct genesis::utils::SvgGroup (*)(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &, const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::make_svg_matrix, "C++: genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &, const class std::vector<std::string, class std::allocator<std::string > > &) --> struct genesis::utils::SvgGroup", pybind11::arg("mat"), pybind11::arg("settings"), pybind11::arg("row_labels"), pybind11::arg("col_labels"));

	// genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &) file:genesis/utils/formats/svg/matrix.hpp line:89
	M("genesis::utils").def("make_svg_matrix", (struct genesis::utils::SvgGroup (*)(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::utils::make_svg_matrix, "C++: genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings, const class std::vector<std::string, class std::allocator<std::string > > &) --> struct genesis::utils::SvgGroup", pybind11::arg("mat"), pybind11::arg("settings"), pybind11::arg("row_labels"));

	// genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings) file:genesis/utils/formats/svg/matrix.hpp line:95
	M("genesis::utils").def("make_svg_matrix", [](const class genesis::utils::Matrix<class genesis::utils::Color> & a0) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_matrix(a0); }, "", pybind11::arg("mat"));
	M("genesis::utils").def("make_svg_matrix", (struct genesis::utils::SvgGroup (*)(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings)) &genesis::utils::make_svg_matrix, "C++: genesis::utils::make_svg_matrix(const class genesis::utils::Matrix<class genesis::utils::Color> &, struct genesis::utils::SvgMatrixSettings) --> struct genesis::utils::SvgGroup", pybind11::arg("mat"), pybind11::arg("settings"));

	{ // genesis::utils::SvgLine file:genesis/utils/formats/svg/shapes.hpp line:50
		pybind11::class_<genesis::utils::SvgLine, std::shared_ptr<genesis::utils::SvgLine>> cl(M("genesis::utils"), "SvgLine", "");
		cl.def( pybind11::init( [](const struct genesis::utils::SvgPoint & a0, const struct genesis::utils::SvgPoint & a1){ return new genesis::utils::SvgLine(a0, a1); } ), "doc" , pybind11::arg("point_1"), pybind11::arg("point_2"));
		cl.def( pybind11::init<const struct genesis::utils::SvgPoint &, const struct genesis::utils::SvgPoint &, const struct genesis::utils::SvgStroke &>(), pybind11::arg("point_1"), pybind11::arg("point_2"), pybind11::arg("stroke") );

		cl.def( pybind11::init( [](double const & a0, double const & a1, double const & a2, double const & a3){ return new genesis::utils::SvgLine(a0, a1, a2, a3); } ), "doc" , pybind11::arg("x1"), pybind11::arg("y1"), pybind11::arg("x2"), pybind11::arg("y2"));
		cl.def( pybind11::init<double, double, double, double, const struct genesis::utils::SvgStroke &>(), pybind11::arg("x1"), pybind11::arg("y1"), pybind11::arg("x2"), pybind11::arg("y2"), pybind11::arg("stroke") );

		cl.def( pybind11::init( [](genesis::utils::SvgLine const &o){ return new genesis::utils::SvgLine(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgLine::id);
		cl.def_readwrite("point_1", &genesis::utils::SvgLine::point_1);
		cl.def_readwrite("point_2", &genesis::utils::SvgLine::point_2);
		cl.def_readwrite("stroke", &genesis::utils::SvgLine::stroke);
		cl.def_readwrite("transform", &genesis::utils::SvgLine::transform);
		cl.def("assign", (struct genesis::utils::SvgLine & (genesis::utils::SvgLine::*)(const struct genesis::utils::SvgLine &)) &genesis::utils::SvgLine::operator=, "C++: genesis::utils::SvgLine::operator=(const struct genesis::utils::SvgLine &) --> struct genesis::utils::SvgLine &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgLine::*)() const) &genesis::utils::SvgLine::bounding_box, "C++: genesis::utils::SvgLine::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgLine const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgLine const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgLine::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgLine::write, "C++: genesis::utils::SvgLine::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
}
