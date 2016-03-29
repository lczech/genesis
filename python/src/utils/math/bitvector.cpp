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

PYTHON_EXPORT_CLASS (Bitvector, "utils")
{

    // -------------------------------------------------------------------
    //     Class Bitvector
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Bitvector > ( "Bitvector", boost::python::init< const size_t, boost::python::optional< const bool > >(( boost::python::arg("size"), boost::python::arg("init")=(const bool)(false) )) )
        .def( boost::python::init< const size_t, const std::initializer_list< int > >(( boost::python::arg("size"), boost::python::arg("list") )) )

        // Public Member Functions

        .def(
            "count",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::count ),
            get_docstring("size_t ::genesis::utils::Bitvector::count () const")
        )
        .def(
            "dump",
            ( std::string ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::dump )
        )
        .def(
            "dump_int",
            ( std::string ( ::genesis::utils::Bitvector::* )( ::genesis::utils::Bitvector::IntType ) const )( &::genesis::utils::Bitvector::dump_int ),
            ( boost::python::arg("x") )
        )
        .def(
            "flip",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::flip ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::flip (size_t index)")
        )
        .def(
            "get",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::get ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::utils::Bitvector::get (size_t index) const")
        )
        .def(
            "hash",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::hash ),
            get_docstring("size_t ::genesis::utils::Bitvector::hash () const")
        )
        .def(
            "invert",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::invert ),
            get_docstring("void ::genesis::utils::Bitvector::invert ()")
        )
        .def(
            "normalize",
            ( void ( ::genesis::utils::Bitvector::* )(  ))( &::genesis::utils::Bitvector::normalize ),
            get_docstring("void ::genesis::utils::Bitvector::normalize ()")
        )
        .def(
            "reset",
            ( void ( ::genesis::utils::Bitvector::* )( const bool ))( &::genesis::utils::Bitvector::reset ),
            ( boost::python::arg("value")=(const bool)(false) ),
            get_docstring("void ::genesis::utils::Bitvector::reset (const bool value=false)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::set ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index)")
        )
        .def(
            "set",
            ( void ( ::genesis::utils::Bitvector::* )( size_t, bool ))( &::genesis::utils::Bitvector::set ),
            ( boost::python::arg("index"), boost::python::arg("value") ),
            get_docstring("void ::genesis::utils::Bitvector::set (size_t index, bool value)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::size ),
            get_docstring("size_t ::genesis::utils::Bitvector::size () const")
        )
        // .def(
        //     "symmetric_difference",
        //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::Bitvector::symmetric_difference ),
        //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
        // )
        // .staticmethod("symmetric_difference")
        .def(
            "symmetric_difference",
            ( Bitvector ( ::genesis::utils::Bitvector::* )( Bitvector const & ) const )( &::genesis::utils::Bitvector::symmetric_difference ),
            ( boost::python::arg("rhs") )
        )
        .def(
            "unset",
            ( void ( ::genesis::utils::Bitvector::* )( size_t ))( &::genesis::utils::Bitvector::unset ),
            ( boost::python::arg("index") ),
            get_docstring("void ::genesis::utils::Bitvector::unset (size_t index)")
        )
        .def(
            "x_hash",
            ( ::genesis::utils::Bitvector::IntType ( ::genesis::utils::Bitvector::* )(  ) const )( &::genesis::utils::Bitvector::x_hash ),
            get_docstring("IntType ::genesis::utils::Bitvector::x_hash () const")
        )

        // Operators

        .def( boost::python::self != boost::python::self )
        .def( boost::python::self &= boost::python::self )
        .def( boost::python::self < boost::python::self )
        .def( boost::python::self <= boost::python::self )
        .def( boost::python::self == boost::python::self )
        .def( boost::python::self > boost::python::self )
        .def( boost::python::self >= boost::python::self )
        .def(
            "__getitem__",
            ( bool ( ::genesis::utils::Bitvector::* )( size_t ) const )( &::genesis::utils::Bitvector::operator[] ),
            ( boost::python::arg("index") ),
            get_docstring("bool ::genesis::utils::Bitvector::operator[] (size_t index) const")
        )
        .def( boost::python::self ^= boost::python::self )
        .def( boost::python::self |= boost::python::self )
        .def( ~boost::python::self )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_math_bitvector_export, "utils")
{

    // boost::python::def(
    //     "operator&",
    //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator& ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );
    //
    // boost::python::def(
    //     "operator-",
    //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator- ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") ),
    //     get_docstring("Bitvector ::genesis::utils::operator- (Bitvector const & lhs, Bitvector const & rhs)")
    // );
    //
    // boost::python::def(
    //     "operator^",
    //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator^ ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );
    //
    // boost::python::def(
    //     "operator|",
    //     ( Bitvector ( * )( Bitvector const &, Bitvector const & ))( &::genesis::utils::operator| ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );
    //
    // boost::python::def(
    //     "operator<<",
    //     ( std::ostream & ( * )( std::ostream &, Bitvector const & ))( &::genesis::utils::operator<< ),
    //     ( boost::python::arg("s"), boost::python::arg("rhs") ),
    //     boost::python::return_value_policy<boost::python::reference_existing_object>()
    // );
}
