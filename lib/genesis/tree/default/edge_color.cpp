/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
 * @ingroup tree
 */

#include "genesis/tree/default/edge_color.hpp"

#include "genesis/tree/default/tree.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/gradient.hpp"

#include <assert.h>
#include <algorithm>
#include <limits>

namespace genesis {
namespace tree {

// =================================================================================================
//     Edge Color Functions
// =================================================================================================

/**
 * @brief
 */
std::vector<utils::Color> edge_color_branch_length_gradient( Tree const& tree, bool zero_based )
{
    // Init the result vector with the min head color (green) for each edge.
    auto ret = std::vector<utils::Color>( tree.edge_count(), utils::heat_gradient(0.0) );
    if (tree.edge_count() == 0) {
        // If we are here, this is an empty vector.
        return ret;
    }

    // Perpare min and max.
    double min_bl = std::numeric_limits<double>::max();
    double max_bl = std::numeric_limits<double>::lowest();
    if (zero_based) {
        min_bl = 0.0;
    }

    // Find min and max branch lengths.
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        auto const& edge = **it;
        min_bl = std::min(min_bl, edge.data<DefaultEdgeData>().branch_length);
        max_bl = std::max(max_bl, edge.data<DefaultEdgeData>().branch_length);
    }
    assert( min_bl <= max_bl );
    double dist = max_bl - min_bl;

    // If all branch lengths are the same, return the vector as it is (initialized to green).
    if (dist == 0.0) {;
        return ret;
    }

    // Calculate the heat gradient color based on the branch length for each edge.
    for (auto it = tree.begin_edges(); it != tree.end_edges(); ++it) {
        auto const& edge = **it;
        double val = ( edge.data<DefaultEdgeData>().branch_length - min_bl) / dist;
        ret[edge.index()] = utils::heat_gradient(val);
    }
    return ret;
}

} // namespace tree
} // namespace genesis
