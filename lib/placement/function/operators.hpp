#ifndef GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_
#define GENESIS_PLACEMENT_FUNCTION_OPERATORS_H_

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

#include "placement/placement_tree.hpp"

#include <iosfwd>
#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class Tree;

    class DefaultNodeData;
    class DefaultEdgeData;

    using DefaultTree = Tree;
}

namespace placement {
    class PlacementEdgeData;
    class PlacementNodeData;

    class Sample;
}

namespace placement {

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

bool compatible_trees( PlacementTree const& lhs, PlacementTree const& rhs );
bool compatible_trees( Sample        const& lhs, Sample        const& rhs );

// =================================================================================================
//     Conversion
// =================================================================================================

PlacementTree convert_to_placement_tree( tree::DefaultTree const& source_tree );

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, Sample const& smp );

std::string print_tree( Sample const& smp );

} // namespace placement
} // namespace genesis

#endif // include guard
