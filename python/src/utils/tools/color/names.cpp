/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (ColorNames, "utils")
{

    // -------------------------------------------------------------------
    //     Class ColorNames
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::ColorNames > ( "ColorNames" )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_tools_color_names_export, "utils")
{

    boost::python::def(
        "get_named_color",
        ( Color ( * )( std::string const & ))( &::genesis::utils::get_named_color ),
        ( boost::python::arg("name") ),
        get_docstring("Color ::genesis::utils::get_named_color (std::string const & name)")
    );

    boost::python::def(
        "is_named_color",
        ( bool ( * )( std::string const & ))( &::genesis::utils::is_named_color ),
        ( boost::python::arg("name") ),
        get_docstring("bool ::genesis::utils::is_named_color (std::string const & name)")
    );

    // boost::python::def(
    //     "get_named_color_iterator",
    //     ( std::array< std::pair< std::string, Color >, 140 >::const_iterator ( * )( std::string ))( &::genesis::utils::get_named_color_iterator ),
    //     ( boost::python::arg("name") ),
    //     get_docstring("std::array< std::pair< std::string, Color >, 140 >::const_iterator ::genesis::utils::get_named_color_iterator (std::string name)")
    // );
}
