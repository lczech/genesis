/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS( ::genesis::utils::Style, scope )
{

    // -------------------------------------------------------------------
    //     Class Style
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::utils::Style, std::shared_ptr<::genesis::utils::Style> > ( scope, "Style" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::string const & >(),
            pybind11::arg("foreground_color")
        )
        .def(
            pybind11::init< const char * >(),
            pybind11::arg("foreground_color")
        )
        .def(
            pybind11::init< bool >(),
            pybind11::arg("bold")
        )
        .def(
            pybind11::init< std::string const &, bool >(),
            pybind11::arg("foreground_color"),
            pybind11::arg("bold")
        )
        .def(
            pybind11::init< std::string const &, std::string const & >(),
            pybind11::arg("foreground_color"),
            pybind11::arg("background_color")
        )
        .def(
            pybind11::init< std::string const &, const char * >(),
            pybind11::arg("foreground_color"),
            pybind11::arg("background_color")
        )
        .def(
            pybind11::init< std::string const &, std::string const &, bool >(),
            pybind11::arg("foreground_color"),
            pybind11::arg("background_color"),
            pybind11::arg("bold")
        )
        .def(
            pybind11::init< Style const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "background_color",
            ( Style & ( ::genesis::utils::Style::* )( std::string const & ))( &::genesis::utils::Style::background_color ),
            pybind11::arg("color")
        )
        .def(
            "background_color",
            ( std::string ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::background_color )
        )
        .def(
            "bold",
            ( Style & ( ::genesis::utils::Style::* )( bool ))( &::genesis::utils::Style::bold ),
            pybind11::arg("value")
        )
        .def(
            "bold",
            ( bool ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::bold )
        )
        .def(
            "enabled",
            ( Style & ( ::genesis::utils::Style::* )( bool ))( &::genesis::utils::Style::enabled ),
            pybind11::arg("value")
        )
        .def(
            "enabled",
            ( bool ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::enabled )
        )
        .def(
            "foreground_color",
            ( Style & ( ::genesis::utils::Style::* )( std::string const & ))( &::genesis::utils::Style::foreground_color ),
            pybind11::arg("color")
        )
        .def(
            "foreground_color",
            ( std::string ( ::genesis::utils::Style::* )(  ) const )( &::genesis::utils::Style::foreground_color )
        )
        .def(
            "reset",
            ( Style & ( ::genesis::utils::Style::* )(  ))( &::genesis::utils::Style::reset )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Style::* )( Style & ))( &::genesis::utils::Style::swap ),
            pybind11::arg("other")
        )
        .def(
            "to_bash_string",
            ( std::string ( ::genesis::utils::Style::* )( std::string const & ) const )( &::genesis::utils::Style::to_bash_string ),
            pybind11::arg("text")
        )
        .def(
            "to_python_string",
            ( std::string ( ::genesis::utils::Style::* )( std::string const & ) const )( &::genesis::utils::Style::to_python_string ),
            pybind11::arg("text")
        )
        .def_static(
            "get_background_color_value",
            ( std::string ( * )( std::string ))( &::genesis::utils::Style::get_background_color_value ),
            pybind11::arg("name")
        )
        .def_static(
            "get_foreground_color_value",
            ( std::string ( * )( std::string ))( &::genesis::utils::Style::get_foreground_color_value ),
            pybind11::arg("name")
        )
        .def_static(
            "is_background_color",
            ( bool ( * )( std::string ))( &::genesis::utils::Style::is_background_color ),
            pybind11::arg("name")
        )
        .def_static(
            "is_foreground_color",
            ( bool ( * )( std::string ))( &::genesis::utils::Style::is_foreground_color ),
            pybind11::arg("name")
        )

        // Operators

        .def(
            "__str__",
            []( ::genesis::utils::Style const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS( utils_text_style_export, ::genesis::utils, scope )
{

    scope.def(
        "swap",
        ( void ( * )( Style &, Style & ))( &::genesis::utils::swap ),
            pybind11::arg("lhs"),
            pybind11::arg("rhs")
    );
}
