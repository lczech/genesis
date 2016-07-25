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
 * @brief Implementation of TreeEdge class.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_edge.hpp"

#include "tree/tree_link.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the index of this Edge.
 */
size_t TreeEdge::index() const
{
    return index_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points towards the root.
 */
TreeLink& TreeEdge::primary_link()
{
    return *link_p_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points towards the root.
 */
TreeLink const& TreeEdge::primary_link() const
{
    return *link_p_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points away from the root.
 */
TreeLink& TreeEdge::secondary_link()
{
    return *link_s_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points away from the root.
 */
TreeLink const& TreeEdge::secondary_link() const
{
    return *link_s_;
}

/**
 * @brief Return the TreeNode of this TreeEdge that points towards the root.
 */
TreeNode& TreeEdge::primary_node()
{
    return link_p_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points towards the root.
 */
TreeNode const& TreeEdge::primary_node() const
{
    return link_p_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points away from the root.
 */
TreeNode& TreeEdge::secondary_node()
{
    return link_s_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points away from the root.
 */
TreeNode const& TreeEdge::secondary_node() const
{
    return link_s_->node();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

TreeEdge& TreeEdge::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

TreeEdge& TreeEdge::reset_primary_link( TreeLink* val )
{
    link_p_ = val;
    return *this;
}

TreeEdge& TreeEdge::reset_secondary_link( TreeLink* val )
{
    link_s_ = val;
    return *this;
}

// =================================================================================================
//     Member Functions
// =================================================================================================

/**
 * @brief Returns a one-line dump summary of the data of this edge.
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
std::string TreeEdge::dump() const
{
    // throw std::runtime_error( "Not implemented" );
    // return "Node P: " + primary_node().data.name + ", Node S: " + secondary_node().data.name + "\t" + data.dump();
    return "Base edge dump";
}

} // namespace tree
} // namespace genesis
