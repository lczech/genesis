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
 * @brief Implementation of TreeNode class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_node.hpp"

#include "tree/tree_link.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the index of this Node.
 */
size_t TreeNode::index() const
{
    return index_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 */
TreeLink& TreeNode::primary_link()
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 */
TreeLink const& TreeNode::primary_link() const
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 *
 * This is just an alias for primary_link(), that is shorter to use when needed
 * frequently in an algorithm.
 */
TreeLink& TreeNode::link()
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 *
 * This is just an alias for primary_link(), that is shorter to use when needed
 * frequently in an algorithm.
 */
TreeLink const& TreeNode::link() const
{
    return *link_;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

TreeNode& TreeNode::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

TreeNode& TreeNode::reset_primary_link( TreeLink* val )
{
    link_ = val;
    return *this;
}

// =================================================================================================
//     Member Functions
// =================================================================================================

/**
 * @brief Rank of the node, i.e. how many immediate children it has.
 */
size_t TreeNode::rank() const
{
    int rank = -1;
    TreeLink* link = link_;

    do {
        ++rank;
        link = &link->next();
    } while( link != link_ );

    // We add at least 1 to the initial value of the rank, so this is valid.
    return static_cast<size_t>(rank);
}

/**
 * @brief True iff the node is a leaf/tip.
 */
bool TreeNode::is_leaf() const
{
    return link_->is_leaf();
}

/**
 * @brief True iff the node is an inner node.
 */
bool TreeNode::is_inner() const
{
    return link_->is_inner();
}

/**
 * @brief Returns a one-line dump summary of the data of this node.
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
std::string TreeNode::dump() const
{
    // throw std::runtime_error( "Not implemented" );
    // return "";
    return "Base node dump";
    // return "Rank: " + std::to_string(rank()) + " \t " + "name: " + this->data.name;
}

} // namespace tree
} // namespace genesis
