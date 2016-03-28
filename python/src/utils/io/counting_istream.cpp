/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::utils;

/*
PYTHON_EXPORT_CLASS (CountingIstream, "utils")
{

    // -------------------------------------------------------------------
    //     Class CountingIstream
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::CountingIstream > ( "CountingIstream", boost::python::init<  >(  ) )
        .def( boost::python::init< std::istream & >(( boost::python::arg("in") )) )
        .def( boost::python::init< std::streambuf * >(( boost::python::arg("in_buf") )) )
        .def( boost::python::init< self_type const & >(( boost::python::arg("") )) )
        .def( boost::python::init< self_type && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "advance",
            ( void ( ::genesis::utils::CountingIstream::* )(  ))( &::genesis::utils::CountingIstream::advance ),
            get_docstring("void ::genesis::utils::CountingIstream::advance ()")
        )
        .def(
            "advance_non_counting",
            ( void ( ::genesis::utils::CountingIstream::* )(  ))( &::genesis::utils::CountingIstream::advance_non_counting ),
            get_docstring("void ::genesis::utils::CountingIstream::advance_non_counting ()")
        )
        .def(
            "at",
            ( std::string ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::at ),
            get_docstring("std::string ::genesis::utils::CountingIstream::at () const")
        )
        .def(
            "column",
            ( size_t ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::column ),
            get_docstring("size_t ::genesis::utils::CountingIstream::column () const")
        )
        .def(
            "current",
            ( char ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::current ),
            get_docstring("char ::genesis::utils::CountingIstream::current () const")
        )
        .def(
            "eos",
            ( bool ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::eos ),
            get_docstring("bool ::genesis::utils::CountingIstream::eos () const")
        )
        .def(
            "good",
            ( bool ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::good ),
            get_docstring("bool ::genesis::utils::CountingIstream::good () const")
        )
        .def(
            "line",
            ( size_t ( ::genesis::utils::CountingIstream::* )(  ) const )( &::genesis::utils::CountingIstream::line ),
            get_docstring("size_t ::genesis::utils::CountingIstream::line () const")
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::CountingIstream::* )( self_type & ))( &::genesis::utils::CountingIstream::swap ),
            ( boost::python::arg("other") )
        )
    ;
}
*/
