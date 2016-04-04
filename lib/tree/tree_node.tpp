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
 * @brief Implementation of TreeNode class template.
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
 * @brief Return the index of this Node.
 */
template <class NDT, class EDT>
size_t TreeNode<NDT, EDT>::index() const
{
    return index_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeNode<NDT, EDT>::primary_link()
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeNode<NDT, EDT>::primary_link() const
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 *
 * This is just an alias for primary_link(), that is shorter to use when needed
 * frequently in an algorithm.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT>& TreeNode<NDT, EDT>::link()
{
    return *link_;
}

/**
 * @brief Return the TreeLink that points towards the root.
 *
 * This is just an alias for primary_link(), that is shorter to use when needed
 * frequently in an algorithm.
 */
template <class NDT, class EDT>
TreeLink<NDT, EDT> const& TreeNode<NDT, EDT>::link() const
{
    return *link_;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

template <class NDT, class EDT>
TreeNode<NDT, EDT>& TreeNode<NDT, EDT>::reset_index( size_t val )
{
    index_ = val;
    return *this;
}

template <class NDT, class EDT>
TreeNode<NDT, EDT>& TreeNode<NDT, EDT>::reset_primary_link( LinkType* val )
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
template <class NDT, class EDT>
size_t TreeNode<NDT, EDT>::rank() const
{
    int rank = -1;
    TreeLink<NDT, EDT>* link = link_;

    do {
        ++rank;
        link = &link->next();
    } while( link != link_ );

    // We add at least 1 to the initial value of the rank, so this is valid.
    return static_cast<size_t>(rank);
}

// The following are definitions that would create circular dependecies when included in the
// class definition. Thus, they need to be here, after the definition, so that their dependend
// source files can be included without circles.
// See http://www.cplusplus.com/forum/articles/10627/ for more information on this.

// Usually, this part would be at the end of the header file, but as this is a class template,
// where the implementation (this file) is included from within the header anyway, we can as well
// outsource the functions to here.

/**
 * @brief True iff the node is a leaf/tip.
 */
template <class NDT, class EDT>
bool TreeNode<NDT, EDT>::is_leaf() const
{
    return link_->is_leaf();
}

/**
 * @brief True iff the node is an inner node.
 */
template <class NDT, class EDT>
bool TreeNode<NDT, EDT>::is_inner() const
{
    return link_->is_inner();
}

/**
 * @brief Returns a one-line dump summary of the data of this node.
 *
 * TODO this method assumes that the tree node has a name. not good.
 */
template <class NDT, class EDT>
std::string TreeNode<NDT, EDT>::dump() const
{
    return "Rank: " + std::to_string(rank()) + " \t " + "name: " + this->data.name;
}

} // namespace tree
} // namespace genesis
