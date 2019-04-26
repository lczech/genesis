#ifndef GENESIS_TREE_TREE_SUBTREE_H_
#define GENESIS_TREE_TREE_SUBTREE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/tree.hpp"

namespace genesis {
namespace tree {

// =============================================================================
//     Subtree
// =============================================================================

/**
 * @brief Reference to a subtree of a Tree.
 *
 * This class refers to a particular subtree of a Tree.
 * Such a subtree contains all TreeNode%s and TreeEdge%s at one side of a TreeEdge,
 * while leaving out the other side of it.
 *
 * The "subtree" of a leaf node is only that node itself. There are no empty subtrees,
 * and there is no subtree that contains all of the Tree. That is, a Subtree is always a split
 * (or bipartition) of the Tree into two parts, separated by an edge.
 *
 * A subtree is defined by the TreeLink that is part of the subtree,
 * while its @link TreeLink::outer() outer()@endlink link is not:
 * ![Subtree given by a TreeLink.](tree/subtree.png)
 *
 * In the example, parts `A` and `B`, as well as the node on the left, belong to the Subtree,
 * and parts `C` and `D`, as well as the node on the right do not.
 *
 * The marked TreeLink, as well as its TreeNode and TreeEdge can be used to construct a Subtree
 * and can be retrieved from it.
 * The Tree that a Subtree refers to has to stay alive, otherwise we get dangling pointers.
 * It is the responsibility of the user to ensure this.
 *
 * The Tree iterators (e.g., IteratorPreorder and IteratorPostorder) furthermore offer
 * constructors that take a Subtree and only iterate this part of the Tree.
 */
class Subtree
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    // /* *
    //  * @ brief Default construct a subtree that does .
    //  */
    // Subtree() = default;

    /**
     * @brief Construct a Subtree that contains all of the tree except for the part that the
     * @link TreeLink::outer() outer()@endlink link of the given TreeLink belongs to.
     */
    explicit Subtree( TreeLink const& link )
        : link_( &link )
    {}

    /**
     * @brief Construct a Subtree representing the given TreeNode and the part of the Tree
     * away from its root node.
     */
    explicit Subtree( TreeNode const& node )
        : link_( &node.primary_link() )
    {}

    /**
     * @brief Construct a Subtree in one of the two directions of a given TreeEdge.
     *
     * With @p use_primary set to `false` (default), the subtree is the one pointing away from the
     * root. With @p use_primary set to `true`, the root side of the edge is used as subtree instead.
     */
    Subtree( TreeEdge const& edge, bool use_primary = false )
        : link_( use_primary ? &edge.primary_link() : &edge.secondary_link() )
    {}

    ~Subtree() = default;

    Subtree( Subtree const& ) = default;
    Subtree( Subtree&& )      = default;

    Subtree& operator= ( Subtree const& ) = default;
    Subtree& operator= ( Subtree&& )      = default;

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    /**
     * @brief Get the TreeLink that separates the subtree from the rest of the tree.
     *
     * This link belongs to the subtree, while its outer() does not.
     */
    TreeLink const& link() const
    {
        return *link_;
    }

    /**
     * @brief Get the TreeNode at which the subtree starts.
     */
    TreeNode const& node() const
    {
        return link_->node();
    }

    /**
     * @brief Get the TreeEdge that separates the subtree from the rest of the tree.
     */
    TreeEdge const& edge() const
    {
        return link_->edge();
    }

    /**
     * @brief Invert/flip the subtree to represent the other (complementary) part of the tree.
     */
    void invert()
    {
        link_ = &link_->outer();
    }

    // /**
    //  * @brief Return whether this subtree points to something, or was default constructed.
    //  */
    // operator bool() const
    // {
    //     return link_ == nullptr;
    // }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    TreeLink const*  link_ = nullptr;

};

} // namespace tree
} // namespace genesis

#endif // include guard
