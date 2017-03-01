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
void python_export_function_utils_text_string_T ()
{
    scope.def(
        "join",
        ( std::string ( * )( T const &, std::string const & ))( &::genesis::utils::join ),
            pybind11::arg("v"),
            pybind11::arg("delimiter"),
        get_docstring("std::string ::genesis::utils::join (T const & v, std::string const & delimiter)")
    );

    scope.def(
        "to_string",
        ( std::string ( * )( T const & ))( &::genesis::utils::to_string ),
            pybind11::arg("v"),
        get_docstring("std::string ::genesis::utils::to_string (T const & v)")
    );
}
