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
void python_export_function_utils_formats_svg_helper_T ()
{
    scope.def(
        "svg_attribute",
        ( std::string ( * )( std::string const &, T const &, std::string const & ))( &::genesis::utils::svg_attribute ),
            pybind11::arg("name"),
            pybind11::arg("value"),
            pybind11::arg("unit")=(std::string const &)("")
    );
}
