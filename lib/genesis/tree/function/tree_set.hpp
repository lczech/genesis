#ifndef GENESIS_TREE_FUNCTION_TREE_SET_H_
#define GENESIS_TREE_FUNCTION_TREE_SET_H_

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

#include <functional>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeSet;

// =================================================================================================
//     Tree Set Functions
// =================================================================================================

Tree      * find_tree ( TreeSet      & tset, std::string const& name);
Tree const* find_tree ( TreeSet const& tset, std::string const& name);

Tree average_branch_length_tree( TreeSet const& tset );

// =================================================================================================
//     Comparators
// =================================================================================================

bool all_equal(
    TreeSet const& tset,
    std::function<bool( TreeNode const&, TreeNode const& )> node_comparator,
    std::function<bool( TreeEdge const&, TreeEdge const& )> edge_comparator
);

// bool all_equal( TreeSet const& tset );

bool all_identical_topology( TreeSet const& tset );

} // namespace tree
} // namespace genesis

#endif // include guard
