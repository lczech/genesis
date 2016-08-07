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

#include "tree/tree/edge.hpp"

#include "tree/tree/link.hpp"

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

/**
 * @brief Return `true` if the TreeNode has a data object assigned to it.
 */
bool TreeEdge::has_data() const
{
    return data_.get() != nullptr;
}

/**
 * @brief Return a pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseEdgeData* TreeEdge::data_ptr()
{
    return data_.get();
}

/**
 * @brief Return a const pointer to the data.
 *
 * In most cases, using data<>() is more convenient. However, in some cases, this function
 * might be necessary.
 */
BaseEdgeData const* TreeEdge::data_ptr() const
{
    return data_.get();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Reset the internal index of this TreeEdge.
 *
 * This is a helper function that needs to be used with care and only in cases where appropriate.
 * The index is an invariant that needs to be kept, as it needs to match the index in the Tree
 * container.
 *
 * This function exists to allow building and modifying a Tree without the need for many friend
 * declarations. However, the function should rarely be needed outside of this context.
 */
TreeEdge& TreeEdge::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

/**
 * @brief Reset the internal pointer to the primary TreeLink of this TreeEdge.
 *
 * This is a helper function that needs to be used with care and only in cases where appropriate.
 *
 * This function exists to allow building and modifying a Tree without the need for many friend
 * declarations. However, the function should rarely be needed outside of this context.
 */
TreeEdge& TreeEdge::reset_primary_link( TreeLink* val )
{
    link_p_ = val;
    return *this;
}

/**
 * @brief Reset the internal pointer to the secondary TreeLink of this TreeEdge.
 *
 * This is a helper function that needs to be used with care and only in cases where appropriate.
 *
 * This function exists to allow building and modifying a Tree without the need for many friend
 * declarations. However, the function should rarely be needed outside of this context.
 */
TreeEdge& TreeEdge::reset_secondary_link( TreeLink* val )
{
    link_s_ = val;
    return *this;
}

/**
 * @brief Reset the data pointer of this TreeEdge.
 *
 * Using this function, a TreeEdge can be assigend new data. It is also possible to change the
 * data type completely (as long as it derives from BaseEdgeData). Be however aware that many
 * functions that work with trees expect a certain data type. Thus, changing it might break those
 * functions and lead to exceptions and other errors.
 */
TreeEdge& TreeEdge::reset_data( std::unique_ptr< BaseEdgeData > data )
{
    data_ = std::move( data );
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
