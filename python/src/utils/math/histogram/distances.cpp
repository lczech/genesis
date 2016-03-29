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

PYTHON_EXPORT_FUNCTIONS(utils_math_histogram_distances_export, "utils")
{

    boost::python::def(
        "earth_movers_distance",
        ( double ( * )( const Histogram &, const Histogram &, bool ))( &::genesis::utils::earth_movers_distance ),
        ( boost::python::arg("h1"), boost::python::arg("h2"), boost::python::arg("norm") )
    );

    // boost::python::def(
    //     "earth_movers_distance_simple",
    //     ( double ( * )( const Histogram &, const Histogram & ))( &::genesis::utils::earth_movers_distance_simple ),
    //     ( boost::python::arg("h1"), boost::python::arg("h2") )
    // );
}
