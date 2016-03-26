/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (Lexer, "utils")
{

    // -------------------------------------------------------------------
    //     Class Lexer
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Lexer > ( "Lexer" )

        // Public Member Functions

        .def(
            "back",
            ( LexerToken ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::back ),
            get_docstring("LexerToken ::genesis::utils::Lexer::back () const")
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::clear ),
            get_docstring("void ::genesis::utils::Lexer::clear ()")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::dump ),
            get_docstring("std::string ::genesis::utils::Lexer::dump () const")
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::empty ),
            get_docstring("bool ::genesis::utils::Lexer::empty () const")
        )
        .def(
            "from_string",
            ( bool ( ::genesis::utils::Lexer::* )( const std::string & ))( &::genesis::utils::Lexer::from_string ),
            ( boost::python::arg("in") ),
            get_docstring("bool ::genesis::utils::Lexer::from_string (const std::string & in)")
        )
        .def(
            "has_error",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::has_error ),
            get_docstring("bool ::genesis::utils::Lexer::has_error () const")
        )
        .def(
            "process_all",
            ( bool ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::process_all )
        )
        .def(
            "process_step",
            ( bool ( ::genesis::utils::Lexer::* )(  ))( &::genesis::utils::Lexer::process_step ),
            get_docstring("bool ::genesis::utils::Lexer::process_step ()")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::size ),
            get_docstring("size_t ::genesis::utils::Lexer::size () const")
        )
        .def(
            "validate_brackets",
            ( bool ( ::genesis::utils::Lexer::* )(  ) const )( &::genesis::utils::Lexer::validate_brackets ),
            get_docstring("bool ::genesis::utils::Lexer::validate_brackets () const")
        )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::utils::Lexer::begin, &::genesis::utils::Lexer::end )
        // )
    ;
}
