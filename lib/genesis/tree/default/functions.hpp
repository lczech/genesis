#ifndef GENESIS_TREE_DEFAULT_FUNCTIONS_H_
#define GENESIS_TREE_DEFAULT_FUNCTIONS_H_

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
 * @brief Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/utils/containers/sorted_vector.hpp"

#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeSet;

// =================================================================================================
//     Node Names
// =================================================================================================

std::unordered_set<std::string> node_names(
    Tree const& tree,
    bool leaves_only = false
);

utils::SortedVector<std::string> node_names_sorted(
    Tree const& tree,
    bool leaves_only = false
);

std::unordered_set<std::string> node_names(
    TreeSet const& tree_set,
    bool leaves_only = false
);

utils::SortedVector<std::string> node_names_sorted(
    TreeSet const& tree_set,
    bool leaves_only = false
);

TreeNode const* find_node(
    Tree const& tree,
    std::string const& name,
    bool replace_underscores = false
);

TreeNode* find_node(
    Tree& tree,
    std::string const& name,
    bool replace_underscores = false
);

// =================================================================================================
//     Branch Length
// =================================================================================================

double length(   Tree const& tree );
double height(   Tree const& tree );
double diameter( Tree const& tree );

std::vector<double> branch_lengths( Tree const& tree );

void set_all_branch_lengths(
    Tree&  tree,
    double length = 1.0
);

void scale_all_branch_lengths(
    Tree&  tree,
    double factor = 1.0
);

} // namespace tree
} // namespace genesis

#endif // include guard
