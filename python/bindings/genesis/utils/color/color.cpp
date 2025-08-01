#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/functions.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

void bind_genesis_utils_color_color(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::color::Color file:genesis/utils/color/color.hpp line:48
        pybind11::
            class_<genesis::utils::color::Color, std::shared_ptr<genesis::utils::color::Color>>
                cl( M( "genesis::utils::color" ), "Color", "" );
        // function-signature: genesis::utils::color::Color::Color()()
        // file:genesis/utils/color/color.hpp line:59
        cl.def( pybind11::init( []() { return new genesis::utils::color::Color(); } ) );
        // function-signature: genesis::utils::color::Color::Color(double, double, double)(double,
        // double, double) file:genesis/utils/color/color.hpp line:66
        cl.def(
            pybind11::init<double, double, double>(),
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" )
        );

        // function-signature: genesis::utils::color::Color::Color(double, double, double,
        // double)(double, double, double, double) file:genesis/utils/color/color.hpp line:73
        cl.def(
            pybind11::init<double, double, double, double>(),
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" ),
            pybind11::arg( "a" )
        );

        // function-signature: genesis::utils::color::Color::Color(const class
        // genesis::utils::color::Color &)(const class genesis::utils::color::Color &)
        // file:genesis/utils/color/color.hpp line:77
        cl.def( pybind11::init( []( genesis::utils::color::Color const& o ) {
            return new genesis::utils::color::Color( o );
        } ) );
        // function-signature: class genesis::utils::color::Color &
        // genesis::utils::color::Color::operator=(const class genesis::utils::color::Color &)(const
        // class genesis::utils::color::Color &) file:genesis/utils/color/color.hpp line:80
        cl.def(
            "assign",
            ( class genesis::utils::color::Color &
              (genesis::utils::color::Color::*)(const class genesis::utils::color::Color&)) &
                genesis::utils::color::Color::operator=,
            "C++: genesis::utils::color::Color::operator=(const class genesis::utils::color::Color "
            "&) --> class genesis::utils::color::Color &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: void genesis::utils::color::Color::swap(class
        // genesis::utils::color::Color &)(class genesis::utils::color::Color &)
        // file:genesis/utils/color/color.hpp line:83
        cl.def(
            "swap",
            ( void( genesis::utils::color::Color::* )( class genesis::utils::color::Color& ) ) &
                genesis::utils::color::Color::swap,
            "C++: genesis::utils::color::Color::swap(class genesis::utils::color::Color &) --> "
            "void",
            pybind11::arg( "other" )
        );
        // function-signature: class genesis::utils::color::Color
        // genesis::utils::color::Color::from_bytes(unsigned char, unsigned char, unsigned char,
        // unsigned char)(unsigned char, unsigned char, unsigned char, unsigned char)
        // file:genesis/utils/color/color.hpp line:99
        cl.def_static(
            "from_bytes",
            []( unsigned char const& a0, unsigned char const& a1, unsigned char const& a2
            ) -> genesis::utils::color::Color {
                return genesis::utils::color::Color::from_bytes( a0, a1, a2 );
            },
            "",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" )
        );
        cl.def_static(
            "from_bytes",
            ( class genesis::utils::color::Color( * )(
                unsigned char, unsigned char, unsigned char, unsigned char
            ) ) &
                genesis::utils::color::Color::from_bytes,
            "C++: genesis::utils::color::Color::from_bytes(unsigned char, unsigned char, unsigned "
            "char, unsigned char) --> class genesis::utils::color::Color",
            pybind11::arg( "r" ),
            pybind11::arg( "g" ),
            pybind11::arg( "b" ),
            pybind11::arg( "a" )
        );
        // function-signature: class genesis::utils::color::Color
        // genesis::utils::color::Color::from_hex(const std::string &, const std::string &)(const
        // std::string &, const std::string &) file:genesis/utils/color/color.hpp line:104
        cl.def_static(
            "from_hex",
            []( const std::string& a0 ) -> genesis::utils::color::Color {
                return genesis::utils::color::Color::from_hex( a0 );
            },
            "",
            pybind11::arg( "hex_color" )
        );
        cl.def_static(
            "from_hex",
            ( class genesis::utils::color::Color( * )( const std::string&, const std::string& ) ) &
                genesis::utils::color::Color::from_hex,
            "C++: genesis::utils::color::Color::from_hex(const std::string &, const std::string &) "
            "--> class genesis::utils::color::Color",
            pybind11::arg( "hex_color" ),
            pybind11::arg( "prefix" )
        );
        // function-signature: double genesis::utils::color::Color::r() const()
        // file:genesis/utils/color/color.hpp line:110
        cl.def(
            "r",
            ( double( genesis::utils::color::Color::* )() const ) & genesis::utils::color::Color::r,
            "C++: genesis::utils::color::Color::r() const --> double"
        );
        // function-signature: double genesis::utils::color::Color::g() const()
        // file:genesis/utils/color/color.hpp line:115
        cl.def(
            "g",
            ( double( genesis::utils::color::Color::* )() const ) & genesis::utils::color::Color::g,
            "C++: genesis::utils::color::Color::g() const --> double"
        );
        // function-signature: double genesis::utils::color::Color::b() const()
        // file:genesis/utils/color/color.hpp line:120
        cl.def(
            "b",
            ( double( genesis::utils::color::Color::* )() const ) & genesis::utils::color::Color::b,
            "C++: genesis::utils::color::Color::b() const --> double"
        );
        // function-signature: double genesis::utils::color::Color::a() const()
        // file:genesis/utils/color/color.hpp line:125
        cl.def(
            "a",
            ( double( genesis::utils::color::Color::* )() const ) & genesis::utils::color::Color::a,
            "C++: genesis::utils::color::Color::a() const --> double"
        );
        // function-signature: unsigned char genesis::utils::color::Color::r_byte() const()
        // file:genesis/utils/color/color.hpp line:130
        cl.def(
            "r_byte",
            ( unsigned char ( genesis::utils::color::Color::* )() const ) &
                genesis::utils::color::Color::r_byte,
            "C++: genesis::utils::color::Color::r_byte() const --> unsigned char"
        );
        // function-signature: unsigned char genesis::utils::color::Color::g_byte() const()
        // file:genesis/utils/color/color.hpp line:135
        cl.def(
            "g_byte",
            ( unsigned char ( genesis::utils::color::Color::* )() const ) &
                genesis::utils::color::Color::g_byte,
            "C++: genesis::utils::color::Color::g_byte() const --> unsigned char"
        );
        // function-signature: unsigned char genesis::utils::color::Color::b_byte() const()
        // file:genesis/utils/color/color.hpp line:140
        cl.def(
            "b_byte",
            ( unsigned char ( genesis::utils::color::Color::* )() const ) &
                genesis::utils::color::Color::b_byte,
            "C++: genesis::utils::color::Color::b_byte() const --> unsigned char"
        );
        // function-signature: unsigned char genesis::utils::color::Color::a_byte() const()
        // file:genesis/utils/color/color.hpp line:145
        cl.def(
            "a_byte",
            ( unsigned char ( genesis::utils::color::Color::* )() const ) &
                genesis::utils::color::Color::a_byte,
            "C++: genesis::utils::color::Color::a_byte() const --> unsigned char"
        );
        // function-signature: void genesis::utils::color::Color::r(double)(double)
        // file:genesis/utils/color/color.hpp line:154
        cl.def(
            "r",
            ( void( genesis::utils::color::Color::* )( double ) ) & genesis::utils::color::Color::r,
            "C++: genesis::utils::color::Color::r(double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::g(double)(double)
        // file:genesis/utils/color/color.hpp line:156
        cl.def(
            "g",
            ( void( genesis::utils::color::Color::* )( double ) ) & genesis::utils::color::Color::g,
            "C++: genesis::utils::color::Color::g(double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::b(double)(double)
        // file:genesis/utils/color/color.hpp line:158
        cl.def(
            "b",
            ( void( genesis::utils::color::Color::* )( double ) ) & genesis::utils::color::Color::b,
            "C++: genesis::utils::color::Color::b(double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::a(double)(double)
        // file:genesis/utils/color/color.hpp line:160
        cl.def(
            "a",
            ( void( genesis::utils::color::Color::* )( double ) ) & genesis::utils::color::Color::a,
            "C++: genesis::utils::color::Color::a(double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::r_byte(unsigned char)(unsigned
        // char) file:genesis/utils/color/color.hpp line:162
        cl.def(
            "r_byte",
            ( void( genesis::utils::color::Color::* )( unsigned char ) ) &
                genesis::utils::color::Color::r_byte,
            "C++: genesis::utils::color::Color::r_byte(unsigned char) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::g_byte(unsigned char)(unsigned
        // char) file:genesis/utils/color/color.hpp line:167
        cl.def(
            "g_byte",
            ( void( genesis::utils::color::Color::* )( unsigned char ) ) &
                genesis::utils::color::Color::g_byte,
            "C++: genesis::utils::color::Color::g_byte(unsigned char) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::b_byte(unsigned char)(unsigned
        // char) file:genesis/utils/color/color.hpp line:172
        cl.def(
            "b_byte",
            ( void( genesis::utils::color::Color::* )( unsigned char ) ) &
                genesis::utils::color::Color::b_byte,
            "C++: genesis::utils::color::Color::b_byte(unsigned char) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::color::Color::a_byte(unsigned char)(unsigned
        // char) file:genesis/utils/color/color.hpp line:177
        cl.def(
            "a_byte",
            ( void( genesis::utils::color::Color::* )( unsigned char ) ) &
                genesis::utils::color::Color::a_byte,
            "C++: genesis::utils::color::Color::a_byte(unsigned char) --> void",
            pybind11::arg( "value" )
        );

        cl.def( "__str__", []( genesis::utils::color::Color const& o ) -> std::string {
            std::ostringstream s;
            using namespace genesis::utils::color;
            s << o;
            return s.str();
        } );
    }
    // genesis::utils::color::swap(class genesis::utils::color::Color &, class
    // genesis::utils::color::Color &) file:genesis/utils/color/color.hpp line:208
    // function-signature: void genesis::utils::color::swap(class genesis::utils::color::Color &,
    // class genesis::utils::color::Color &)(class genesis::utils::color::Color &, class
    // genesis::utils::color::Color &) file:genesis/utils/color/color.hpp line:208
    M( "genesis::utils::color" )
        .def(
            "swap",
            ( void ( * )( class genesis::utils::color::Color&, class genesis::utils::color::Color& )
            ) & genesis::utils::color::swap,
            "C++: genesis::utils::color::swap(class genesis::utils::color::Color &, class "
            "genesis::utils::color::Color &) --> void",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );
}
