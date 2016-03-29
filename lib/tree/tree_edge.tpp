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
 * @brief Implementation of TreeEdge class template.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree_link.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the index of this Link.
 */
template <class NDT, class EDT>
size_t TreeEdge<NDT, EDT>::index() const
{
    return index_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points towards the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeEdge<NDT, EDT>::primary_link()
{
    return *link_p_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points towards the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeEdge<NDT, EDT>::primary_link() const
{
    return *link_p_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points away from the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeEdge<NDT, EDT>::secondary_link()
{
    return *link_s_;
}

/**
 * @brief Return the TreeLink of this TreeEdge that points away from the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeEdge<NDT, EDT>::secondary_link() const
{
    return *link_s_;
}

// The following are inline definitions that would create circular dependecies when included in the
// class definition. Thus, they need to be here, after the definition, so that their dependend
// source files can be included without circles.
// See http://www.cplusplus.com/forum/articles/10627/ for more information on this.

/**
 * @brief Return the TreeNode of this TreeEdge that points towards the root.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT>& TreeEdge<NDT, EDT>::primary_node()
{
    return link_p_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points towards the root.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT> const& TreeEdge<NDT, EDT>::primary_node() const
{
    return link_p_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points away from the root.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT>& TreeEdge<NDT, EDT>::secondary_node()
{
    return link_s_->node();
}

/**
 * @brief Return the TreeNode of this TreeEdge that points away from the root.
 */
template <class NDT, class EDT>
TreeNode<NDT, EDT> const& TreeEdge<NDT, EDT>::secondary_node() const
{
    return link_s_->node();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

template <class NDT, class EDT>
TreeEdge<NDT, EDT>& TreeEdge<NDT, EDT>::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeEdge<NDT, EDT>& TreeEdge<NDT, EDT>::reset_primary_link( LinkType* val )
{
    link_p_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeEdge<NDT, EDT>& TreeEdge<NDT, EDT>::reset_secondary_link( LinkType* val )
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
template <class NDT, class EDT>
std::string TreeEdge<NDT, EDT>::dump() const
{
    return "Node P: " + primary_node().data.name + ", Node S: " + secondary_node().data.name + "\t" + data.dump();
}

} // namespace tree
} // namespace genesis
