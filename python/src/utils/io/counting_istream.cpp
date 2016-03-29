/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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
