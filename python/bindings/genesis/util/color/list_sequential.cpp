#include <genesis/util/color/color.hpp>
#include <genesis/util/color/list_sequential.hpp>
#include <genesis/util/color/map.hpp>
#include <genesis/util/color/normalization.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_util_color_list_sequential(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::color::color_list_sequential(enum genesis::util::color::ColorListSequential)
    // file:genesis/util/color/list_sequential.hpp line:308 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_sequential(enum
    // genesis::util::color::ColorListSequential)(enum genesis::util::color::ColorListSequential)
    // file:genesis/util/color/list_sequential.hpp line:308
    M( "genesis::util::color" )
        .def(
            "color_list_sequential",
            ( const class std::vector<
                class genesis::util::color::Color>& (*)( enum genesis::util::color::
                                                             ColorListSequential ) ) &
                genesis::util::color::color_list_sequential,
            "Get a sequential color palette.\n\n See ColorListSequential for the available "
            "ones.\n\nC++: genesis::util::color::color_list_sequential(enum "
            "genesis::util::color::ColorListSequential) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_sequential(const std::string &)
    // file:genesis/util/color/list_sequential.hpp line:319 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_sequential(const std::string &)(const std::string &)
    // file:genesis/util/color/list_sequential.hpp line:319
    M( "genesis::util::color" )
        .def(
            "color_list_sequential",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_sequential,
            "Get a sequential color palette by its name.\n\n The names used here are the same as "
            "in ColorListSequential, but without the leading `k`,\n i.e., using their original "
            "name (case insensitive and ignoring all non-alnum characters).\n A list of all color "
            "palettes offered here by their name can also be obtained from\n "
            "color_list_sequential_names().\n For instance, ColorListSequential::kViridis can be "
            "obtained here simply as `\"viridis\"`.\n\nC++: "
            "genesis::util::color::color_list_sequential(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_sequential_names()
    // file:genesis/util/color/list_sequential.hpp line:327 function-signature: class
    // std::vector<std::string > genesis::util::color::color_list_sequential_names()()
    // file:genesis/util/color/list_sequential.hpp line:327
    M( "genesis::util::color" )
        .def(
            "color_list_sequential_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_sequential_names,
            "Get the list of all available sequential color palettes as strings.\n\n See also "
            "ColorListSequential for the corresponding enum. Use the color list names provided "
            "here\n as arguments for color_list_sequential( std::string const& ).\n\nC++: "
            "genesis::util::color::color_list_sequential_names() --> class std::vector<std::string "
            ">"
        );

    { // genesis::util::color::ColorMap file:genesis/util/color/map.hpp line:62
        pybind11::
            class_<genesis::util::color::ColorMap, std::shared_ptr<genesis::util::color::ColorMap>>
                cl( M( "genesis::util::color" ),
                    "ColorMap",
                    "Store a list of colors and offer them as a map for values in range `[ 0.0, "
                    "1.0 ]`.\n\n The class is an abstraction of color lists, making them easier to "
                    "use for ranges, gradients, etc.\n When invoked, it interpolates between "
                    "entries of the list according to the provided value.\n It is best used in "
                    "combination with a ColorNormalization, so that arbitrary ranges can be\n "
                    "mapped into the allowed interval `[ 0.0, 1.0 ]`." );
        // function-signature: genesis::util::color::ColorMap::ColorMap()()
        // file:genesis/util/color/map.hpp line:70
        cl.def( pybind11::init( []() { return new genesis::util::color::ColorMap(); } ) );
        // function-signature: genesis::util::color::ColorMap::ColorMap(const class
        // std::vector<class genesis::util::color::Color> &)(const class std::vector<class
        // genesis::util::color::Color> &) file:genesis/util/color/map.hpp line:72
        cl.def(
            pybind11::init<const class std::vector<class genesis::util::color::Color>&>(),
            pybind11::arg( "colors" )
        );

        // function-signature: genesis::util::color::ColorMap::ColorMap(const class
        // genesis::util::color::ColorMap &)(const class genesis::util::color::ColorMap &)
        // file:genesis/util/color/map.hpp line:79
        cl.def( pybind11::init( []( genesis::util::color::ColorMap const& o ) {
            return new genesis::util::color::ColorMap( o );
        } ) );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::operator=(const class genesis::util::color::ColorMap
        // &)(const class genesis::util::color::ColorMap &) file:genesis/util/color/map.hpp line:82
        cl.def(
            "assign",
            ( class genesis::util::color::ColorMap &
              (genesis::util::color::ColorMap::*)(const class genesis::util::color::ColorMap&)) &
                genesis::util::color::ColorMap::operator=,
            "C++: genesis::util::color::ColorMap::operator=(const class "
            "genesis::util::color::ColorMap &) --> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class genesis::util::color::Color &
        // genesis::util::color::ColorMap::mask_color() const() file:genesis/util/color/map.hpp
        // line:102
        cl.def(
            "mask_color",
            ( const class genesis::util::color::Color& (genesis::util::color::ColorMap::*)() const
            ) & genesis::util::color::ColorMap::mask_color,
            "Color that indicates values equal to ColorNormalization::mask_value()\n or non-finite "
            "values.\n\n This color is used whenever an invalid (i.e., non-finite) value is "
            "requested. This happens\n for example for the ColorNormalization::mask_value(), which "
            "is a simple \"invalid value\"\n filter that can be used if a dataset contains a "
            "specific value to indicate N/A or NAN values.\n For example, some sensor data might "
            "use `99999` as this value. If this appears, and the\n "
            "ColorNormalization::mask_value() is set to that value, it will be mapped to a quited "
            "NAN\n by the ColorNormalization, and so end up getting the mask color here.\n Another "
            "instance where this happens is if negative values are used with the\n "
            "ColorNormalizationLogarithmic.\n\nC++: genesis::util::color::ColorMap::mask_color() "
            "const --> const class genesis::util::color::Color &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: const class genesis::util::color::Color &
        // genesis::util::color::ColorMap::over_color() const() file:genesis/util/color/map.hpp
        // line:112
        cl.def(
            "over_color",
            ( const class genesis::util::color::Color& (genesis::util::color::ColorMap::*)() const
            ) & genesis::util::color::ColorMap::over_color,
            "Color that indicates values greater than max().\n\n Only used if `clip_over() == "
            "false`.\n\nC++: genesis::util::color::ColorMap::over_color() const --> const class "
            "genesis::util::color::Color &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: const class genesis::util::color::Color &
        // genesis::util::color::ColorMap::under_color() const() file:genesis/util/color/map.hpp
        // line:122
        cl.def(
            "under_color",
            ( const class genesis::util::color::Color& (genesis::util::color::ColorMap::*)() const
            ) & genesis::util::color::ColorMap::under_color,
            "Color that indicates values less than min().\n\n Only used if `clip_under() == "
            "false`.\n\nC++: genesis::util::color::ColorMap::under_color() const --> const class "
            "genesis::util::color::Color &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: bool genesis::util::color::ColorMap::clip_under() const()
        // file:genesis/util/color/map.hpp line:132
        cl.def(
            "clip_under",
            ( bool( genesis::util::color::ColorMap::* )() const ) &
                genesis::util::color::ColorMap::clip_under,
            "Clip (clamp) values less than min() to be inside `[ min, max ]`.\n\n If set to "
            "`true`, under_color() is not used to indicate values out of range.\n\nC++: "
            "genesis::util::color::ColorMap::clip_under() const --> bool"
        );
        // function-signature: bool genesis::util::color::ColorMap::clip_over() const()
        // file:genesis/util/color/map.hpp line:142
        cl.def(
            "clip_over",
            ( bool( genesis::util::color::ColorMap::* )() const ) &
                genesis::util::color::ColorMap::clip_over,
            "Clip (clamp) values greater than max() to be inside `[ min, max ]`.\n\n If set to "
            "`true`, over_color() is not used to indicate values out of range.\n\nC++: "
            "genesis::util::color::ColorMap::clip_over() const --> bool"
        );
        // function-signature: bool genesis::util::color::ColorMap::reverse() const()
        // file:genesis/util/color/map.hpp line:150
        cl.def(
            "reverse",
            ( bool( genesis::util::color::ColorMap::* )() const ) &
                genesis::util::color::ColorMap::reverse,
            "Use the palette colors in reverse, back to front.\n\nC++: "
            "genesis::util::color::ColorMap::reverse() const --> bool"
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::mask_color(class genesis::util::color::Color)(class
        // genesis::util::color::Color) file:genesis/util/color/map.hpp line:162
        cl.def(
            "mask_color",
            ( class genesis::util::color::ColorMap &
              (genesis::util::color::ColorMap::*)( class genesis::util::color::Color ) ) &
                genesis::util::color::ColorMap::mask_color,
            "C++: genesis::util::color::ColorMap::mask_color(class genesis::util::color::Color) "
            "--> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::over_color(class genesis::util::color::Color)(class
        // genesis::util::color::Color) file:genesis/util/color/map.hpp line:171
        cl.def(
            "over_color",
            ( class genesis::util::color::ColorMap &
              (genesis::util::color::ColorMap::*)( class genesis::util::color::Color ) ) &
                genesis::util::color::ColorMap::over_color,
            "C++: genesis::util::color::ColorMap::over_color(class genesis::util::color::Color) "
            "--> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::under_color(class genesis::util::color::Color)(class
        // genesis::util::color::Color) file:genesis/util/color/map.hpp line:180
        cl.def(
            "under_color",
            ( class genesis::util::color::ColorMap &
              (genesis::util::color::ColorMap::*)( class genesis::util::color::Color ) ) &
                genesis::util::color::ColorMap::under_color,
            "C++: genesis::util::color::ColorMap::under_color(class genesis::util::color::Color) "
            "--> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::clip(bool)(bool) file:genesis/util/color/map.hpp line:189
        cl.def(
            "clip",
            ( class genesis::util::color::ColorMap & (genesis::util::color::ColorMap::*)(bool)) &
                genesis::util::color::ColorMap::clip,
            "Set both clip_under( bool ) and clip_over( bool ).\n\nC++: "
            "genesis::util::color::ColorMap::clip(bool) --> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::clip_under(bool)(bool) file:genesis/util/color/map.hpp
        // line:199
        cl.def(
            "clip_under",
            ( class genesis::util::color::ColorMap & (genesis::util::color::ColorMap::*)(bool)) &
                genesis::util::color::ColorMap::clip_under,
            "C++: genesis::util::color::ColorMap::clip_under(bool) --> class "
            "genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::clip_over(bool)(bool) file:genesis/util/color/map.hpp
        // line:208
        cl.def(
            "clip_over",
            ( class genesis::util::color::ColorMap & (genesis::util::color::ColorMap::*)(bool)) &
                genesis::util::color::ColorMap::clip_over,
            "C++: genesis::util::color::ColorMap::clip_over(bool) --> class "
            "genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::reverse(bool)(bool) file:genesis/util/color/map.hpp
        // line:214
        cl.def(
            "reverse",
            ( class genesis::util::color::ColorMap & (genesis::util::color::ColorMap::*)(bool)) &
                genesis::util::color::ColorMap::reverse,
            "C++: genesis::util::color::ColorMap::reverse(bool) --> class "
            "genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorMap &
        // genesis::util::color::ColorMap::palette(const class std::vector<class
        // genesis::util::color::Color> &)(const class std::vector<class
        // genesis::util::color::Color> &) file:genesis/util/color/map.hpp line:220
        cl.def(
            "palette",
            ( class genesis::util::color::ColorMap &
              (genesis::util::color::ColorMap::*)(const class std::vector<
                                                  class genesis::util::color::Color>&)) &
                genesis::util::color::ColorMap::palette,
            "C++: genesis::util::color::ColorMap::palette(const class std::vector<class "
            "genesis::util::color::Color> &) --> class genesis::util::color::ColorMap &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: const class std::vector<class genesis::util::color::Color> &
        // genesis::util::color::ColorMap::palette() const() file:genesis/util/color/map.hpp
        // line:233
        cl.def(
            "palette",
            ( const class std::vector<
                class genesis::util::color::Color>& (genesis::util::color::ColorMap::*)() const ) &
                genesis::util::color::ColorMap::palette,
            "Get the color list currently in use.\n\nC++: "
            "genesis::util::color::ColorMap::palette() const --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: bool genesis::util::color::ColorMap::empty() const()
        // file:genesis/util/color/map.hpp line:241
        cl.def(
            "empty",
            ( bool( genesis::util::color::ColorMap::* )() const ) &
                genesis::util::color::ColorMap::empty,
            "Return whether the Palette is empty, that is, no colors were set.\n\nC++: "
            "genesis::util::color::ColorMap::empty() const --> bool"
        );
        // function-signature: unsigned long genesis::util::color::ColorMap::size() const()
        // file:genesis/util/color/map.hpp line:249
        cl.def(
            "size",
            ( unsigned long ( genesis::util::color::ColorMap::* )() const ) &
                genesis::util::color::ColorMap::size,
            "Return the size of the map, that is, the number of colors in the list.\n\nC++: "
            "genesis::util::color::ColorMap::size() const --> unsigned long"
        );
        // function-signature: class genesis::util::color::Color
        // genesis::util::color::ColorMap::color(unsigned long) const(unsigned long)
        // file:genesis/util/color/map.hpp line:260
        cl.def(
            "color",
            ( class genesis::util::color::Color( genesis::util::color::ColorMap::* )( unsigned long
            ) const ) &
                genesis::util::color::ColorMap::color,
            "Return a particular color from the palette, module the palette size.\n\n This is "
            "useful for qualitative measures. The function respects the reverse() setting,\n and "
            "uses modulo for indices out of range, that is, it \"wraps around\".\n\nC++: "
            "genesis::util::color::ColorMap::color(unsigned long) const --> class "
            "genesis::util::color::Color",
            pybind11::arg( "index" )
        );
        // function-signature: class std::vector<class genesis::util::color::Color>
        // genesis::util::color::ColorMap::color_list(unsigned long) const(unsigned long)
        // file:genesis/util/color/map.hpp line:278
        cl.def(
            "color_list",
            []( genesis::util::color::ColorMap const& o
            ) -> std::vector<class genesis::util::color::Color> { return o.color_list(); },
            ""
        );
        cl.def(
            "color_list",
            ( class std::vector<
                class genesis::util::color::Color>( genesis::util::color::ColorMap::* )(
                unsigned long
            ) const ) &
                genesis::util::color::ColorMap::color_list,
            "Get a color list based on the palette, containing  colors sampled at equal\n distance "
            "across the palette.\n\n This is for example useful for creating a palette to write a "
            "bitmap file, see\n \n\n\n\n\n\n\nC++: "
            "genesis::util::color::ColorMap::color_list(unsigned long) const --> class "
            "std::vector<class genesis::util::color::Color>",
            pybind11::arg( "n" )
        );
        // function-signature: class genesis::util::color::Color
        // genesis::util::color::ColorMap::operator()(double) const(double)
        // file:genesis/util/color/map.hpp line:293
        cl.def(
            "__call__",
            ( class genesis::util::color::Color( genesis::util::color::ColorMap::* )( double ) const
            ) & genesis::util::color::ColorMap::operator(),
            "Return an interpolated color for a  in the range `[ 0.0, 1.0 ]`,\n representing a "
            "position in the palette.\n\n Values less than `0.0` are mapped to under_color() or to "
            "the first color of the map,\n depending on whether clip_under() is set. The same "
            "applies for values greater than `1.0`:\n Either they map to over_color() or the last "
            "color in the map, depending on whether\n clip_over() is set. Lastly, any non-finite "
            "values (e.g., NaN) are mapped to mask_color().\n\nC++: "
            "genesis::util::color::ColorMap::operator()(double) const --> class "
            "genesis::util::color::Color",
            pybind11::arg( "value" )
        );
        // function-signature: class std::vector<class genesis::util::color::Color>
        // genesis::util::color::ColorMap::operator()(const class std::vector<double> &) const(const
        // class std::vector<double> &) file:genesis/util/color/map.hpp line:300
        cl.def(
            "__call__",
            ( class std::vector<
                class genesis::util::color::
                    Color>( genesis::util::color::ColorMap::* )( const class std::vector<double>& )
                  const ) &
                genesis::util::color::ColorMap::operator(),
            "Return the mapped colors for a vector of \n\n Simply applies operator() ( double "
            "value ) to all values.\n\nC++: genesis::util::color::ColorMap::operator()(const class "
            "std::vector<double> &) const --> class std::vector<class genesis::util::color::Color>",
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::util::color::Color
        // genesis::util::color::ColorMap::operator()(const class
        // genesis::util::color::ColorNormalization &, double) const(const class
        // genesis::util::color::ColorNormalization &, double) file:genesis/util/color/map.hpp
        // line:321
        cl.def(
            "__call__",
            ( class genesis::util::color::Color( genesis::util::color::ColorMap::* )(
                const class genesis::util::color::ColorNormalization&, double
            ) const ) &
                genesis::util::color::ColorMap::operator(),
            "Return a Color for the given  normalized by \n     \n\nC++: "
            "genesis::util::color::ColorMap::operator()(const class "
            "genesis::util::color::ColorNormalization &, double) const --> class "
            "genesis::util::color::Color",
            pybind11::arg( "norm" ),
            pybind11::arg( "value" )
        );
        // function-signature: class std::vector<class genesis::util::color::Color>
        // genesis::util::color::ColorMap::operator()(const class
        // genesis::util::color::ColorNormalization &, const class std::vector<double> &)
        // const(const class genesis::util::color::ColorNormalization &, const class
        // std::vector<double> &) file:genesis/util/color/map.hpp line:326
        cl.def(
            "__call__",
            ( class std::vector<
                class genesis::util::color::
                    Color>( genesis::util::color::
                                ColorMap::* )( const class genesis::util::color::ColorNormalization&, const class std::vector<double>& )
                  const ) &
                genesis::util::color::ColorMap::operator(),
            "Return the mapped colors for a vector of  normalized by \n     \n\nC++: "
            "genesis::util::color::ColorMap::operator()(const class "
            "genesis::util::color::ColorNormalization &, const class std::vector<double> &) const "
            "--> class std::vector<class genesis::util::color::Color>",
            pybind11::arg( "norm" ),
            pybind11::arg( "values" )
        );
    }
}
