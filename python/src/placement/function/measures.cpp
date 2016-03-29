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

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_function_measures_export, "placement")
{

    boost::python::def(
        "center_of_gravity_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::center_of_gravity_distance ),
        ( boost::python::arg("map_a"), boost::python::arg("map_b"), boost::python::arg("with_pendant_length") )
    );

    boost::python::def(
        "center_of_gravity_variance",
        ( double ( * )( const Sample &, bool ))( &::genesis::placement::center_of_gravity_variance ),
        ( boost::python::arg("map"), boost::python::arg("with_pendant_length") ),
        get_docstring("double ::genesis::placement::center_of_gravity_variance (const Sample & map, bool with_pendant_length)")
    );

    boost::python::def(
        "earth_movers_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::earth_movers_distance ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("with_pendant_length") ),
        get_docstring("double ::genesis::placement::earth_movers_distance (const Sample & lhs, const Sample & rhs, bool with_pendant_length)")
    );

    boost::python::def(
        "node_histogram_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::node_histogram_distance ),
        ( boost::python::arg("map_a"), boost::python::arg("map_b"), boost::python::arg("with_pendant_length") )
    );

    boost::python::def(
        "pairwise_distance",
        ( double ( * )( const Sample &, const Sample &, bool ))( &::genesis::placement::pairwise_distance ),
        ( boost::python::arg("map_a"), boost::python::arg("map_b"), boost::python::arg("with_pendant_length") ),
        get_docstring("double ::genesis::placement::pairwise_distance (const Sample & map_a, const Sample & map_b, bool with_pendant_length)")
    );

    boost::python::def(
        "pquery_distance",
        ( double ( * )( const PqueryPlain &, const PqueryPlain &, const utils::Matrix< double > &, bool ))( &::genesis::placement::pquery_distance ),
        ( boost::python::arg("pqry_a"), boost::python::arg("pqry_b"), boost::python::arg("node_distances"), boost::python::arg("with_pendant_length") ),
        get_docstring("double ::genesis::placement::pquery_distance (const PqueryPlain & pqry_a, const PqueryPlain & pqry_b, const utils::Matrix< double > & node_distances, bool with_pendant_length)")
    );

    boost::python::def(
        "variance",
        ( double ( * )( const Sample &, bool ))( &::genesis::placement::variance ),
        ( boost::python::arg("map"), boost::python::arg("with_pendant_length") ),
        get_docstring("double ::genesis::placement::variance (const Sample & map, bool with_pendant_length)")
    );

    // boost::python::def(
    //     "variance_partial",
    //     ( double ( * )( const PqueryPlain &, const std::vector< PqueryPlain > &, const utils::Matrix< double > &, bool ))( &::genesis::placement::variance_partial ),
    //     ( boost::python::arg("pqry_a"), boost::python::arg("pqrys_b"), boost::python::arg("node_distances"), boost::python::arg("with_pendant_length") ),
    //     get_docstring("double ::genesis::placement::variance_partial (const PqueryPlain & pqry_a, const std::vector< PqueryPlain > & pqrys_b, const utils::Matrix< double > & node_distances, bool with_pendant_length)")
    // );

    boost::python::def(
        "center_of_gravity",
        ( std::pair< PlacementTreeEdge const *, double > ( * )( const Sample &, bool ))( &::genesis::placement::center_of_gravity ),
        ( boost::python::arg("map"), boost::python::arg("with_pendant_length") ),
        get_docstring("std::pair< PlacementTreeEdge const *, double > ::genesis::placement::center_of_gravity (const Sample & map, bool with_pendant_length)")
    );

    boost::python::def(
        "node_distance_histograms",
        ( std::vector< utils::Histogram > ( * )( const Sample &, bool ))( &::genesis::placement::node_distance_histograms ),
        ( boost::python::arg("map"), boost::python::arg("with_pendant_length") )
    );

    boost::python::def(
        "node_distance_histogram",
        ( utils::Histogram ( * )( const PlacementTreeNode &, bool ))( &::genesis::placement::node_distance_histogram ),
        ( boost::python::arg("node"), boost::python::arg("with_pendant_length") )
    );

    // boost::python::def(
    //     "node_histogram_distance_matrix",
    //     ( utils::Matrix< double > ( * )( const SampleSet &, bool ))( &::genesis::placement::node_histogram_distance_matrix ),
    //     ( boost::python::arg("maps"), boost::python::arg("with_pendant_length")=(bool)(false) )
    // );

    // boost::python::def(
    //     "variance_thread",
    //     ( void ( * )( const int, const int, const std::vector< PqueryPlain > *, const utils::Matrix< double > *, double *, bool ))( &::genesis::placement::variance_thread ),
    //     ( boost::python::arg("offset"), boost::python::arg("incr"), boost::python::arg("pqrys"), boost::python::arg("node_distances"), boost::python::arg("partial"), boost::python::arg("with_pendant_length") ),
    //     get_docstring("void ::genesis::placement::variance_thread (const int offset, const int incr, const std::vector< PqueryPlain > * pqrys, const utils::Matrix< double > * node_distances, double * partial, bool with_pendant_length)")
    // );
}
