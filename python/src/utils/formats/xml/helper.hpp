/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

template<typename T>
void python_export_function_utils_formats_xml_helper_T ()
{
    scope.def(
        "xml_attribute",
        ( std::string ( * )( std::string const &, T const & ))( &::genesis::utils::xml_attribute ),
            pybind11::arg("name"),
            pybind11::arg("value")
    );
}
