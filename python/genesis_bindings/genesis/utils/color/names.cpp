#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/map.hpp>
#include <genesis/utils/color/names.hpp>
#include <genesis/utils/color/normalization.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

// genesis::utils::ColorNormalization file:genesis/utils/color/normalization.hpp line:52
struct PyCallBack_genesis_utils_ColorNormalization : public genesis::utils::ColorNormalization {
	using genesis::utils::ColorNormalization::ColorNormalization;

	double normalize_(double a0) const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalization *>(this), "normalize_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>(a0);
			if (pybind11::detail::cast_is_temporary_value_reference<double>::value) {
				static pybind11::detail::override_caster_t<double> caster;
				return pybind11::detail::cast_ref<double>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<double>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"ColorNormalization::normalize_\"");
	}
	bool is_valid_() const override {
		pybind11::gil_scoped_acquire gil;
		pybind11::function overload = pybind11::get_overload(static_cast<const genesis::utils::ColorNormalization *>(this), "is_valid_");
		if (overload) {
			auto o = overload.operator()<pybind11::return_value_policy::reference>();
			if (pybind11::detail::cast_is_temporary_value_reference<bool>::value) {
				static pybind11::detail::override_caster_t<bool> caster;
				return pybind11::detail::cast_ref<bool>(std::move(o), caster);
			}
			else return pybind11::detail::cast_safe<bool>(std::move(o));
		}
		pybind11::pybind11_fail("Tried to call pure virtual function \"ColorNormalization::is_valid_\"");
	}
};

void bind_genesis_utils_color_names(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::is_color_name(const std::string &) file:genesis/utils/color/names.hpp line:51
	M("genesis::utils").def("is_color_name", (bool (*)(const std::string &)) &genesis::utils::is_color_name, "Return `true` if the name represents one of the named colors offered by genesis,\n which is (currently) a shortcut for is_web_color_name() and is_xkcd_color_name().\n\nC++: genesis::utils::is_color_name(const std::string &) --> bool", pybind11::arg("name"));

	// genesis::utils::color_from_name(const std::string &) file:genesis/utils/color/names.hpp line:57
	M("genesis::utils").def("color_from_name", (class genesis::utils::Color (*)(const std::string &)) &genesis::utils::color_from_name, "Return the color represented by the given name,\n which is (currently) a shortcut for color_from_name_web() and color_from_name_xkcd().\n\nC++: genesis::utils::color_from_name(const std::string &) --> class genesis::utils::Color", pybind11::arg("name"));

	// genesis::utils::color_palette_web() file:genesis/utils/color/names.hpp line:63
	M("genesis::utils").def("color_palette_web", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (*)()) &genesis::utils::color_palette_web, "C++: genesis::utils::color_palette_web() --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >");

	// genesis::utils::is_web_color_name(const std::string &) file:genesis/utils/color/names.hpp line:70
	M("genesis::utils").def("is_web_color_name", (bool (*)(const std::string &)) &genesis::utils::is_web_color_name, "Return true iff the given name is a named web color.\n\n Names are compared only by their alnum chars, and the letter case is ignored.\n\nC++: genesis::utils::is_web_color_name(const std::string &) --> bool", pybind11::arg("name"));

	// genesis::utils::color_from_name_web(const std::string &) file:genesis/utils/color/names.hpp line:78
	M("genesis::utils").def("color_from_name_web", (class genesis::utils::Color (*)(const std::string &)) &genesis::utils::color_from_name_web, "Retrieve a named web color by name.\n\n Names are compared only by their alnum chars, and the letter case is ignored.\n If the color name does not exist, an `std::invalid_argument` exception is thrown.\n\nC++: genesis::utils::color_from_name_web(const std::string &) --> class genesis::utils::Color", pybind11::arg("name"));

	// genesis::utils::color_palette_xkcd() file:genesis/utils/color/names.hpp line:84
	M("genesis::utils").def("color_palette_xkcd", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (*)()) &genesis::utils::color_palette_xkcd, "C++: genesis::utils::color_palette_xkcd() --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >");

	// genesis::utils::is_xkcd_color_name(const std::string &) file:genesis/utils/color/names.hpp line:92
	M("genesis::utils").def("is_xkcd_color_name", (bool (*)(const std::string &)) &genesis::utils::is_xkcd_color_name, "Return true iff the given name is a named xkcd color.\n\n Names are compared only by their alnum chars, and the letter case is ignored.\n See color_from_name_xkcd() for details on this color list.\n\nC++: genesis::utils::is_xkcd_color_name(const std::string &) --> bool", pybind11::arg("name"));

	// genesis::utils::color_from_name_xkcd(const std::string &) file:genesis/utils/color/names.hpp line:106
	M("genesis::utils").def("color_from_name_xkcd", (class genesis::utils::Color (*)(const std::string &)) &genesis::utils::color_from_name_xkcd, "Retrieve a named xkcd color by name.\n\n Names are compared only by their alnum chars, and the letter case is ignored.\n If the color name does not exist, an `std::invalid_argument` exception is thrown.\n\n The colors are taken from an [https://xkcd.com/](xkcd) color survey.\n They were published under Public Domain, http://creativecommons.org/publicdomain/zero/1.0/.\n See https://xkcd.com/color/rgb/ and https://blog.xkcd.com/2010/05/03/color-survey-results/\n for details. See also our\n \n\n\n\nC++: genesis::utils::color_from_name_xkcd(const std::string &) --> class genesis::utils::Color", pybind11::arg("name"));

	{ // genesis::utils::ColorMap file:genesis/utils/color/map.hpp line:61
		pybind11::class_<genesis::utils::ColorMap, std::shared_ptr<genesis::utils::ColorMap>> cl(M("genesis::utils"), "ColorMap", "Store a list of colors and offer them as a map for values in range `[ 0.0, 1.0 ]`.\n\n The class is an abstraction of color lists, making them easier to use for ranges, gradients, etc.\n When invoked, it interpolates between entries of the list according to the provided value.\n It is best used in combination with a ColorNormalization, so that arbitrary ranges can be\n mapped into the allowed interval `[ 0.0, 1.0 ]`.");
		cl.def( pybind11::init( [](){ return new genesis::utils::ColorMap(); } ) );
		cl.def( pybind11::init<const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &>(), pybind11::arg("colors") );

		cl.def( pybind11::init( [](genesis::utils::ColorMap const &o){ return new genesis::utils::ColorMap(o); } ) );
		cl.def("assign", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(const class genesis::utils::ColorMap &)) &genesis::utils::ColorMap::operator=, "C++: genesis::utils::ColorMap::operator=(const class genesis::utils::ColorMap &) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("mask_color", (const class genesis::utils::Color & (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::mask_color, "Color that indicates values equal to ColorNormalization::mask_value()\n or non-finite values.\n\n This color is used whenever an invalid (i.e., non-finite) value is requested. This happens\n for example for the ColorNormalization::mask_value(), which is a simple \"invalid value\"\n filter that can be used if a dataset contains a specific value to indicate N/A or NAN values.\n For example, some sensor data might use `99999` as this value. If this appears, and the\n ColorNormalization::mask_value() is set to that value, it will be mapped to a quited NAN\n by the ColorNormalization, and so end up getting the mask color here.\n Another instance where this happens is if negative values are used with the\n ColorNormalizationLogarithmic.\n\nC++: genesis::utils::ColorMap::mask_color() const --> const class genesis::utils::Color &", pybind11::return_value_policy::reference_internal);
		cl.def("over_color", (const class genesis::utils::Color & (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::over_color, "Color that indicates values greater than max().\n\n Only used if `clip_over() == false`.\n\nC++: genesis::utils::ColorMap::over_color() const --> const class genesis::utils::Color &", pybind11::return_value_policy::reference_internal);
		cl.def("under_color", (const class genesis::utils::Color & (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::under_color, "Color that indicates values less than min().\n\n Only used if `clip_under() == false`.\n\nC++: genesis::utils::ColorMap::under_color() const --> const class genesis::utils::Color &", pybind11::return_value_policy::reference_internal);
		cl.def("clip_under", (bool (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::clip_under, "Clip (clamp) values less than min() to be inside `[ min, max ]`.\n\n If set to `true`, under_color() is not used to indicate values out of range.\n\nC++: genesis::utils::ColorMap::clip_under() const --> bool");
		cl.def("clip_over", (bool (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::clip_over, "Clip (clamp) values greater than max() to be inside `[ min, max ]`.\n\n If set to `true`, over_color() is not used to indicate values out of range.\n\nC++: genesis::utils::ColorMap::clip_over() const --> bool");
		cl.def("reverse", (bool (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::reverse, "Use the palette colors in reverse, back to front.\n\nC++: genesis::utils::ColorMap::reverse() const --> bool");
		cl.def("mask_color", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(class genesis::utils::Color)) &genesis::utils::ColorMap::mask_color, "mask_color()\n\nC++: genesis::utils::ColorMap::mask_color(class genesis::utils::Color) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("over_color", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(class genesis::utils::Color)) &genesis::utils::ColorMap::over_color, "over_color()\n\nC++: genesis::utils::ColorMap::over_color(class genesis::utils::Color) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("under_color", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(class genesis::utils::Color)) &genesis::utils::ColorMap::under_color, "under_color()\n\nC++: genesis::utils::ColorMap::under_color(class genesis::utils::Color) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("clip", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(bool)) &genesis::utils::ColorMap::clip, "Set both clip_under( bool ) and clip_over( bool ).\n\nC++: genesis::utils::ColorMap::clip(bool) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("clip_under", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(bool)) &genesis::utils::ColorMap::clip_under, "clip_under()\n\nC++: genesis::utils::ColorMap::clip_under(bool) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("clip_over", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(bool)) &genesis::utils::ColorMap::clip_over, "clip_over()\n\nC++: genesis::utils::ColorMap::clip_over(bool) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("reverse", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(bool)) &genesis::utils::ColorMap::reverse, "C++: genesis::utils::ColorMap::reverse(bool) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("palette", (class genesis::utils::ColorMap & (genesis::utils::ColorMap::*)(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &)) &genesis::utils::ColorMap::palette, "C++: genesis::utils::ColorMap::palette(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &) --> class genesis::utils::ColorMap &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("palette", (const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > & (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::palette, "Get the color list currently in use.\n\nC++: genesis::utils::ColorMap::palette() const --> const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &", pybind11::return_value_policy::reference_internal);
		cl.def("empty", (bool (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::empty, "Return whether the Palette is empty, that is, no colors were set.\n\nC++: genesis::utils::ColorMap::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::utils::ColorMap::*)() const) &genesis::utils::ColorMap::size, "Return the size of the map, that is, the number of colors in the list.\n\nC++: genesis::utils::ColorMap::size() const --> unsigned long");
		cl.def("color", (class genesis::utils::Color (genesis::utils::ColorMap::*)(unsigned long) const) &genesis::utils::ColorMap::color, "Return a particular color from the palette, module the palette size.\n\n This is useful for qualitative measures. The function respects the reverse() setting,\n and uses modulo for indices out of range, that is, it \"wraps around\".\n\nC++: genesis::utils::ColorMap::color(unsigned long) const --> class genesis::utils::Color", pybind11::arg("index"));
		cl.def("color_list", [](genesis::utils::ColorMap const &o) -> std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > { return o.color_list(); }, "");
		cl.def("color_list", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (genesis::utils::ColorMap::*)(unsigned long) const) &genesis::utils::ColorMap::color_list, "Get a color list based on the palette, containing  colors sampled at equal\n distance across the palette.\n\n This is for example useful for creating a palette to write a bitmap file, see\n \n\n\n\n\n\n\nC++: genesis::utils::ColorMap::color_list(unsigned long) const --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >", pybind11::arg("n"));
		cl.def("__call__", (class genesis::utils::Color (genesis::utils::ColorMap::*)(double) const) &genesis::utils::ColorMap::operator(), "Return an interpolated color for a  in the range `[ 0.0, 1.0 ]`,\n representing a position in the palette.\n\n Values less than `0.0` are mapped to under_color() or to the first color of the map,\n depending on whether clip_under() is set. The same applies for values greater than `1.0`:\n Either they map to over_color() or the last color in the map, depending on whether\n clip_over() is set. Lastly, any non-finite values (e.g., NaN) are mapped to mask_color().\n\nC++: genesis::utils::ColorMap::operator()(double) const --> class genesis::utils::Color", pybind11::arg("value"));
		cl.def("__call__", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (genesis::utils::ColorMap::*)(const class std::vector<double, class std::allocator<double> > &) const) &genesis::utils::ColorMap::operator(), "Return the mapped colors for a vector of \n\n Simply applies operator() ( double value ) to all values.\n\nC++: genesis::utils::ColorMap::operator()(const class std::vector<double, class std::allocator<double> > &) const --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >", pybind11::arg("values"));
		cl.def("__call__", (class genesis::utils::Color (genesis::utils::ColorMap::*)(const class genesis::utils::ColorNormalization &, double) const) &genesis::utils::ColorMap::operator(), "Return a Color for the given  normalized by \n     \n\nC++: genesis::utils::ColorMap::operator()(const class genesis::utils::ColorNormalization &, double) const --> class genesis::utils::Color", pybind11::arg("norm"), pybind11::arg("value"));
		cl.def("__call__", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (genesis::utils::ColorMap::*)(const class genesis::utils::ColorNormalization &, const class std::vector<double, class std::allocator<double> > &) const) &genesis::utils::ColorMap::operator(), "Return the mapped colors for a vector of  normalized by \n     \n\nC++: genesis::utils::ColorMap::operator()(const class genesis::utils::ColorNormalization &, const class std::vector<double, class std::allocator<double> > &) const --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >", pybind11::arg("norm"), pybind11::arg("values"));
	}
	{ // genesis::utils::ColorNormalization file:genesis/utils/color/normalization.hpp line:52
		pybind11::class_<genesis::utils::ColorNormalization, std::shared_ptr<genesis::utils::ColorNormalization>, PyCallBack_genesis_utils_ColorNormalization> cl(M("genesis::utils"), "ColorNormalization", "Base class for color normalization.");
		cl.def( pybind11::init( [](){ return new PyCallBack_genesis_utils_ColorNormalization(); } ) );
		cl.def(pybind11::init<PyCallBack_genesis_utils_ColorNormalization const &>());
		cl.def("__call__", (class std::vector<double, class std::allocator<double> > (genesis::utils::ColorNormalization::*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) const) &genesis::utils::ColorNormalization::operator()<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ColorNormalization::operator()(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) const --> class std::vector<double, class std::allocator<double> >", pybind11::arg("first"), pybind11::arg("last"));
		cl.def("assign", (class genesis::utils::ColorNormalization & (genesis::utils::ColorNormalization::*)(const class genesis::utils::ColorNormalization &)) &genesis::utils::ColorNormalization::operator=, "C++: genesis::utils::ColorNormalization::operator=(const class genesis::utils::ColorNormalization &) --> class genesis::utils::ColorNormalization &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("mask_value", (double (genesis::utils::ColorNormalization::*)() const) &genesis::utils::ColorNormalization::mask_value, "Mask value that identifies invalid values.\n\n When querying a value that compares equal to the mask value, the color functions\n return mask_color() instead of a Color from the palette. This is meant as a simple means\n of filtering and visualizing invalid values.\n\n The mask value is initialized to `quiet_NaN`, so that it never compares equal to anything.\n Thus, it is not used by default has to be explicitly set to a different value.\n\nC++: genesis::utils::ColorNormalization::mask_value() const --> double");
		cl.def("is_valid", (bool (genesis::utils::ColorNormalization::*)() const) &genesis::utils::ColorNormalization::is_valid, "Return whether ranges and other values are correct.\n\nC++: genesis::utils::ColorNormalization::is_valid() const --> bool");
		cl.def("mask_value", (class genesis::utils::ColorNormalization & (genesis::utils::ColorNormalization::*)(double)) &genesis::utils::ColorNormalization::mask_value, "mask_value()\n\nC++: genesis::utils::ColorNormalization::mask_value(double) --> class genesis::utils::ColorNormalization &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("__call__", (double (genesis::utils::ColorNormalization::*)(double) const) &genesis::utils::ColorNormalization::operator(), "Normalize a  into range `[ 0.0, 1.0 ]`.\n\n The function first checks whether the value is finite or mask_value(),\n and returns `quiet_NaN` if so. If not, the normalization is applied.\n\nC++: genesis::utils::ColorNormalization::operator()(double) const --> double", pybind11::arg("value"));
		cl.def("__call__", (class std::vector<double, class std::allocator<double> > (genesis::utils::ColorNormalization::*)(const class std::vector<double, class std::allocator<double> > &) const) &genesis::utils::ColorNormalization::operator(), "Return the normalized values for a vector of \n\n Simply applies operator() ( double value ) to all values.\n\nC++: genesis::utils::ColorNormalization::operator()(const class std::vector<double, class std::allocator<double> > &) const --> class std::vector<double, class std::allocator<double> >", pybind11::arg("values"));
	}
}
