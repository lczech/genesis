#ifndef GENESIS_TREE_DEFAULT_OPERATORS_H_
#define GENESIS_TREE_DEFAULT_OPERATORS_H_

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

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;

class DefaultNodeData;
class DefaultEdgeData;

using DefaultTree = Tree;

// =================================================================================================
//     Conversion
// =================================================================================================

/**
 * @brief Convert a Tree to a DefaultTree with DefaultNodeData and DefaultEdgeData.
 *
 * This works for all Tree%s that have data derived from those two data base classes.
 * Also nodes or edges without data work. However, an expection is thrown if any node or edge
 * contains data that is not derived from DefaultNodeData and DefaultEdgeData, respectively.
 *
 * The data itself is copied using the clone functions, so that all names and branch lengths
 * are transferred to the returned Tree.
 */
DefaultTree convert_to_default_tree( Tree const& source_tree );

} // namespace tree
} // namespace genesis

#endif // include guard
