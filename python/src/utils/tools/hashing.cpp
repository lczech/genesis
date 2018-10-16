/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS( utils_tools_hashing_export, ::genesis::utils, scope )
{

    scope.def(
        "hash_from_file_hex",
        ( std::string ( * )( std::string const &, HashingFunctions ))( &::genesis::utils::hash_from_file_hex ),
            pybind11::arg("filename"),
            pybind11::arg("hash_fct"),
        get_docstring("std::string ::genesis::utils::hash_from_file_hex (std::string const & filename, HashingFunctions hash_fct)")
    );

    scope.def(
        "hash_from_stream_hex",
        ( std::string ( * )( std::istream &, HashingFunctions ))( &::genesis::utils::hash_from_stream_hex ),
            pybind11::arg("is"),
            pybind11::arg("hash_fct"),
        get_docstring("std::string ::genesis::utils::hash_from_stream_hex (std::istream & is, HashingFunctions hash_fct)")
    );

    scope.def(
        "hash_from_string_hex",
        ( std::string ( * )( std::string const &, HashingFunctions ))( &::genesis::utils::hash_from_string_hex ),
            pybind11::arg("input"),
            pybind11::arg("hash_fct"),
        get_docstring("std::string ::genesis::utils::hash_from_string_hex (std::string const & input, HashingFunctions hash_fct)")
    );
}
