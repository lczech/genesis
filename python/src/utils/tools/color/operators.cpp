/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_tools_color_operators_export, "utils")
{

    boost::python::def(
        "color_from_doubles",
        ( Color ( * )( double, double, double ))( &::genesis::utils::color_from_doubles ),
        ( boost::python::arg("r"), boost::python::arg("g"), boost::python::arg("b") ),
        get_docstring("Color ::genesis::utils::color_from_doubles (double r, double g, double b)")
    );

    boost::python::def(
        "color_from_hex",
        ( Color ( * )( std::string, std::string ))( &::genesis::utils::color_from_hex ),
        ( boost::python::arg("h"), boost::python::arg("prefix") ),
        get_docstring("Color ::genesis::utils::color_from_hex (std::string h, std::string prefix)")
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, const Color & ))( &::genesis::utils::operator<< ),
        ( boost::python::arg("os"), boost::python::arg("color") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("std::ostream & ::genesis::utils::operator<< (std::ostream & os, const Color & color)")
    );

    boost::python::def(
        "color_to_hex",
        ( std::string ( * )( const Color &, std::string, bool ))( &::genesis::utils::color_to_hex ),
        ( boost::python::arg("c"), boost::python::arg("prefix"), boost::python::arg("uppercase") ),
        get_docstring("std::string ::genesis::utils::color_to_hex (const Color & c, std::string prefix, bool uppercase)")
    );
}
