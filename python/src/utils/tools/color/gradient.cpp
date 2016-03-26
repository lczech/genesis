/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_tools_color_gradient_export, "utils")
{

    boost::python::def(
        "gradient",
        ( Color ( * )( std::map< double, Color > const &, double ))( &::genesis::utils::gradient ),
        ( boost::python::arg("ranges"), boost::python::arg("percentage") ),
        get_docstring("Color ::genesis::utils::gradient (std::map< double, Color > const & ranges, double percentage)")
    );

    boost::python::def(
        "heat_gradient",
        ( Color ( * )( double ))( &::genesis::utils::heat_gradient ),
        ( boost::python::arg("percentage") ),
        get_docstring("Color ::genesis::utils::heat_gradient (double percentage)")
    );

    boost::python::def(
        "interpolate",
        ( Color ( * )( Color, Color, double ))( &::genesis::utils::interpolate ),
        ( boost::python::arg("color1"), boost::python::arg("color2"), boost::python::arg("fraction") ),
        get_docstring("Color ::genesis::utils::interpolate (Color color1, Color color2, double fraction)")
    );

    boost::python::def(
        "interpolate",
        ( unsigned char ( * )( unsigned char, unsigned char, double ))( &::genesis::utils::interpolate ),
        ( boost::python::arg("d1"), boost::python::arg("d2"), boost::python::arg("fraction") ),
        get_docstring("unsigned char ::genesis::utils::interpolate (unsigned char d1, unsigned char d2, double fraction)")
    );
}
