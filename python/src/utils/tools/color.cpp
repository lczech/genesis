/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::Color, scope )
{

    // -------------------------------------------------------------------
    //     Class Color
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Color, std::shared_ptr<::genesis::utils::Color> > ( scope, "Color" )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::Color::Color ()")
        )
        .def(
            pybind11::init< double, double, double >(),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b"),
            get_docstring("::genesis::utils::Color::Color (double r, double g, double b)")
        )
        .def(
            pybind11::init< double, double, double, double >(),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b"),
            pybind11::arg("a"),
            get_docstring("::genesis::utils::Color::Color (double r, double g, double b, double a)")
        )
        .def(
            pybind11::init< Color const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "a",
            ( double ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::a )
        )
        .def(
            "a",
            ( void ( ::genesis::utils::Color::* )( double ))( &::genesis::utils::Color::a ),
            pybind11::arg("value")
        )
        .def(
            "a_byte",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::a_byte )
        )
        .def(
            "a_byte",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::a_byte ),
            pybind11::arg("value")
        )
        .def(
            "b",
            ( double ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::b )
        )
        .def(
            "b",
            ( void ( ::genesis::utils::Color::* )( double ))( &::genesis::utils::Color::b ),
            pybind11::arg("value")
        )
        .def(
            "b_byte",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::b_byte )
        )
        .def(
            "b_byte",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::b_byte ),
            pybind11::arg("value")
        )
        .def(
            "g",
            ( double ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::g )
        )
        .def(
            "g",
            ( void ( ::genesis::utils::Color::* )( double ))( &::genesis::utils::Color::g ),
            pybind11::arg("value")
        )
        .def(
            "g_byte",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::g_byte )
        )
        .def(
            "g_byte",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::g_byte ),
            pybind11::arg("value")
        )
        .def(
            "r",
            ( double ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::r )
        )
        .def(
            "r",
            ( void ( ::genesis::utils::Color::* )( double ))( &::genesis::utils::Color::r ),
            pybind11::arg("value")
        )
        .def(
            "r_byte",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::r_byte )
        )
        .def(
            "r_byte",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::r_byte ),
            pybind11::arg("value")
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Color::* )( Color & ))( &::genesis::utils::Color::swap ),
            pybind11::arg("other")
        )
        .def_static(
            "from_bytes",
            ( Color ( * )( unsigned char, unsigned char, unsigned char, unsigned char ))( &::genesis::utils::Color::from_bytes ),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b"),
            pybind11::arg("a")=(unsigned char)(255)
        )
        .def_static(
            "from_hex",
            ( Color ( * )( std::string const &, std::string const & ))( &::genesis::utils::Color::from_hex ),
            pybind11::arg("hex_color"),
            pybind11::arg("prefix")=(std::string const &)("#")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::Color const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_tools_color_export, ::genesis::utils, scope )
{

    scope.def(
        "operator!=",
        ( bool ( * )( Color const &, Color const & ))( &::genesis::utils::operator!= ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "operator==",
        ( bool ( * )( Color const &, Color const & ))( &::genesis::utils::operator== ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );

    scope.def(
        "swap",
        ( void ( * )( Color &, Color & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
