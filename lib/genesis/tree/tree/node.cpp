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
 * @brief Implementation of TreeNode class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree/node.hpp"

#include "genesis/tree/tree/edge.hpp"
#include "genesis/tree/tree/link.hpp"

#include <assert.h>

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

/**
 * @brief Return `true` if the TreeNode has a data object assigned to it.
 */
bool TreeNode::has_data() const
{
    return data_.get() != nullptr;
}

/**
 * @brief Return a pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseNodeData* TreeNode::data_ptr()
{
    return data_.get();
}

/**
 * @brief Return a const pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseNodeData const* TreeNode::data_ptr() const
{
    return data_.get();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Reset the internal index of this TreeNode.
 *
 * This is a helper function that needs to be used with care and only in cases where appropriate.
 * The index is an invariant that needs to be kept, as it needs to match the index in the Tree
 * container.
 *
 * This function exists to allow building and modifying a Tree without the need for many friend
 * declarations. However, the function should rarely be needed outside of this context.
 */
TreeNode& TreeNode::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

/**
 * @brief Reset the internal pointer to the TreeLink of this TreeNode.
 *
 * This is a helper function that needs to be used with care and only in cases where appropriate.
 *
 * This function exists to allow building and modifying a Tree without the need for many friend
 * declarations. However, the function should rarely be needed outside of this context.
 */
TreeNode& TreeNode::reset_primary_link( TreeLink* val )
{
    link_ = val;
    return *this;
}

/**
 * @brief Reset the data pointer of this TreeNode.
 *
 * Using this function, a TreeNode can be assigend new data. It is also possible to change the
 * data type completely (as long as it derives from BaseNodeData). Be however aware that many
 * functions that work with trees expcet a certain data type. Thus, changing it might break those
 * functions and lead to exceptions and other errors.
 */
TreeNode& TreeNode::reset_data( std::unique_ptr< BaseNodeData > data )
{
    data_ = std::move( data );
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
 * @brief True iff the node is the root of its Tree.
 */
bool TreeNode::is_root() const
{
    // The link_ is always the one pointing towards the root. Also, the edge of that link always has
    // the primary link set to that it points towards the root.
    // At the root itself, however, this means we are pointing to ourselves. Use this to check
    // if this node is the root.
    return &link_->edge().primary_link() == link_;
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
