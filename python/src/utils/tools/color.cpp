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
            pybind11::init< unsigned char, unsigned char, unsigned char >(),
            pybind11::arg("r"),
            pybind11::arg("g"),
            pybind11::arg("b"),
            get_docstring("::genesis::utils::Color::Color (unsigned char r, unsigned char g, unsigned char b)")
        )
        .def(
            pybind11::init< Color const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "b",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::b )
        )
        .def(
            "b",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::b ),
            pybind11::arg("value")
        )
        .def(
            "g",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::g )
        )
        .def(
            "g",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::g ),
            pybind11::arg("value")
        )
        .def(
            "r",
            ( unsigned char ( ::genesis::utils::Color::* )(  ) const )( &::genesis::utils::Color::r )
        )
        .def(
            "r",
            ( void ( ::genesis::utils::Color::* )( unsigned char ))( &::genesis::utils::Color::r ),
            pybind11::arg("value")
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Color::* )( Color & ))( &::genesis::utils::Color::swap ),
            pybind11::arg("other")
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
