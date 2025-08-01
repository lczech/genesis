#include <functional>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/function.hpp>
#include <genesis/utils/color/helper.hpp>
#include <genesis/utils/color/map.hpp>
#include <genesis/utils/color/norm_boundary.hpp>
#include <genesis/utils/color/norm_diverging.hpp>
#include <genesis/utils/color/norm_linear.hpp>
#include <genesis/utils/color/norm_logarithmic.hpp>
#include <genesis/utils/color/normalization.hpp>
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

void bind_genesis_utils_color_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::utils::color::color_from_bytes(unsigned char, unsigned char, unsigned char, unsigned
    // char) file:genesis/utils/color/function.hpp line:57 function-signature: class
    // genesis::utils::color::Color genesis::utils::color::color_from_bytes(unsigned char, unsigned
    // char, unsigned char, unsigned char)(unsigned char, unsigned char, unsigned char, unsigned
    // char) file:genesis/utils/color/function.hpp line:57
    M( "genesis::utils::color" )
        .def(
            "color_from_bytes",
            []( unsigned char const& a0, unsigned char const& a1, unsigned char const& a2
            ) -> genesis::utils::color::Color {
                return genesis::utils::color::color_from_bytes( a0, a1, a2 );
            },
            "",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" )
        );
    M( "genesis::utils::color" )
        .def(
            "color_from_bytes",
            ( class genesis::utils::color::Color( * )(
                unsigned char, unsigned char, unsigned char, unsigned char
            ) ) &
                genesis::utils::color::color_from_bytes,
            "Create a Color given three or four values in the range `[ 0, 255 ]` for each\n of the "
            "components red, green and blue, and optionally alpha.\n\nC++: "
            "genesis::utils::color::color_from_bytes(unsigned char, unsigned char, unsigned char, "
            "unsigned char) --> class genesis::utils::color::Color",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" ),
            pybind11::arg( "a" )
        );

    // genesis::utils::color::color_from_hex(const std::string &, const std::string &)
    // file:genesis/utils/color/function.hpp line:66 function-signature: class
    // genesis::utils::color::Color genesis::utils::color::color_from_hex(const std::string &, const
    // std::string &)(const std::string &, const std::string &)
    // file:genesis/utils/color/function.hpp line:66
    M( "genesis::utils::color" )
        .def(
            "color_from_hex",
            []( const std::string& a0 ) -> genesis::utils::color::Color {
                return genesis::utils::color::color_from_hex( a0 );
            },
            "",
            pybind11::arg( "hex_color" )
        );
    M( "genesis::utils::color" )
        .def(
            "color_from_hex",
            ( class genesis::utils::color::Color( * )( const std::string&, const std::string& ) ) &
                genesis::utils::color::color_from_hex,
            "Create a Color given a hex color string in the format \"#003366[ff]\".\n\n The hash "
            "sign in the beginning can be replaced by any given prefix.\n If the string is not "
            "correctly formatted, an std::invalid_argument exception is thrown.\n If the string "
            "contains only RGB, alpha is set to `1.0`.\n\nC++: "
            "genesis::utils::color::color_from_hex(const std::string &, const std::string &) --> "
            "class genesis::utils::color::Color",
            pybind11::arg( "hex_color" ),
            pybind11::arg( "prefix" )
        );

    // genesis::utils::color::color_to_hex(const class genesis::utils::color::Color &, const
    // std::string &, bool, bool) file:genesis/utils/color/function.hpp line:76 function-signature:
    // std::string genesis::utils::color::color_to_hex(const class genesis::utils::color::Color &,
    // const std::string &, bool, bool)(const class genesis::utils::color::Color &, const
    // std::string &, bool, bool) file:genesis/utils/color/function.hpp line:76
    M( "genesis::utils::color" )
        .def(
            "color_to_hex",
            []( const class genesis::utils::color::Color& a0 ) -> std::string {
                return genesis::utils::color::color_to_hex( a0 );
            },
            "",
            pybind11::arg( "c" )
        );
    M( "genesis::utils::color" )
        .def(
            "color_to_hex",
            []( const class genesis::utils::color::Color& a0, const std::string& a1
            ) -> std::string { return genesis::utils::color::color_to_hex( a0, a1 ); },
            "",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" )
        );
    M( "genesis::utils::color" )
        .def(
            "color_to_hex",
            []( const class genesis::utils::color::Color& a0, const std::string& a1, bool const& a2
            ) -> std::string { return genesis::utils::color::color_to_hex( a0, a1, a2 ); },
            "",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "uppercase" )
        );
    M( "genesis::utils::color" )
        .def(
            "color_to_hex",
            ( std::string( * )(
                const class genesis::utils::color::Color&, const std::string&, bool, bool
            ) ) &
                genesis::utils::color::color_to_hex,
            "Return a hex string representation of a Color in the format \"#003366[ff]\".\n\n The "
            "hash sign in the beginning can be replaced by any given \n If  is set to `true`, any "
            "outputted alphabetical chars\n (that is, A to F for hex strings) are uppercase.\n if  "
            "is set to `true`, two additional hex digits are printed for the alpha "
            "channel.\n\nC++: genesis::utils::color::color_to_hex(const class "
            "genesis::utils::color::Color &, const std::string &, bool, bool) --> std::string",
            pybind11::arg( "c" ),
            pybind11::arg( "prefix" ),
            pybind11::arg( "uppercase" ),
            pybind11::arg( "with_alpha" )
        );

    // genesis::utils::color::resolve_color_string(const std::string &)
    // file:genesis/utils/color/function.hpp line:98 function-signature: class
    // genesis::utils::color::Color genesis::utils::color::resolve_color_string(const std::string
    // &)(const std::string &) file:genesis/utils/color/function.hpp line:98
    M( "genesis::utils::color" )
        .def(
            "resolve_color_string",
            ( class genesis::utils::color::Color( * )( const std::string& ) ) &
                genesis::utils::color::resolve_color_string,
            "Resolve a string representing a color.\n\n The string can either be a hex color as "
            "accepted by color_from_hex(), e.g., \"#003366[ff]\",\n or one of the named colors, "
            "see is_color_name() and color_from_name().\n\nC++: "
            "genesis::utils::color::resolve_color_string(const std::string &) --> class "
            "genesis::utils::color::Color",
            pybind11::arg( "color_str" )
        );

    // genesis::utils::color::interpolate(const class genesis::utils::color::Color &, const class
    // genesis::utils::color::Color &, double) file:genesis/utils/color/function.hpp line:107
    // function-signature: class genesis::utils::color::Color
    // genesis::utils::color::interpolate(const class genesis::utils::color::Color &, const class
    // genesis::utils::color::Color &, double)(const class genesis::utils::color::Color &, const
    // class genesis::utils::color::Color &, double) file:genesis/utils/color/function.hpp line:107
    M( "genesis::utils::color" )
        .def(
            "interpolate",
            ( class genesis::utils::color::Color( * )(
                const class genesis::utils::color::Color&,
                const class genesis::utils::color::Color&,
                double
            ) ) &
                genesis::utils::color::interpolate,
            "Linearily interpolate between two Color%s.\n\nC++: "
            "genesis::utils::color::interpolate(const class genesis::utils::color::Color &, const "
            "class genesis::utils::color::Color &, double) --> class genesis::utils::color::Color",
            pybind11::arg( "color1" ),
            pybind11::arg( "color2" ),
            pybind11::arg( "fraction" )
        );

    // genesis::utils::color::gradient(const class std::map<double, class
    // genesis::utils::color::Color> &, double) file:genesis/utils/color/function.hpp line:140
    // function-signature: class genesis::utils::color::Color genesis::utils::color::gradient(const
    // class std::map<double, class genesis::utils::color::Color> &, double)(const class
    // std::map<double, class genesis::utils::color::Color> &, double)
    // file:genesis/utils/color/function.hpp line:140
    M( "genesis::utils::color" )
        .def(
            "gradient",
            ( class genesis::utils::color::Color( * )(
                const class std::map<double, class genesis::utils::color::Color>&, double
            ) ) &
                genesis::utils::color::gradient,
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
            "genesis::utils::color::gradient(const class std::map<double, class "
            "genesis::utils::color::Color> &, double) --> class genesis::utils::color::Color",
            pybind11::arg( "ranges" ),
            pybind11::arg( "value" )
        );

    // genesis::utils::color::heat_gradient(double) file:genesis/utils/color/function.hpp line:149
    // function-signature: class genesis::utils::color::Color
    // genesis::utils::color::heat_gradient(double)(double) file:genesis/utils/color/function.hpp
    // line:149
    M( "genesis::utils::color" )
        .def(
            "heat_gradient",
            ( class genesis::utils::color::Color( * )( double ) ) &
                genesis::utils::color::heat_gradient,
            "Return a Color that represents a heat gradient for a given percentage value.\n\n "
            "Given a percentage value in the range of `[ 0.0, 1.0] `, the functions represents the "
            "heat of\n that value. For 0.0, the heat gradient value is green, for 0.5 yellow and "
            "for 1.0 red.\n The values in between are interpolated linearily.\n\nC++: "
            "genesis::utils::color::heat_gradient(double) --> class genesis::utils::color::Color",
            pybind11::arg( "percentage" )
        );

    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalization &) file:genesis/utils/color/helper.hpp line:60
    // function-signature: class std::map<double, class genesis::utils::color::Color>
    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalization &)(const class genesis::utils::color::ColorMap &,
    // const class genesis::utils::color::ColorNormalization &) file:genesis/utils/color/helper.hpp
    // line:60
    M( "genesis::utils::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::utils::color::
                    Color>( * )( const class genesis::utils::color::ColorMap&, const class genesis::utils::color::ColorNormalization& )
            ) & genesis::utils::color::color_stops,
            "C++: genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap "
            "&, const class genesis::utils::color::ColorNormalization &) --> class "
            "std::map<double, class genesis::utils::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLinear &) file:genesis/utils/color/helper.hpp
    // line:61 function-signature: class std::map<double, class genesis::utils::color::Color>
    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLinear &)(const class
    // genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLinear &) file:genesis/utils/color/helper.hpp
    // line:61
    M( "genesis::utils::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::utils::color::
                    Color>( * )( const class genesis::utils::color::ColorMap&, const class genesis::utils::color::ColorNormalizationLinear& )
            ) & genesis::utils::color::color_stops,
            "C++: genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap "
            "&, const class genesis::utils::color::ColorNormalizationLinear &) --> class "
            "std::map<double, class genesis::utils::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLogarithmic &) file:genesis/utils/color/helper.hpp
    // line:62 function-signature: class std::map<double, class genesis::utils::color::Color>
    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLogarithmic &)(const class
    // genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationLogarithmic &) file:genesis/utils/color/helper.hpp
    // line:62
    M( "genesis::utils::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::utils::color::
                    Color>( * )( const class genesis::utils::color::ColorMap&, const class genesis::utils::color::ColorNormalizationLogarithmic& )
            ) & genesis::utils::color::color_stops,
            "C++: genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap "
            "&, const class genesis::utils::color::ColorNormalizationLogarithmic &) --> class "
            "std::map<double, class genesis::utils::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationDiverging &) file:genesis/utils/color/helper.hpp
    // line:63 function-signature: class std::map<double, class genesis::utils::color::Color>
    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationDiverging &)(const class
    // genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationDiverging &) file:genesis/utils/color/helper.hpp
    // line:63
    M( "genesis::utils::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::utils::color::
                    Color>( * )( const class genesis::utils::color::ColorMap&, const class genesis::utils::color::ColorNormalizationDiverging& )
            ) & genesis::utils::color::color_stops,
            "C++: genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap "
            "&, const class genesis::utils::color::ColorNormalizationDiverging &) --> class "
            "std::map<double, class genesis::utils::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationBoundary &) file:genesis/utils/color/helper.hpp
    // line:64 function-signature: class std::map<double, class genesis::utils::color::Color>
    // genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationBoundary &)(const class
    // genesis::utils::color::ColorMap &, const class
    // genesis::utils::color::ColorNormalizationBoundary &) file:genesis/utils/color/helper.hpp
    // line:64
    M( "genesis::utils::color" )
        .def(
            "color_stops",
            ( class std::map<
                double,
                class genesis::utils::color::
                    Color>( * )( const class genesis::utils::color::ColorMap&, const class genesis::utils::color::ColorNormalizationBoundary& )
            ) & genesis::utils::color::color_stops,
            "C++: genesis::utils::color::color_stops(const class genesis::utils::color::ColorMap "
            "&, const class genesis::utils::color::ColorNormalizationBoundary &) --> class "
            "std::map<double, class genesis::utils::color::Color>",
            pybind11::arg( "map" ),
            pybind11::arg( "norm" )
        );

    // genesis::utils::color::color_tickmarks(const class genesis::utils::color::ColorNormalization
    // &, unsigned long) file:genesis/utils/color/helper.hpp line:70 function-signature: class
    // std::map<double, std::string > genesis::utils::color::color_tickmarks(const class
    // genesis::utils::color::ColorNormalization &, unsigned long)(const class
    // genesis::utils::color::ColorNormalization &, unsigned long)
    // file:genesis/utils/color/helper.hpp line:70
    M( "genesis::utils::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::utils::color::ColorNormalization&, unsigned long
            ) ) &
                genesis::utils::color::color_tickmarks,
            "C++: genesis::utils::color::color_tickmarks(const class "
            "genesis::utils::color::ColorNormalization &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );

    // genesis::utils::color::color_tickmarks(const class
    // genesis::utils::color::ColorNormalizationLinear &, unsigned long)
    // file:genesis/utils/color/helper.hpp line:71 function-signature: class std::map<double,
    // std::string > genesis::utils::color::color_tickmarks(const class
    // genesis::utils::color::ColorNormalizationLinear &, unsigned long)(const class
    // genesis::utils::color::ColorNormalizationLinear &, unsigned long)
    // file:genesis/utils/color/helper.hpp line:71
    M( "genesis::utils::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::utils::color::ColorNormalizationLinear&, unsigned long
            ) ) &
                genesis::utils::color::color_tickmarks,
            "C++: genesis::utils::color::color_tickmarks(const class "
            "genesis::utils::color::ColorNormalizationLinear &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );
}
