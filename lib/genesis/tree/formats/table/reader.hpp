#ifndef GENESIS_TREE_FORMATS_TABLE_READER_H_
#define GENESIS_TREE_FORMATS_TABLE_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Table Reader
// =================================================================================================

/**
 * @brief Create a tree, given lists of child parent pairs.
 *
 * The two provided lists need to have the same length, and corresponding entries that form
 * child-parent-pairs of node names. These names are then used to create a tree.
 */
Tree make_tree_from_parents_table(
    std::vector<std::string> const& child_names,
    std::vector<std::string> const& parent_names
);

} // namespace tree
} // namespace genesis

#endif // include guard
