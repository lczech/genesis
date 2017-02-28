#ifndef GENESIS_PLACEMENT_PQUERY_PLAIN_H_
#define GENESIS_PLACEMENT_PQUERY_PLAIN_H_

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
 * @brief Header of PqueryPlain class.
 *
 * @file
 * @ingroup placement
 */

#include <cstddef>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Placement Plain
// =================================================================================================

/**
 * @brief Simple POD struct for a Placement used for speeding up some calculations.
 *
 * It is not as flexible as the default representation, but its memory footprint is compact, because
 * of the lack of pointers. Except for its purpose in some calculations, it is normally not needed
 * for anything else.
 */
struct PqueryPlacementPlain
{
    size_t edge_index;
    size_t primary_node_index;
    size_t secondary_node_index;

    double branch_length;
    double pendant_length;
    double proximal_length;
    double like_weight_ratio;
};

// =================================================================================================
//     Pquery Plain
// =================================================================================================

/**
 * @brief Simple POD struct that stores the information of a Pquery in a simple format for
 * speeding up some calculations.
 *
 * This class is used as an alternative representation of Pqueries that does not use pointer and
 * thus is faster in certain calculations. It is normally not needed for anything else.
 */
struct PqueryPlain
{
    size_t                            index;
    std::vector<PqueryPlacementPlain> placements;
};

} // namespace placement
} // namespace genesis

#endif // include guard
