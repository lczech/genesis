#ifndef GENESIS_TREE_FUNCTION_TREE_SET_H_
#define GENESIS_TREE_FUNCTION_TREE_SET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

/**
 * @brief Get the first Tree in a TreeSet that is stored with a given name,
 * or `nullptr` if not found.
 */
Tree      * find_tree ( TreeSet      & tset, std::string const& name);

/**
 * @brief Get the first Tree in a TreeSet that is stored with a given name,
 * or `nullptr` if not found.
 */
Tree const* find_tree ( TreeSet const& tset, std::string const& name);

// =================================================================================================
//     Comparators
// =================================================================================================

/**
 * @brief Compare whether all Tree%s in a TreeSet are equal using a given comparator functional.
 *
 * See Tree::equal() for more information.
 */
bool all_equal(
    TreeSet const& tset,
    std::function<bool( TreeNode const&, TreeNode const& )> node_comparator,
    std::function<bool( TreeEdge const&, TreeEdge const& )> edge_comparator
);

// /**
//  * @brief Compare whether all Tree%s in a TreeSet are equal using their default comparision
//  * operators for nodes and edges.
//  */
// bool all_equal( TreeSet const& tset );

/**
 * @brief Return true iff all Tree%s in a TreeSet have an identical topology.
 */
bool all_identical_topology( TreeSet const& tset );

} // namespace tree
} // namespace genesis

#endif // include guard
