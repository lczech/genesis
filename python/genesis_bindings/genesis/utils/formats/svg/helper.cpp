#include <genesis/utils/color/color.hpp>
#include <genesis/utils/formats/svg/attributes.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/object.hpp>
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

void bind_genesis_utils_formats_svg_helper(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgSize file:genesis/utils/formats/svg/helper.hpp line:77
		pybind11::class_<genesis::utils::SvgSize, std::shared_ptr<genesis::utils::SvgSize>> cl(M("genesis::utils"), "SvgSize", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgSize(); } ), "doc" );
		cl.def( pybind11::init( [](double const & a0){ return new genesis::utils::SvgSize(a0); } ), "doc" , pybind11::arg("width"));
		cl.def( pybind11::init<double, double>(), pybind11::arg("width"), pybind11::arg("height") );

		cl.def( pybind11::init( [](genesis::utils::SvgSize const &o){ return new genesis::utils::SvgSize(o); } ) );
		cl.def_readwrite("width", &genesis::utils::SvgSize::width);
		cl.def_readwrite("height", &genesis::utils::SvgSize::height);
		cl.def("assign", (struct genesis::utils::SvgSize & (genesis::utils::SvgSize::*)(const struct genesis::utils::SvgSize &)) &genesis::utils::SvgSize::operator=, "C++: genesis::utils::SvgSize::operator=(const struct genesis::utils::SvgSize &) --> struct genesis::utils::SvgSize &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::SvgMargin file:genesis/utils/formats/svg/helper.hpp line:92
		pybind11::class_<genesis::utils::SvgMargin, std::shared_ptr<genesis::utils::SvgMargin>> cl(M("genesis::utils"), "SvgMargin", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgMargin(); } ) );
		cl.def( pybind11::init<double>(), pybind11::arg("all") );

		cl.def( pybind11::init<double, double>(), pybind11::arg("top_bottom"), pybind11::arg("right_left") );

		cl.def( pybind11::init<double, double, double>(), pybind11::arg("top"), pybind11::arg("right_left"), pybind11::arg("bottom") );

		cl.def( pybind11::init<double, double, double, double>(), pybind11::arg("top"), pybind11::arg("right"), pybind11::arg("bottom"), pybind11::arg("left") );

		cl.def( pybind11::init( [](genesis::utils::SvgMargin const &o){ return new genesis::utils::SvgMargin(o); } ) );
		cl.def_readwrite("top", &genesis::utils::SvgMargin::top);
		cl.def_readwrite("right", &genesis::utils::SvgMargin::right);
		cl.def_readwrite("bottom", &genesis::utils::SvgMargin::bottom);
		cl.def_readwrite("left", &genesis::utils::SvgMargin::left);
		cl.def("assign", (struct genesis::utils::SvgMargin & (genesis::utils::SvgMargin::*)(const struct genesis::utils::SvgMargin &)) &genesis::utils::SvgMargin::operator=, "C++: genesis::utils::SvgMargin::operator=(const struct genesis::utils::SvgMargin &) --> struct genesis::utils::SvgMargin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::utils::SvgBox file:genesis/utils/formats/svg/helper.hpp line:127
		pybind11::class_<genesis::utils::SvgBox, std::shared_ptr<genesis::utils::SvgBox>> cl(M("genesis::utils"), "SvgBox", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgBox(); } ) );
		cl.def( pybind11::init<struct genesis::utils::SvgPoint, struct genesis::utils::SvgPoint>(), pybind11::arg("top_left"), pybind11::arg("bottom_right") );

		cl.def( pybind11::init<struct genesis::utils::SvgPoint, double, double>(), pybind11::arg("top_left_v"), pybind11::arg("width"), pybind11::arg("height") );

		cl.def( pybind11::init( [](genesis::utils::SvgBox const &o){ return new genesis::utils::SvgBox(o); } ) );
		cl.def_readwrite("top_left", &genesis::utils::SvgBox::top_left);
		cl.def_readwrite("bottom_right", &genesis::utils::SvgBox::bottom_right);
		cl.def("width", (double (genesis::utils::SvgBox::*)() const) &genesis::utils::SvgBox::width, "C++: genesis::utils::SvgBox::width() const --> double");
		cl.def("height", (double (genesis::utils::SvgBox::*)() const) &genesis::utils::SvgBox::height, "C++: genesis::utils::SvgBox::height() const --> double");
		cl.def("size", (struct genesis::utils::SvgSize (genesis::utils::SvgBox::*)() const) &genesis::utils::SvgBox::size, "C++: genesis::utils::SvgBox::size() const --> struct genesis::utils::SvgSize");
		cl.def("empty", (bool (genesis::utils::SvgBox::*)() const) &genesis::utils::SvgBox::empty, "C++: genesis::utils::SvgBox::empty() const --> bool");
		cl.def_static("combine", (struct genesis::utils::SvgBox (*)(const struct genesis::utils::SvgBox &, const struct genesis::utils::SvgBox &)) &genesis::utils::SvgBox::combine, "C++: genesis::utils::SvgBox::combine(const struct genesis::utils::SvgBox &, const struct genesis::utils::SvgBox &) --> struct genesis::utils::SvgBox", pybind11::arg("lhs"), pybind11::arg("rhs"));
	}
	{ // genesis::utils::SvgDrawingOptions file:genesis/utils/formats/svg/helper.hpp line:213
		pybind11::class_<genesis::utils::SvgDrawingOptions, std::shared_ptr<genesis::utils::SvgDrawingOptions>> cl(M("genesis::utils"), "SvgDrawingOptions", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgDrawingOptions(); } ) );
		cl.def( pybind11::init( [](genesis::utils::SvgDrawingOptions const &o){ return new genesis::utils::SvgDrawingOptions(o); } ) );
		cl.def_readwrite("offset_x", &genesis::utils::SvgDrawingOptions::offset_x);
		cl.def_readwrite("offset_y", &genesis::utils::SvgDrawingOptions::offset_y);
	}
	// genesis::utils::svg_comment(const std::string &) file:genesis/utils/formats/svg/helper.hpp line:223
	M("genesis::utils").def("svg_comment", (std::string (*)(const std::string &)) &genesis::utils::svg_comment, "C++: genesis::utils::svg_comment(const std::string &) --> std::string", pybind11::arg("content"));

	// genesis::utils::svg_data_uri(const std::string &, const std::string &, bool) file:genesis/utils/formats/svg/helper.hpp line:239
	M("genesis::utils").def("svg_data_uri", [](const std::string & a0, const std::string & a1) -> std::string { return genesis::utils::svg_data_uri(a0, a1); }, "", pybind11::arg("media_type"), pybind11::arg("content"));
	M("genesis::utils").def("svg_data_uri", (std::string (*)(const std::string &, const std::string &, bool)) &genesis::utils::svg_data_uri, "C++: genesis::utils::svg_data_uri(const std::string &, const std::string &, bool) --> std::string", pybind11::arg("media_type"), pybind11::arg("content"), pybind11::arg("encode_base64"));

	// genesis::utils::svg_arc(double, double, double, double, double, bool) file:genesis/utils/formats/svg/helper.hpp line:265
	M("genesis::utils").def("svg_arc", [](double const & a0, double const & a1, double const & a2, double const & a3, double const & a4) -> std::string { return genesis::utils::svg_arc(a0, a1, a2, a3, a4); }, "", pybind11::arg("center_x"), pybind11::arg("center_y"), pybind11::arg("radius"), pybind11::arg("start_angle"), pybind11::arg("end_angle"));
	M("genesis::utils").def("svg_arc", (std::string (*)(double, double, double, double, double, bool)) &genesis::utils::svg_arc, "Create an arc to use in an SvgPath.\n\n Example:\n\n     SvgPath(\n         { svg_arc( 0, 0, 10, start_a, end_a ) },\n         stroke\n     );\n\n The angles are measured in radians, and always drawn clockwise from start to end.\n If  is given, the path is drawn including segments to the circle center;\n otherwise, the path position is moved to the beginning of the arc first, and left at its end.\n\nC++: genesis::utils::svg_arc(double, double, double, double, double, bool) --> std::string", pybind11::arg("center_x"), pybind11::arg("center_y"), pybind11::arg("radius"), pybind11::arg("start_angle"), pybind11::arg("end_angle"), pybind11::arg("wedge"));

	// genesis::utils::svg_bounding_box(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &) file:genesis/utils/formats/svg/helper.hpp line:274
	M("genesis::utils").def("svg_bounding_box", (struct genesis::utils::SvgBox (*)(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &)) &genesis::utils::svg_bounding_box, "Compute the bounding box of a set of points.\n\nC++: genesis::utils::svg_bounding_box(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &) --> struct genesis::utils::SvgBox", pybind11::arg("points"));

	// genesis::utils::svg_bounding_box(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &, const struct genesis::utils::SvgTransform &) file:genesis/utils/formats/svg/helper.hpp line:281
	M("genesis::utils").def("svg_bounding_box", (struct genesis::utils::SvgBox (*)(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &, const struct genesis::utils::SvgTransform &)) &genesis::utils::svg_bounding_box, "Compute the bounding box of a set of points, including their transformation.\n\nC++: genesis::utils::svg_bounding_box(const class std::vector<struct genesis::utils::SvgPoint, class std::allocator<struct genesis::utils::SvgPoint> > &, const struct genesis::utils::SvgTransform &) --> struct genesis::utils::SvgBox", pybind11::arg("points"), pybind11::arg("transform"));

	{ // genesis::utils::SvgObject file:genesis/utils/formats/svg/object.hpp line:46
		pybind11::class_<genesis::utils::SvgObject, std::shared_ptr<genesis::utils::SvgObject>> cl(M("genesis::utils"), "SvgObject", "");
		cl.def( pybind11::init( [](genesis::utils::SvgObject const &o){ return new genesis::utils::SvgObject(o); } ) );
		cl.def("assign", (class genesis::utils::SvgObject & (genesis::utils::SvgObject::*)(class genesis::utils::SvgObject)) &genesis::utils::SvgObject::operator=, "C++: genesis::utils::SvgObject::operator=(class genesis::utils::SvgObject) --> class genesis::utils::SvgObject &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("id", (const std::string & (genesis::utils::SvgObject::*)() const) &genesis::utils::SvgObject::id, "C++: genesis::utils::SvgObject::id() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgObject::*)() const) &genesis::utils::SvgObject::bounding_box, "C++: genesis::utils::SvgObject::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgObject const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgObject const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgObject::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgObject::write, "C++: genesis::utils::SvgObject::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
	}
	{ // genesis::utils::SvgStroke file:genesis/utils/formats/svg/attributes.hpp line:49
		pybind11::class_<genesis::utils::SvgStroke, std::shared_ptr<genesis::utils::SvgStroke>> cl(M("genesis::utils"), "SvgStroke", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgStroke(); } ), "doc" );
		cl.def( pybind11::init<enum genesis::utils::SvgStroke::Type>(), pybind11::arg("type") );

		cl.def( pybind11::init( [](class genesis::utils::Color const & a0){ return new genesis::utils::SvgStroke(a0); } ), "doc" , pybind11::arg("color"));
		cl.def( pybind11::init<class genesis::utils::Color, double>(), pybind11::arg("color"), pybind11::arg("width") );

		cl.def( pybind11::init<std::string>(), pybind11::arg("gradient_id") );

		cl.def( pybind11::init( [](genesis::utils::SvgStroke const &o){ return new genesis::utils::SvgStroke(o); } ) );

		pybind11::enum_<genesis::utils::SvgStroke::Type>(cl, "Type", "")
			.value("kColor", genesis::utils::SvgStroke::Type::kColor)
			.value("kGradient", genesis::utils::SvgStroke::Type::kGradient)
			.value("kNone", genesis::utils::SvgStroke::Type::kNone)
			.value("kOmit", genesis::utils::SvgStroke::Type::kOmit);


		pybind11::enum_<genesis::utils::SvgStroke::LineCap>(cl, "LineCap", "")
			.value("kOmit", genesis::utils::SvgStroke::LineCap::kOmit)
			.value("kButt", genesis::utils::SvgStroke::LineCap::kButt)
			.value("kSquare", genesis::utils::SvgStroke::LineCap::kSquare)
			.value("kRound", genesis::utils::SvgStroke::LineCap::kRound);


		pybind11::enum_<genesis::utils::SvgStroke::LineJoin>(cl, "LineJoin", "")
			.value("kOmit", genesis::utils::SvgStroke::LineJoin::kOmit)
			.value("kMiter", genesis::utils::SvgStroke::LineJoin::kMiter)
			.value("kRound", genesis::utils::SvgStroke::LineJoin::kRound)
			.value("kBevel", genesis::utils::SvgStroke::LineJoin::kBevel);

		cl.def_readwrite("type", &genesis::utils::SvgStroke::type);
		cl.def_readwrite("gradient_id", &genesis::utils::SvgStroke::gradient_id);
		cl.def_readwrite("color", &genesis::utils::SvgStroke::color);
		cl.def_readwrite("width", &genesis::utils::SvgStroke::width);
		cl.def_readwrite("width_unit", &genesis::utils::SvgStroke::width_unit);
		cl.def_readwrite("line_cap", &genesis::utils::SvgStroke::line_cap);
		cl.def_readwrite("line_join", &genesis::utils::SvgStroke::line_join);
		cl.def_readwrite("miterlimit", &genesis::utils::SvgStroke::miterlimit);
		cl.def_readwrite("dash_array", &genesis::utils::SvgStroke::dash_array);
		cl.def_readwrite("dash_offset", &genesis::utils::SvgStroke::dash_offset);
		cl.def("assign", (struct genesis::utils::SvgStroke & (genesis::utils::SvgStroke::*)(const struct genesis::utils::SvgStroke &)) &genesis::utils::SvgStroke::operator=, "C++: genesis::utils::SvgStroke::operator=(const struct genesis::utils::SvgStroke &) --> struct genesis::utils::SvgStroke &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::utils::SvgStroke::*)(std::ostream &) const) &genesis::utils::SvgStroke::write, "C++: genesis::utils::SvgStroke::write(std::ostream &) const --> void", pybind11::arg("out"));
	}
	{ // genesis::utils::SvgFill file:genesis/utils/formats/svg/attributes.hpp line:131
		pybind11::class_<genesis::utils::SvgFill, std::shared_ptr<genesis::utils::SvgFill>> cl(M("genesis::utils"), "SvgFill", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgFill(); } ), "doc" );
		cl.def( pybind11::init<enum genesis::utils::SvgFill::Type>(), pybind11::arg("type") );

		cl.def( pybind11::init<class genesis::utils::Color>(), pybind11::arg("color") );

		cl.def( pybind11::init<std::string>(), pybind11::arg("gradient_id") );

		cl.def( pybind11::init( [](genesis::utils::SvgFill const &o){ return new genesis::utils::SvgFill(o); } ) );

		pybind11::enum_<genesis::utils::SvgFill::Type>(cl, "Type", "")
			.value("kColor", genesis::utils::SvgFill::Type::kColor)
			.value("kGradient", genesis::utils::SvgFill::Type::kGradient)
			.value("kNone", genesis::utils::SvgFill::Type::kNone)
			.value("kOmit", genesis::utils::SvgFill::Type::kOmit);


		pybind11::enum_<genesis::utils::SvgFill::Rule>(cl, "Rule", "")
			.value("kNone", genesis::utils::SvgFill::Rule::kNone)
			.value("kNonZero", genesis::utils::SvgFill::Rule::kNonZero)
			.value("kEvenOdd", genesis::utils::SvgFill::Rule::kEvenOdd);

		cl.def_readwrite("type", &genesis::utils::SvgFill::type);
		cl.def_readwrite("gradient_id", &genesis::utils::SvgFill::gradient_id);
		cl.def_readwrite("color", &genesis::utils::SvgFill::color);
		cl.def_readwrite("rule", &genesis::utils::SvgFill::rule);
		cl.def("assign", (struct genesis::utils::SvgFill & (genesis::utils::SvgFill::*)(const struct genesis::utils::SvgFill &)) &genesis::utils::SvgFill::operator=, "C++: genesis::utils::SvgFill::operator=(const struct genesis::utils::SvgFill &) --> struct genesis::utils::SvgFill &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::utils::SvgFill::*)(std::ostream &) const) &genesis::utils::SvgFill::write, "C++: genesis::utils::SvgFill::write(std::ostream &) const --> void", pybind11::arg("out"));
	}
	{ // genesis::utils::SvgFont file:genesis/utils/formats/svg/attributes.hpp line:195
		pybind11::class_<genesis::utils::SvgFont, std::shared_ptr<genesis::utils::SvgFont>> cl(M("genesis::utils"), "SvgFont", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgFont(); } ), "doc" );
		cl.def( pybind11::init( [](double const & a0){ return new genesis::utils::SvgFont(a0); } ), "doc" , pybind11::arg("size"));
		cl.def( pybind11::init<double, const std::string &>(), pybind11::arg("size"), pybind11::arg("family") );

		cl.def( pybind11::init( [](genesis::utils::SvgFont const &o){ return new genesis::utils::SvgFont(o); } ) );
		cl.def_readwrite("size", &genesis::utils::SvgFont::size);
		cl.def_readwrite("family", &genesis::utils::SvgFont::family);
		cl.def("assign", (struct genesis::utils::SvgFont & (genesis::utils::SvgFont::*)(const struct genesis::utils::SvgFont &)) &genesis::utils::SvgFont::operator=, "C++: genesis::utils::SvgFont::operator=(const struct genesis::utils::SvgFont &) --> struct genesis::utils::SvgFont &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::utils::SvgFont::*)(std::ostream &) const) &genesis::utils::SvgFont::write, "C++: genesis::utils::SvgFont::write(std::ostream &) const --> void", pybind11::arg("out"));
	}
}
