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
 * @brief Implementation of Placement Simulator class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/simulator/simulator.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Simulator
// =================================================================================================

/**
 * @brief Generate `n` many Pqueries and place them in the Sample.
 */
void Simulator::generate( Sample& sample, size_t n )
{
    // Prepare distributions.
    edge_distribution_.prepare( sample );
    proximal_length_distribution_.prepare( sample );
    pendant_length_distribution_.prepare( sample );

    for (size_t i = 0; i < n; ++i) {
        // Generate one Pquery.
        Pquery& pqry = sample.add_pquery();
        pqry.add_name( "pquery_" + std::to_string( sample.pquery_size() ) );

        // Get a random edge.
        size_t edge_idx = edge_distribution_.generate();
        auto&  edge     = sample.tree().edge_at(edge_idx);

        // Add a placement at the edge.
        PqueryPlacement& place = pqry.add_placement(edge);
        place.proximal_length = proximal_length_distribution_.generate(edge);
        place.pendant_length  = pendant_length_distribution_.generate(edge);
    }
}

} // namespace placement
} // namespace genesis
