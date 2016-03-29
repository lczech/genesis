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

PYTHON_EXPORT_CLASS (HistogramAccumulator, "utils")
{

    // -------------------------------------------------------------------
    //     Class HistogramAccumulator
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::utils::HistogramAccumulator > ( "HistogramAccumulator", boost::python::init<  >(  ) )
        .def( boost::python::init< const std::vector< double > &, boost::python::optional< double > >(( boost::python::arg("values"), boost::python::arg("weight")=(double)(1.0) )) )
        .def( boost::python::init< const std::vector< std::pair< double, double >> & >(( boost::python::arg("weighted_values") )) )
        .def( boost::python::init< HistogramAccumulator const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "accumulate",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( double, double ))( &::genesis::utils::HistogramAccumulator::accumulate ),
            ( boost::python::arg("x"), boost::python::arg("weight") )
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, bool ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            ( boost::python::arg("num_bins"), boost::python::arg("integer_ranges")=(bool)(false) )
        )
        .def(
            "build_uniform_ranges_histogram",
            ( Histogram ( ::genesis::utils::HistogramAccumulator::* )( size_t, double, double ) const )( &::genesis::utils::HistogramAccumulator::build_uniform_ranges_histogram ),
            ( boost::python::arg("num_bins"), boost::python::arg("min"), boost::python::arg("max") )
        )
        .def(
            "clear",
            ( void ( ::genesis::utils::HistogramAccumulator::* )(  ))( &::genesis::utils::HistogramAccumulator::clear )
        )
        .def(
            "empty",
            ( bool ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::empty )
        )
        .def(
            "increment",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( double ))( &::genesis::utils::HistogramAccumulator::increment ),
            ( boost::python::arg("x") )
        )
        .def(
            "max",
            ( double ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::max )
        )
        .def(
            "min",
            ( double ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::min )
        )
        .def(
            "size",
            ( size_t ( ::genesis::utils::HistogramAccumulator::* )(  ) const )( &::genesis::utils::HistogramAccumulator::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::utils::HistogramAccumulator::* )( HistogramAccumulator & ))( &::genesis::utils::HistogramAccumulator::swap ),
            ( boost::python::arg("other") )
        )

        // Iterators

        .def(
            "__iter__",
            boost::python::range ( &::genesis::utils::HistogramAccumulator::begin, &::genesis::utils::HistogramAccumulator::end )
        )
    ;
}

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_accumulator_export, "utils")
{

    boost::python::def(
        "swap",
        ( void ( * )( HistogramAccumulator &, HistogramAccumulator & ))( &::genesis::utils::swap ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") )
    );
}
