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
 * @brief Implementation of TreeLink class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree/link.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the index of this Link.
 */
size_t TreeLink::index() const
{
    return index_;
}

/**
 * @brief Return the next TreeLink within the TreeNode of this link.
 */
TreeLink& TreeLink::next()
{
    return *next_;
}

/**
 * @brief Return the next TreeLink within the TreeNode of this link.
 */
TreeLink const& TreeLink::next() const
{
    return *next_;
}

/**
 * @brief Return the previous TreeLink within the TreeNode of this link.
 *
 * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
 * As this link first has to be found, this function is not as cheap as next().
 */
TreeLink& TreeLink::prev()
{
    TreeLink* res = this;
    while( &res->next() != this ) {
        res = &res->next();
    }
    return *res;
}

/**
 * @brief Return the previous TreeLink within the TreeNode of this link.
 *
 * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
 * As this link first has to be found, this function is not as cheap as next().
 */
TreeLink const& TreeLink::prev() const
{
    TreeLink const* res = this;
    while( &res->next() != this ) {
        res = &res->next();
    }
    return *res;
}

/**
 * @brief Return the TreeLink of the adjacent TreeNode.
 */
TreeLink& TreeLink::outer()
{
    return *outer_;
}

/**
 * @brief Return the TreeLink of the adjacent TreeNode.
 */
TreeLink const& TreeLink::outer() const
{
    return *outer_;
}

/**
 * @brief Return the TreeEdge of this TreeLink.
 */
TreeEdge& TreeLink::edge()
{
    return *edge_;
}

/**
 * @brief Return the TreeEdge of this TreeLink.
 */
TreeEdge const& TreeLink::edge() const
{
    return *edge_;
}

/**
 * @brief Return the TreeNode of this TreeLink.
 */
TreeNode& TreeLink::node()
{
    return *node_;
}

/**
 * @brief Return the TreeNode of this TreeLink.
 */
TreeNode const& TreeLink::node() const
{
    return *node_;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

TreeLink& TreeLink::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

TreeLink& TreeLink::reset_next(  TreeLink* val )
{
    next_ = val;
    return *this;
}

TreeLink& TreeLink::reset_outer( TreeLink* val )
{
    outer_ = val;
    return *this;
}

TreeLink& TreeLink::reset_node( TreeNode* val )
{
    node_ = val;
    return *this;
}

TreeLink& TreeLink::reset_edge( TreeEdge* val )
{
    edge_ = val;
    return *this;
}

// =================================================================================================
//     Member Functions
// =================================================================================================

/**
 * @brief Return true iff the node of this link is a leaf node.
 */
bool TreeLink::is_leaf() const
{
    return next_ == this;
}

/**
 * @brief Return true iff the node of this link is an inner node.
 */
bool TreeLink::is_inner() const
{
    return next_ != this;
}

/**
 * @brief Return a string containing dump information about this link.
 *
 * At the moment, a link does not contain any information, so an empty string is returned.
 * This might change in the future, in case links also contain data.
 */
std::string TreeLink::dump() const
{
    return "";
}

} // namespace tree
} // namespace genesis
