/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

template<class T>
void python_export_function_utils_io_lexer_parser_T ()
{
    boost::python::def(
        "parse_float",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_float ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_float (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_integer (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_signed_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_signed_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_signed_integer (utils::CountingIstream & source)")
    );

    boost::python::def(
        "parse_unsigned_integer",
        ( T ( * )( utils::CountingIstream & ))( &::genesis::utils::parse_unsigned_integer ),
        ( boost::python::arg("source") ),
        get_docstring("T ::genesis::utils::parse_unsigned_integer (utils::CountingIstream & source)")
    );
}

