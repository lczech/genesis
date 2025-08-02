#include <functional>
#include <genesis/util/color/color.hpp>
#include <genesis/util/color/function.hpp>
#include <genesis/util/color/helper.hpp>
#include <genesis/util/color/map.hpp>
#include <genesis/util/color/norm_boundary.hpp>
#include <genesis/util/color/norm_diverging.hpp>
#include <genesis/util/color/norm_linear.hpp>
#include <genesis/util/color/norm_logarithmic.hpp>
#include <genesis/util/color/normalization.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>
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

void bind_genesis_util_color_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::color::color_from_bytes(unsigned char, unsigned char, unsigned char, unsigned
    // char) file:genesis/util/color/function.hpp line:57 function-signature: class
    // genesis::util::color::Color genesis::util::color::color_from_bytes(unsigned char, unsigned
    // char, unsigned char, unsigned char)(unsigned char, unsigned char, unsigned char, unsigned
    // char) file:genesis/util/color/function.hpp line:57
    M( "genesis::util::color" )
        .def(
            "color_from_bytes",
            []( unsigned char const& a0, unsigned char const& a1, unsigned char const& a2
            ) -> genesis::util::color::Color {
                return genesis::util::color::color_from_bytes( a0, a1, a2 );
            },
            "",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" )
        );
    M( "genesis::util::color" )
        .def(
            "color_from_bytes",
            ( class genesis::util::color::Color( * )(
                unsigned char, unsigned char, unsigned char, unsigned char
            ) ) &
                genesis::util::color::color_from_bytes,
            "Create a Color given three or four values in the range `[ 0, 255 ]` for each\n of the "
            "components red, green and blue, and optionally alpha.\n\nC++: "
            "genesis::util::color::color_from_bytes(unsigned char, unsigned char, unsigned char, "
            "unsigned char) --> class genesis::util::color::Color",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" ),
            pybind11::arg( "a" )
        );

    // genesis::util::color::color_from_hex(const std::string &, const std::string &)
    // file:genesis/util/color/function.hpp line:66 function-signature: class
    // genesis::util::color::Color genesis::util::color::color_from_hex(const std::string &, const
    // std::string &)(const std::string &, const std::string &) file:genesis/util/color/function.hpp
    // line:66
    M( "genesis::util::color" )
        .def(
            "color_from_hex",
            []( const std::string& a0 ) -> genesis::util::color::Color {
                return genesis::util::color::color_from_hex( a0 );
            },
            "",
            pybind11::arg( "hex_color" )
        );
    M( "genesis::util::color" )
        .def(
            "color_from_hex",
            ( class genesis::util::color::Color( * )( const std::string&, const std::string& ) ) &
                genesis::util::color::color_from_hex,
            "Create a Color given a hex color string in the format \"#003366[ff]\".\n\n The hash "
            "sign in the beginning can be replaced by any given prefix.\n If the string is not "
            "correctly formatted, an std::invalid_argument exception is thrown.\n If the string "
            "contains only RGB, alpha is set to `1.0`.\n\nC++: "
            "genesis::util::color::color_from_hex(const std::string &, const std::string &) --> "
            "class genesis::util::color::Color",
            pybind11::arg( "hex_color" ),
            pybind11::arg( "prefix" )
        );

    // genesis::util::color::color_to_hex(const class genesis::util::color::Color &, const
    // std::string &, bool, bool) file:genesis/util/color/function.hpp line:76 function-signature:
    // std::string genesis::util::color::color_to_hex(const class genesis::util::color::Color &,
    // const std::string &, bool, bool)(const class genesis::util::color::Color &, const std::string
    // &, bool, bool) file:genesis/util/color/function.hpp line:76
    M( "genesis::util::color" )
        .def(
            "color_to_hex",
            []( const class genesis::util::color::Color& a0 ) -> std::string {
                return genesis::util::color::color_to_hex( a0 );
            },
            "",
            pybind11::arg( "c" )
        );
    M( "genesis::util::color" )
        .def(
            "color_to_hex",
            []( const class genesis::util::color::Color& a0, const std::string& a1
            ) -> std::string { return genesis::util::color::color_to_hex( a0, a1 ); },
            "",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" )
        );
    M( "genesis::util::color" )
        .def(
            "color_to_hex",
            []( const class genesis::util::color::Color& a0, const std::string& a1, bool const& a2
            ) -> std::string { return genesis::util::color::color_to_hex( a0, a1, a2 ); },
            "",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "uppercase" )
        );
    M( "genesis::util::color" )
        .def(
            "color_to_hex",
            ( std::string( * )(
                const class genesis::util::color::Color&, const std::string&, bool, bool
            ) ) &
                genesis::util::color::color_to_hex,
            "Return a hex string representation of a Color in the format \"#003366[ff]\".\n\n The "
            "hash sign in the beginning can be replaced by any given \n If  is set to `true`, any "
            "outputted alphabetical chars\n (that is, A to F for hex strings) are uppercase.\n if  "
            "is set to `true`, two additional hex digits are printed for the alpha "
            "channel.\n\nC++: genesis::util::color::color_to_hex(const class "
            "genesis::util::color::Color &, const std::string &, bool, bool) --> std::string",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "uppercase" ),
            pybind11::arg( "with_alpha" )
        );

    // genesis::util::color::resolve_color_string(const std::string &)
    // file:genesis/util/color/function.hpp line:98 function-signature: class
    // genesis::util::color::Color genesis::util::color::resolve_color_string(const std::string
    // &)(const std::string &) file:genesis/util/color/function.hpp line:98
    M( "genesis::util::color" )
        .def(
            "resolve_color_string",
            ( class genesis::util::color::Color( * )( const std::string& ) ) &
                genesis::util::color::resolve_color_string,
            "Resolve a string representing a color.\n\n The string can either be a hex color as "
            "accepted by color_from_hex(), e.g., \"#003366[ff]\",\n or one of the named colors, "
            "see is_color_name() and color_from_name().\n\nC++: "
            "genesis::util::color::resolve_color_string(const std::string &) --> class "
            "genesis::util::color::Color",
            pybind11::arg( "color_str" )
        );

    // genesis::util::color::interpolate(const class genesis::util::color::Color &, const class
    // genesis::util::color::Color &, double) file:genesis/util/color/function.hpp line:107
    // function-signature: class genesis::util::color::Color genesis::util::color::interpolate(const
    // class genesis::util::color::Color &, const class genesis::util::color::Color &, double)(const
    // class genesis::util::color::Color &, const class genesis::util::color::Color &, double)
    // file:genesis/util/color/function.hpp line:107
    M( "genesis::util::color" )
        .def(
            "interpolate",
            ( class genesis::util::color::Color( * )(
                const class genesis::util::color::Color&,
                const class genesis::util::color::Color&,
                double
            ) ) &
                genesis::util::color::interpolate,
            "Linearily interpolate between two Color%s.\n\nC++: "
            "genesis::util::color::interpolate(const class genesis::util::color::Color &, const "
            "class genesis::util::color::Color &, double) --> class genesis::util::color::Color",
            pybind11::arg( "color1" ),
            pybind11::arg( "color2" ),
            pybind11::arg( "fraction" )
        );

    // genesis::util::color::gradient(const class std::map<double, class
    // genesis::util::color::Color> &, double) file:genesis/util/color/function.hpp line:140
    // function-signature: class genesis::util::color::Color genesis::util::color::gradient(const
    // class std::map<double, class genesis::util::color::Color> &, double)(const class
    // std::map<double, class genesis::util::color::Color> &, double)
    // file:genesis/util/color/function.hpp line:140
    M( "genesis::util::color" )
        .def(
            "gradient",
            ( class genesis::util::color::Color( * )(
                const class std::map<double, class genesis::util::color::Color>&, double
            ) ) &
                genesis::util::color::gradient,
            "Returns a Color that is created using a color gradient.\n\n The function takes a "
            "color gradient in form of a map from double to Color.\n The keys in the map are "
            "offset positions (stops) with specific Color%s assigned to them.\n An arbitrary "
            "number of keys in between these boundaries is allowed.\n\n Each key is associated "
            "with a Color value, which indicates the color at that position.\n\n Example:\n\n     "
            "// Prepare gradient ranges.\n     auto ranges = std::map<double, Color>();\n     "
            "ranges[ 0.0 ] = Color( 0.0, 1.0, 0.0 );\n     ranges[ 0.5 ] = Color( 1.0, 1.0, 0.0 "
            ");\n     ranges[ 1.0 ] = Color( 1.0, 0.0, 0.0 );\n\n     // Get color at 30% of the "
            "green-yellow-red gradient.\n     Color c = gradient( ranges, 0.3 );\n\n This map "
            "defines the same gradient that is used for `heat_gradient`. The range boundaries\n do "
            "not have to be `[ 0.0, 1.0 ]`. They can be set to any range that is needed to reflect "
            "the\n range of possible values.\n\n The second parameter of this function then takes "
            "the position at which the gradient is evaluated.\n Values outside of the interval "
            "that is defined by the range are set to the closest interval\n border value.\n\n For "
            "example, given the range map above, a value of 0.0 gives green; 0.5 gives yellow;\n "
            "1.0 gives red; values in between are interpolated linearily.\n\nC++: "
            "genesis::util::color::gradient(const class std::map<double, class "
            "genesis::util::color::Color> &, double) --> class genesis::util::color::Color",
            pybind11::arg( "ranges" ),
            pybind11::arg( "value" )
        );

    // genesis::util::color::heat_gradient(double) file:genesis/util/color/function.hpp line:149
    // function-signature: class genesis::util::color::Color
    // genesis::util::color::heat_gradient(double)(double) file:genesis/util/color/function.hpp
    // line:149
    M( "genesis::util::color" )
        .def(
            "heat_gradient",
            ( class genesis::util::color::Color( * )( double ) ) &
                genesis::util::color::heat_gradient,
            "Return a Color that represents a heat gradient for a given percentage value.\n\n "
            "Given a percentage value in the range of `[ 0.0, 1.0] `, the functions represents the "
            "heat of\n that value. For 0.0, the heat gradient value is green, for 0.5 yellow and "
            "for 1.0 red.\n The values in between are interpolated linearily.\n\nC++: "
            "genesis::util::color::heat_gradient(double) --> class genesis::util::color::Color",
            pybind11::arg( "percentage" )
        );

    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalization &) file:genesis/util/color/helper.hpp line:60
    // function-signature: class std::map<double, class genesis::util::color::Color>
    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalization &)(const class genesis::util::color::ColorMap &,
    // const class genesis::util::color::ColorNormalization &) file:genesis/util/color/helper.hpp
    // line:60
    M( "genesis::util::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::util::color::
                    Color>( * )( const class genesis::util::color::ColorMap&, const class genesis::util::color::ColorNormalization& )
            ) & genesis::util::color::color_stops,
            "C++: genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, "
            "const class genesis::util::color::ColorNormalization &) --> class std::map<double, "
            "class genesis::util::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationLinear &) file:genesis/util/color/helper.hpp line:61
    // function-signature: class std::map<double, class genesis::util::color::Color>
    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationLinear &)(const class genesis::util::color::ColorMap
    // &, const class genesis::util::color::ColorNormalizationLinear &)
    // file:genesis/util/color/helper.hpp line:61
    M( "genesis::util::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::util::color::
                    Color>( * )( const class genesis::util::color::ColorMap&, const class genesis::util::color::ColorNormalizationLinear& )
            ) & genesis::util::color::color_stops,
            "C++: genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, "
            "const class genesis::util::color::ColorNormalizationLinear &) --> class "
            "std::map<double, class genesis::util::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationLogarithmic &) file:genesis/util/color/helper.hpp
    // line:62 function-signature: class std::map<double, class genesis::util::color::Color>
    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationLogarithmic &)(const class
    // genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationLogarithmic &) file:genesis/util/color/helper.hpp
    // line:62
    M( "genesis::util::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::util::color::
                    Color>( * )( const class genesis::util::color::ColorMap&, const class genesis::util::color::ColorNormalizationLogarithmic& )
            ) & genesis::util::color::color_stops,
            "C++: genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, "
            "const class genesis::util::color::ColorNormalizationLogarithmic &) --> class "
            "std::map<double, class genesis::util::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationDiverging &) file:genesis/util/color/helper.hpp
    // line:63 function-signature: class std::map<double, class genesis::util::color::Color>
    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationDiverging &)(const class
    // genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationDiverging &) file:genesis/util/color/helper.hpp
    // line:63
    M( "genesis::util::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::util::color::
                    Color>( * )( const class genesis::util::color::ColorMap&, const class genesis::util::color::ColorNormalizationDiverging& )
            ) & genesis::util::color::color_stops,
            "C++: genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, "
            "const class genesis::util::color::ColorNormalizationDiverging &) --> class "
            "std::map<double, class genesis::util::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationBoundary &) file:genesis/util/color/helper.hpp
    // line:64 function-signature: class std::map<double, class genesis::util::color::Color>
    // genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationBoundary &)(const class
    // genesis::util::color::ColorMap &, const class
    // genesis::util::color::ColorNormalizationBoundary &) file:genesis/util/color/helper.hpp
    // line:64
    M( "genesis::util::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::util::color::
                    Color>( * )( const class genesis::util::color::ColorMap&, const class genesis::util::color::ColorNormalizationBoundary& )
            ) & genesis::util::color::color_stops,
            "C++: genesis::util::color::color_stops(const class genesis::util::color::ColorMap &, "
            "const class genesis::util::color::ColorNormalizationBoundary &) --> class "
            "std::map<double, class genesis::util::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::util::color::color_tickmarks(const class genesis::util::color::ColorNormalization &,
    // unsigned long) file:genesis/util/color/helper.hpp line:70 function-signature: class
    // std::map<double, std::string > genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalization &, unsigned long)(const class
    // genesis::util::color::ColorNormalization &, unsigned long) file:genesis/util/color/helper.hpp
    // line:70
    M( "genesis::util::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::util::color::ColorNormalization&, unsigned long
            ) ) &
                genesis::util::color::color_tickmarks,
            "C++: genesis::util::color::color_tickmarks(const class "
            "genesis::util::color::ColorNormalization &, unsigned long) --> class std::map<double, "
            "std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );

    // genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationLinear &, unsigned long)
    // file:genesis/util/color/helper.hpp line:71 function-signature: class std::map<double,
    // std::string > genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationLinear &, unsigned long)(const class
    // genesis::util::color::ColorNormalizationLinear &, unsigned long)
    // file:genesis/util/color/helper.hpp line:71
    M( "genesis::util::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::util::color::ColorNormalizationLinear&, unsigned long
            ) ) &
                genesis::util::color::color_tickmarks,
            "C++: genesis::util::color::color_tickmarks(const class "
            "genesis::util::color::ColorNormalizationLinear &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );

    // genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationLogarithmic &, unsigned long)
    // file:genesis/util/color/helper.hpp line:72 function-signature: class std::map<double,
    // std::string > genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationLogarithmic &, unsigned long)(const class
    // genesis::util::color::ColorNormalizationLogarithmic &, unsigned long)
    // file:genesis/util/color/helper.hpp line:72
    M( "genesis::util::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::util::color::ColorNormalizationLogarithmic&, unsigned long
            ) ) &
                genesis::util::color::color_tickmarks,
            "C++: genesis::util::color::color_tickmarks(const class "
            "genesis::util::color::ColorNormalizationLogarithmic &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );
}
