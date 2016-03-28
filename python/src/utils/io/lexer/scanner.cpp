/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_FUNCTIONS(utils_io_lexer_scanner_export, "utils")
{

    boost::python::def(
        "read_char",
        ( char ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("char ::genesis::utils::read_char (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_char",
        ( char ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("char ::genesis::utils::read_char (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "read_until",
        ( std::string ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_until (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_until",
        ( std::string ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_until (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "read_while",
        ( std::string ( * )( utils::CountingIstream &, char ))( &::genesis::utils::read_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_while (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "read_while",
        ( std::string ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::read_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("std::string ::genesis::utils::read_while (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "check_char",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::check_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::check_char (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "check_char",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::check_char ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::check_char (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "skip_until",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::skip_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_until (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "skip_until",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::skip_until ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_until (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );

    boost::python::def(
        "skip_while",
        ( void ( * )( utils::CountingIstream &, char ))( &::genesis::utils::skip_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_while (utils::CountingIstream & source, char criterion)")
    );

    boost::python::def(
        "skip_while",
        ( void ( * )( utils::CountingIstream &, std::function< bool(char)> ))( &::genesis::utils::skip_while ),
        ( boost::python::arg("source"), boost::python::arg("criterion") ),
        get_docstring("void ::genesis::utils::skip_while (utils::CountingIstream & source, std::function< bool(char)> criterion)")
    );
}
