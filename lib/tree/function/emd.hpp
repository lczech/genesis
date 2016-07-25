#ifndef GENESIS_TREE_FUNCTION_EMD_H_
#define GENESIS_TREE_FUNCTION_EMD_H_

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
 * @ingroup placement
 */

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;

// =================================================================================================
//     Typedefs
// =================================================================================================

using EmdTree = Tree;

// =================================================================================================
//     EMD Functions
// =================================================================================================

double earth_movers_distance( EmdTree const& tree );

void transform_to_unit_branch_lengths( EmdTree& tree );
double center_masses_on_branches( EmdTree& tree );

double sum_of_masses( EmdTree const& tree );

bool validate_emd_tree( EmdTree const& tree, double valid_total_mass_difference = 0.00001 );

} // namespace tree
} // namespace genesis

#endif // include guard
