/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_io_lexer_parser_export, "utils")
{

    boost::python::def(
        "parse_quoted_string",
        ( std::string ( * )( utils::CountingIstream &, bool, bool ))( &::genesis::utils::parse_quoted_string ),
        ( boost::python::arg("source"), boost::python::arg("use_escapes")=(bool)(true), boost::python::arg("include_qmarks")=(bool)(false) ),
        get_docstring("std::string ::genesis::utils::parse_quoted_string (utils::CountingIstream & source, bool use_escapes=true, bool include_qmarks=false)")
    );
}
