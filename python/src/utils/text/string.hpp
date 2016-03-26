/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

template<typename T>
void python_export_function_utils_text_string_T ()
{
    boost::python::def(
        "join",
        ( std::string ( * )( T const &, std::string const & ))( &::genesis::utils::join ),
        ( boost::python::arg("v"), boost::python::arg("delimiter") )
    );
}

