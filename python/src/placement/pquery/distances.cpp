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

PYTHON_EXPORT_FUNCTIONS(placement_pquery_distances_export, "placement")
{

    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const Pquery &, const Pquery &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pqry_a"), boost::python::arg("pqry_b"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const Pquery &, const PlacementTreeNode &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pquery"), boost::python::arg("node"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const PlacementTreeNode &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement"), boost::python::arg("node"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const Pquery &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement"), boost::python::arg("pquery"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlacement &, const PqueryPlacement &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("placement_a"), boost::python::arg("placement_b"), boost::python::arg("node_distance_matrix") )
    // );
    //
    // boost::python::def(
    //     "distance",
    //     ( double ( * )( const PqueryPlain &, const PqueryPlain &, const utils::Matrix< double > & ))( &::genesis::placement::distance ),
    //     ( boost::python::arg("pqry_a"), boost::python::arg("pqry_b"), boost::python::arg("node_distance_matrix") )
    // );
}
