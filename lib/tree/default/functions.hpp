#ifndef GENESIS_TREE_DEFAULT_FUNCTIONS_H_
#define GENESIS_TREE_DEFAULT_FUNCTIONS_H_

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
 * @brief Default Tree functions.
 *
 * @file
 * @ingroup tree
 */

#include<string>
#include<vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Node Names
// =================================================================================================

template <class TreeType>
std::vector<std::string> node_names(
    TreeType& tree,
    bool leaves_only = false
);

template <class TreeType>
typename TreeType::NodeType* find_node(
    TreeType& tree,
    const std::string& name,
    bool replace_underscores = false
);

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/default/functions.tpp"

#endif // include guard
