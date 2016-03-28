/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

PYTHON_EXPORT_CLASS (LexerIterator, "utils")
{

    // -------------------------------------------------------------------
    //     Class LexerIterator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::LexerIterator > ( "LexerIterator", boost::python::init< Lexer *, long >(( boost::python::arg("lexer"), boost::python::arg("position") )) )

        // Public Member Functions

        .def(
            "consume_head",
            ( void ( ::genesis::utils::LexerIterator::* )(  ))( &::genesis::utils::LexerIterator::consume_head ),
            get_docstring("void ::genesis::utils::LexerIterator::consume_head ()")
        )
        .def(
            "get_lexer",
            ( Lexer & ( ::genesis::utils::LexerIterator::* )(  ) const )( &::genesis::utils::LexerIterator::get_lexer ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "has_error",
            ( bool ( ::genesis::utils::LexerIterator::* )(  ) const )( &::genesis::utils::LexerIterator::has_error ),
            get_docstring("bool ::genesis::utils::LexerIterator::has_error () const")
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self == boost::python::self )
    ;
}
