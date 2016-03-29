#ifndef GENESIS_TREE_FUNCTION_OPERATORS_H_
#define GENESIS_TREE_FUNCTION_OPERATORS_H_

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
 * @brief Tree operator functions.
 *
 * @file
 * @ingroup tree
 */

#include <functional>

namespace genesis {
namespace tree {

// =================================================================================================
//     Equality
// =================================================================================================

template <class TreeTypeL, class TreeTypeR>
bool equal(
    const TreeTypeL& lhs,
    const TreeTypeR& rhs,
    std::function<bool
        (const typename TreeTypeL::NodeType&, const typename TreeTypeR::NodeType&)
    > node_comparator,
    std::function<bool
        (const typename TreeTypeL::EdgeType&, const typename TreeTypeR::EdgeType&)
    > edge_comparator
);

template <class TreeTypeL, class TreeTypeR>
bool equal(const TreeTypeL& lhs, const TreeTypeR& rhs);

template <class TreeTypeL, class TreeTypeR>
bool identical_topology(const TreeTypeL& lhs, const TreeTypeR& rhs);

// =================================================================================================
//     Validate
// =================================================================================================

template <class TreeType>
bool validate( TreeType const& tree );

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/function/operators.tpp"

#endif // include guard
