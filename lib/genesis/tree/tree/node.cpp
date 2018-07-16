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

size_t TreeNode::index() const
{
    return index_;
}

TreeLink& TreeNode::primary_link()
{
    return *link_;
}

TreeLink const& TreeNode::primary_link() const
{
    return *link_;
}

TreeLink& TreeNode::link()
{
    return *link_;
}

TreeLink const& TreeNode::link() const
{
    return *link_;
}

bool TreeNode::has_data() const
{
    return data_.get() != nullptr;
}

BaseNodeData* TreeNode::data_ptr()
{
    return data_.get();
}

BaseNodeData const* TreeNode::data_ptr() const
{
    return data_.get();
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

TreeNode& TreeNode::reset_data( std::unique_ptr< BaseNodeData > data )
{
    data_ = std::move( data );
    return *this;
}

// =================================================================================================
//     Member Functions
// =================================================================================================

size_t TreeNode::degree() const
{
    size_t dgr = 0;
    TreeLink* lnk = link_;

    do {
        ++dgr;
        lnk = &lnk->next();
    } while( lnk != link_ );

    return dgr;
}

bool TreeNode::is_leaf() const
{
    return link_->is_leaf();
}

bool TreeNode::is_inner() const
{
    return link_->is_inner();
}

bool TreeNode::is_root() const
{
    // The link_ is always the one pointing towards the root. Also, the edge of that link always has
    // the primary link set to that it points towards the root.
    // At the root itself, however, this means we are pointing to ourselves. Use this to check
    // if this node is the root.
    return &link_->edge().primary_link() == link_;
}

} // namespace tree
} // namespace genesis
