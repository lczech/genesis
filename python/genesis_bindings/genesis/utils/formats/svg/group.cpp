#include <functional>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/formats/svg/axis.hpp>
#include <genesis/utils/formats/svg/definitions.hpp>
#include <genesis/utils/formats/svg/gradient.hpp>
#include <genesis/utils/formats/svg/group.hpp>
#include <genesis/utils/formats/svg/helper.hpp>
#include <genesis/utils/formats/svg/object.hpp>
#include <genesis/utils/tools/tickmarks.hpp>
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

void bind_genesis_utils_formats_svg_group(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::SvgGroup file:genesis/utils/formats/svg/group.hpp line:50
		pybind11::class_<genesis::utils::SvgGroup, std::shared_ptr<genesis::utils::SvgGroup>> cl(M("genesis::utils"), "SvgGroup", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgGroup(); } ) );
		cl.def( pybind11::init( [](genesis::utils::SvgGroup const &o){ return new genesis::utils::SvgGroup(o); } ) );
		cl.def_readwrite("id", &genesis::utils::SvgGroup::id);
		cl.def_readwrite("content_", &genesis::utils::SvgGroup::content_);
		cl.def_readwrite("transform", &genesis::utils::SvgGroup::transform);
		cl.def_readwrite("hyperlink_", &genesis::utils::SvgGroup::hyperlink_);
		cl.def("assign", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)(const struct genesis::utils::SvgGroup &)) &genesis::utils::SvgGroup::operator=, "C++: genesis::utils::SvgGroup::operator=(const struct genesis::utils::SvgGroup &) --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("bounding_box", (struct genesis::utils::SvgBox (genesis::utils::SvgGroup::*)() const) &genesis::utils::SvgGroup::bounding_box, "C++: genesis::utils::SvgGroup::bounding_box() const --> struct genesis::utils::SvgBox");
		cl.def("write", [](genesis::utils::SvgGroup const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", [](genesis::utils::SvgGroup const &o, std::ostream & a0, unsigned long const & a1) -> void { return o.write(a0, a1); }, "", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("write", (void (genesis::utils::SvgGroup::*)(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const) &genesis::utils::SvgGroup::write, "C++: genesis::utils::SvgGroup::write(std::ostream &, unsigned long, const struct genesis::utils::SvgDrawingOptions &) const --> void", pybind11::arg("out"), pybind11::arg("indent"), pybind11::arg("options"));
		cl.def("add", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)(const class genesis::utils::SvgObject &)) &genesis::utils::SvgGroup::add, "Add an SvgObject to the document.\n\n Returns the SvgDocument in order to allow for a fluent interface.\n\nC++: genesis::utils::SvgGroup::add(const class genesis::utils::SvgObject &) --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal, pybind11::arg("object"));
		cl.def("reverse", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)()) &genesis::utils::SvgGroup::reverse, "Reverse the order to the stored SvgObject%s.\n\nC++: genesis::utils::SvgGroup::reverse() --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal);
		cl.def("set_hyperlink", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)(const std::string &)) &genesis::utils::SvgGroup::set_hyperlink, "Simple tweak to allow making parts of an image clickable.\n\n This will simply add an `` tag around the group, making everything in it clickable\n and hyperlink to the given URL. We currently do not have any per-object hyperlinks,\n and hence instead everything with a hyperlink has to go in a group.\n\nC++: genesis::utils::SvgGroup::set_hyperlink(const std::string &) --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("set_hyperlink", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)(const class std::unordered_map<std::string, std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, std::string > > > &)) &genesis::utils::SvgGroup::set_hyperlink, "Tweak to allow making parts of an image cickable.\n\n This is the same as the simply string version of this function, but additionally allows\n to set extra attributes to be used within the `` tag. Note that the key `href` needs\n to be present in the map, in order to actually have a hyerlink to be cickable.\n\nC++: genesis::utils::SvgGroup::set_hyperlink(const class std::unordered_map<std::string, std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, std::string > > > &) --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal, pybind11::arg("values"));
		cl.def("__lshift__", (struct genesis::utils::SvgGroup & (genesis::utils::SvgGroup::*)(const class genesis::utils::SvgObject &)) &genesis::utils::SvgGroup::operator<<, "Shortcut operator for add(), which allows an even more fluent interface.\n\nC++: genesis::utils::SvgGroup::operator<<(const class genesis::utils::SvgObject &) --> struct genesis::utils::SvgGroup &", pybind11::return_value_policy::reference_internal, pybind11::arg("object"));
		cl.def("empty", (bool (genesis::utils::SvgGroup::*)() const) &genesis::utils::SvgGroup::empty, "C++: genesis::utils::SvgGroup::empty() const --> bool");
		cl.def("clear", (void (genesis::utils::SvgGroup::*)()) &genesis::utils::SvgGroup::clear, "C++: genesis::utils::SvgGroup::clear() --> void");
	}
	{ // genesis::utils::SvgAxisSettings file:genesis/utils/formats/svg/axis.hpp line:49
		pybind11::class_<genesis::utils::SvgAxisSettings, std::shared_ptr<genesis::utils::SvgAxisSettings>> cl(M("genesis::utils"), "SvgAxisSettings", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgAxisSettings(); } ) );

		pybind11::enum_<genesis::utils::SvgAxisSettings::Position>(cl, "Position", "")
			.value("kLeft", genesis::utils::SvgAxisSettings::Position::kLeft)
			.value("kRight", genesis::utils::SvgAxisSettings::Position::kRight)
			.value("kTop", genesis::utils::SvgAxisSettings::Position::kTop)
			.value("kBottom", genesis::utils::SvgAxisSettings::Position::kBottom);

		cl.def_readwrite("position", &genesis::utils::SvgAxisSettings::position);
		cl.def_readwrite("length", &genesis::utils::SvgAxisSettings::length);
		cl.def_readwrite("offset", &genesis::utils::SvgAxisSettings::offset);
		cl.def_readwrite("tick_size", &genesis::utils::SvgAxisSettings::tick_size);
		cl.def_readwrite("label_text_size", &genesis::utils::SvgAxisSettings::label_text_size);
		cl.def_readwrite("axis_text_size", &genesis::utils::SvgAxisSettings::axis_text_size);
	}
	// genesis::utils::make_svg_axis(const struct genesis::utils::SvgAxisSettings &, const class std::map<double, std::string, struct std::less<double>, class std::allocator<struct std::pair<const double, std::string > > > &, const std::string &) file:genesis/utils/formats/svg/axis.hpp line:113
	M("genesis::utils").def("make_svg_axis", [](const struct genesis::utils::SvgAxisSettings & a0, const class std::map<double, std::string, struct std::less<double>, class std::allocator<struct std::pair<const double, std::string > > > & a1) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_axis(a0, a1); }, "", pybind11::arg("settings"), pybind11::arg("labels"));
	M("genesis::utils").def("make_svg_axis", (struct genesis::utils::SvgGroup (*)(const struct genesis::utils::SvgAxisSettings &, const class std::map<double, std::string, struct std::less<double>, class std::allocator<struct std::pair<const double, std::string > > > &, const std::string &)) &genesis::utils::make_svg_axis, "Simple helper to make an axis.\n\n The helper currently only draws lines and puts the  alongside. The given \n have already to be pre-computed, with their position (`first` of the map) being their relative\n position in `[ 0.0, 1.0 ]` along the axis.\n\nC++: genesis::utils::make_svg_axis(const struct genesis::utils::SvgAxisSettings &, const class std::map<double, std::string, struct std::less<double>, class std::allocator<struct std::pair<const double, std::string > > > &, const std::string &) --> struct genesis::utils::SvgGroup", pybind11::arg("settings"), pybind11::arg("labels"), pybind11::arg("name"));

	// genesis::utils::make_svg_axis(const struct genesis::utils::SvgAxisSettings &, const class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > &, const std::string &, bool) file:genesis/utils/formats/svg/axis.hpp line:127
	M("genesis::utils").def("make_svg_axis", [](const struct genesis::utils::SvgAxisSettings & a0, const class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > & a1) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_axis(a0, a1); }, "", pybind11::arg("settings"), pybind11::arg("labels"));
	M("genesis::utils").def("make_svg_axis", [](const struct genesis::utils::SvgAxisSettings & a0, const class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > & a1, const std::string & a2) -> genesis::utils::SvgGroup { return genesis::utils::make_svg_axis(a0, a1, a2); }, "", pybind11::arg("settings"), pybind11::arg("labels"), pybind11::arg("name"));
	M("genesis::utils").def("make_svg_axis", (struct genesis::utils::SvgGroup (*)(const struct genesis::utils::SvgAxisSettings &, const class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > &, const std::string &, bool)) &genesis::utils::make_svg_axis, "Simple helper to make an axis.\n\n This overload of the function takes the result of the Tickmarks class when creating labels,\n and forwards it to the other overload that takes string labeles instead, for convenience.\n When using  (default `true`), `std::round` is called on the label values first,\n in order to print integer numbers.\n\nC++: genesis::utils::make_svg_axis(const struct genesis::utils::SvgAxisSettings &, const class std::vector<struct genesis::utils::Tickmarks::LabeledTick, class std::allocator<struct genesis::utils::Tickmarks::LabeledTick> > &, const std::string &, bool) --> struct genesis::utils::SvgGroup", pybind11::arg("settings"), pybind11::arg("labels"), pybind11::arg("name"), pybind11::arg("round_labels"));

	{ // genesis::utils::SvgDefinitions file:genesis/utils/formats/svg/definitions.hpp line:47
		pybind11::class_<genesis::utils::SvgDefinitions, std::shared_ptr<genesis::utils::SvgDefinitions>> cl(M("genesis::utils"), "SvgDefinitions", "");
		cl.def( pybind11::init( [](genesis::utils::SvgDefinitions const &o){ return new genesis::utils::SvgDefinitions(o); } ) );
		cl.def("assign", (class genesis::utils::SvgDefinitions & (genesis::utils::SvgDefinitions::*)(class genesis::utils::SvgDefinitions)) &genesis::utils::SvgDefinitions::operator=, "C++: genesis::utils::SvgDefinitions::operator=(class genesis::utils::SvgDefinitions) --> class genesis::utils::SvgDefinitions &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("write", [](genesis::utils::SvgDefinitions const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", (void (genesis::utils::SvgDefinitions::*)(std::ostream &, unsigned long) const) &genesis::utils::SvgDefinitions::write, "C++: genesis::utils::SvgDefinitions::write(std::ostream &, unsigned long) const --> void", pybind11::arg("out"), pybind11::arg("indent"));
	}
	{ // genesis::utils::SvgGradientStop file:genesis/utils/formats/svg/gradient.hpp line:52
		pybind11::class_<genesis::utils::SvgGradientStop, std::shared_ptr<genesis::utils::SvgGradientStop>> cl(M("genesis::utils"), "SvgGradientStop", "");
		cl.def( pybind11::init( [](double const & a0){ return new genesis::utils::SvgGradientStop(a0); } ), "doc" , pybind11::arg("offset"));
		cl.def( pybind11::init<double, const class genesis::utils::Color &>(), pybind11::arg("offset"), pybind11::arg("stop_color") );

		cl.def( pybind11::init( [](genesis::utils::SvgGradientStop const &o){ return new genesis::utils::SvgGradientStop(o); } ) );
		cl.def_readwrite("offset", &genesis::utils::SvgGradientStop::offset);
		cl.def_readwrite("stop_color", &genesis::utils::SvgGradientStop::stop_color);
		cl.def("assign", (struct genesis::utils::SvgGradientStop & (genesis::utils::SvgGradientStop::*)(const struct genesis::utils::SvgGradientStop &)) &genesis::utils::SvgGradientStop::operator=, "C++: genesis::utils::SvgGradientStop::operator=(const struct genesis::utils::SvgGradientStop &) --> struct genesis::utils::SvgGradientStop &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("validate", (void (genesis::utils::SvgGradientStop::*)() const) &genesis::utils::SvgGradientStop::validate, "C++: genesis::utils::SvgGradientStop::validate() const --> void");
		cl.def("write", (void (genesis::utils::SvgGradientStop::*)(std::ostream &) const) &genesis::utils::SvgGradientStop::write, "C++: genesis::utils::SvgGradientStop::write(std::ostream &) const --> void", pybind11::arg("out"));
	}
	{ // genesis::utils::SvgGradientLinear file:genesis/utils/formats/svg/gradient.hpp line:102
		pybind11::class_<genesis::utils::SvgGradientLinear, std::shared_ptr<genesis::utils::SvgGradientLinear>> cl(M("genesis::utils"), "SvgGradientLinear", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::SvgGradientLinear(); } ) );
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::utils::SvgGradientLinear(a0); } ), "doc" , pybind11::arg("id"));
		cl.def( pybind11::init( [](const std::string & a0, struct genesis::utils::SvgPoint const & a1){ return new genesis::utils::SvgGradientLinear(a0, a1); } ), "doc" , pybind11::arg("id"), pybind11::arg("point_1"));
		cl.def( pybind11::init<const std::string &, struct genesis::utils::SvgPoint, struct genesis::utils::SvgPoint>(), pybind11::arg("id"), pybind11::arg("point_1"), pybind11::arg("point_2") );

		cl.def( pybind11::init( [](genesis::utils::SvgGradientLinear const &o){ return new genesis::utils::SvgGradientLinear(o); } ) );

		pybind11::enum_<genesis::utils::SvgGradientLinear::SpreadMethod>(cl, "SpreadMethod", "")
			.value("kPad", genesis::utils::SvgGradientLinear::SpreadMethod::kPad)
			.value("kRepeat", genesis::utils::SvgGradientLinear::SpreadMethod::kRepeat)
			.value("kReflect", genesis::utils::SvgGradientLinear::SpreadMethod::kReflect);

		cl.def_readwrite("id", &genesis::utils::SvgGradientLinear::id);
		cl.def_readwrite("point_1", &genesis::utils::SvgGradientLinear::point_1);
		cl.def_readwrite("point_2", &genesis::utils::SvgGradientLinear::point_2);
		cl.def_readwrite("spread_method", &genesis::utils::SvgGradientLinear::spread_method);
		cl.def_readwrite("stops", &genesis::utils::SvgGradientLinear::stops);
		cl.def_readwrite("transform", &genesis::utils::SvgGradientLinear::transform);
		cl.def("assign", (struct genesis::utils::SvgGradientLinear & (genesis::utils::SvgGradientLinear::*)(const struct genesis::utils::SvgGradientLinear &)) &genesis::utils::SvgGradientLinear::operator=, "C++: genesis::utils::SvgGradientLinear::operator=(const struct genesis::utils::SvgGradientLinear &) --> struct genesis::utils::SvgGradientLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("validate", (void (genesis::utils::SvgGradientLinear::*)() const) &genesis::utils::SvgGradientLinear::validate, "C++: genesis::utils::SvgGradientLinear::validate() const --> void");
		cl.def("write", [](genesis::utils::SvgGradientLinear const &o, std::ostream & a0) -> void { return o.write(a0); }, "", pybind11::arg("out"));
		cl.def("write", (void (genesis::utils::SvgGradientLinear::*)(std::ostream &, unsigned long) const) &genesis::utils::SvgGradientLinear::write, "C++: genesis::utils::SvgGradientLinear::write(std::ostream &, unsigned long) const --> void", pybind11::arg("out"), pybind11::arg("indent"));
		cl.def("set_stops", (struct genesis::utils::SvgGradientLinear & (genesis::utils::SvgGradientLinear::*)(const class std::map<double, class genesis::utils::Color, struct std::less<double>, class std::allocator<struct std::pair<const double, class genesis::utils::Color> > > &)) &genesis::utils::SvgGradientLinear::set_stops, "C++: genesis::utils::SvgGradientLinear::set_stops(const class std::map<double, class genesis::utils::Color, struct std::less<double>, class std::allocator<struct std::pair<const double, class genesis::utils::Color> > > &) --> struct genesis::utils::SvgGradientLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("ranges"));
		cl.def("add_stop", (struct genesis::utils::SvgGradientLinear & (genesis::utils::SvgGradientLinear::*)(const struct genesis::utils::SvgGradientStop &)) &genesis::utils::SvgGradientLinear::add_stop, "C++: genesis::utils::SvgGradientLinear::add_stop(const struct genesis::utils::SvgGradientStop &) --> struct genesis::utils::SvgGradientLinear &", pybind11::return_value_policy::reference_internal, pybind11::arg("stop"));
		cl.def("empty", (bool (genesis::utils::SvgGradientLinear::*)() const) &genesis::utils::SvgGradientLinear::empty, "C++: genesis::utils::SvgGradientLinear::empty() const --> bool");
	}
}
