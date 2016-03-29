#ifndef GENESIS_TREE_FUNCTION_TREE_SET_H_
#define GENESIS_TREE_FUNCTION_TREE_SET_H_

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
 * @ingroup tree
 */

#include <functional>

namespace genesis {
namespace placement {

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

template <class TreeSetType>
typename TreeSetType::TreeType      * find_tree ( TreeSetType      & tset, std::string const& name);

template <class TreeSetType>
typename TreeSetType::TreeType const* find_tree ( TreeSetType const& tset, std::string const& name);

template <class TreeSetType>
typename TreeSetType::TreeType average_branch_length_tree( TreeSetType const& tset );

// =================================================================================================
//     Comparators
// =================================================================================================

template <class TreeSetType>
bool all_equal(
    TreeSetType const& tset,
    std::function<bool(
        typename TreeSetType::TreeType::NodeType const&,
        typename TreeSetType::TreeType::NodeType const&
    )> node_comparator,
    std::function<bool(
        typename TreeSetType::TreeType::EdgeType const&,
        typename TreeSetType::TreeType::EdgeType const&
    )> edge_comparator
);

template <class TreeSetType>
bool all_equal( TreeSetType const& tset );

template <class TreeSetType>
bool all_identical_topology( TreeSetType const& tset );

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This file contains template functions, so do the inclusion of the implementation here.
#include "tree/function/tree_set.tpp"

#endif // include guard
