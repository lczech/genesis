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

PYTHON_EXPORT_CLASS (Histogram, "utils")
{

    // -------------------------------------------------------------------
    //     Class Histogram
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::Histogram > ( "Histogram", boost::python::init< size_t >(( boost::python::arg("num_bins") )) )
        .def( boost::python::init< size_t, double, double >(( boost::python::arg("num_bins"), boost::python::arg("range_min"), boost::python::arg("range_max") )) )
        .def( boost::python::init< const std::vector< double > & >(( boost::python::arg("ranges") )) )
        .def( boost::python::init< Histogram const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "accumulate",
            ( int ( ::genesis::utils::Histogram::* )( double, double ))( &::genesis::utils::Histogram::accumulate ),
            ( boost::python::arg("x"), boost::python::arg("weight") )
        )
        .def(
            "accumulate_bin",
            ( void ( ::genesis::utils::Histogram::* )( size_t, double ))( &::genesis::utils::Histogram::accumulate_bin ),
            ( boost::python::arg("bin"), boost::python::arg("weight") )
        )
        // .def(
        //     "at",
        //     ( double & ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::at ),
        //     ( boost::python::arg("bin_num") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>()
        // )
        .def(
            "at",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::at ),
            ( boost::python::arg("bin_num") )
        )
        .def(
            "bin_midpoint",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_midpoint ),
            ( boost::python::arg("bin_num") )
        )
        .def(
            "bin_range",
            ( std::pair< double, double > ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_range ),
            ( boost::python::arg("bin_num") )
        )
        .def(
            "bin_width",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::bin_width ),
            ( boost::python::arg("bin_num") )
        )
        .def(
            "bins",
            ( size_t ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::bins )
        )
        .def(
            "cbegin",
            ( ::genesis::utils::Histogram::const_iterator ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::cbegin )
        )
        .def(
            "cend",
            ( ::genesis::utils::Histogram::const_iterator ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::cend )
        )
        .def(
            "check_range",
            ( bool ( ::genesis::utils::Histogram::* )( double ) const )( &::genesis::utils::Histogram::check_range ),
            ( boost::python::arg("x") )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::Histogram::* )(  ))( &::genesis::utils::Histogram::clear )
        )
        .def(
            "find_bin",
            ( int ( ::genesis::utils::Histogram::* )( double ) const )( &::genesis::utils::Histogram::find_bin ),
            ( boost::python::arg("x") )
        )
        .def(
            "increment",
            ( int ( ::genesis::utils::Histogram::* )( double ))( &::genesis::utils::Histogram::increment ),
            ( boost::python::arg("x") )
        )
        .def(
            "increment_bin",
            ( void ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::increment_bin ),
            ( boost::python::arg("bin") )
        )
        .def(
            "out_of_range_behaviour",
            ( ::genesis::utils::Histogram::OutOfRangeBehaviour ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::out_of_range_behaviour )
        )
        .def(
            "out_of_range_behaviour",
            ( void ( ::genesis::utils::Histogram::* )( ::genesis::utils::Histogram::OutOfRangeBehaviour ))( &::genesis::utils::Histogram::out_of_range_behaviour ),
            ( boost::python::arg("v") )
        )
        .def(
            "range_max",
            ( double ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::range_max )
        )
        .def(
            "range_min",
            ( double ( ::genesis::utils::Histogram::* )(  ) const )( &::genesis::utils::Histogram::range_min )
        )
        .def(
            "set_ranges",
            ( void ( ::genesis::utils::Histogram::* )( const std::vector< double > & ))( &::genesis::utils::Histogram::set_ranges ),
            ( boost::python::arg("ranges") )
        )
        .def(
            "set_uniform_ranges",
            ( void ( ::genesis::utils::Histogram::* )( const double, const double ))( &::genesis::utils::Histogram::set_uniform_ranges ),
            ( boost::python::arg("min"), boost::python::arg("max") )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::Histogram::* )( Histogram & ))( &::genesis::utils::Histogram::swap ),
            ( boost::python::arg("other") )
        )

        // Operators

        // .def(
        //     "__getitem__",
        //     ( double & ( ::genesis::utils::Histogram::* )( size_t ))( &::genesis::utils::Histogram::operator[] ),
        //     ( boost::python::arg("bin_num") ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>()
        // )
        .def(
            "__getitem__",
            ( double ( ::genesis::utils::Histogram::* )( size_t ) const )( &::genesis::utils::Histogram::operator[] ),
            ( boost::python::arg("bin_num") )
        )
        .def( boost::python::self == boost::python::self )

        // Iterators

        // .def(
        //     "__iter__",
        //     boost::python::range ( &::genesis::utils::Histogram::begin, &::genesis::utils::Histogram::end )
        // )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_export, "utils")
{

    // boost::python::def(
    //     "equal_ranges",
    //     ( bool ( * )( Histogram const &, Histogram const & ))( &::genesis::utils::equal_ranges ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );

    // boost::python::def(
    //     "operator==",
    //     ( bool ( * )( Histogram const &, Histogram const & ))( &::genesis::utils::operator== ),
    //     ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    // );

    boost::python::def(
        "swap",
        ( void ( * )( Histogram &, Histogram & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
