#ifndef GENESIS_TREE_TREE_LINK_H_
#define GENESIS_TREE_TREE_LINK_H_

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

#include <cassert>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class Tree;
class TreeEdge;
class TreeNode;

// =================================================================================================
//     TreeLink
// =================================================================================================

class TreeLink
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    friend class Tree;

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TreeLink()
        : index_( 0       )
        , next_(  nullptr )
        , outer_( nullptr )
        , node_(  nullptr )
        , edge_(  nullptr )
    {}

    TreeLink( size_t index, TreeLink* next, TreeLink* outer, TreeNode* node, TreeEdge* edge )
        : index_( index )
        , next_(  next  )
        , outer_( outer )
        , node_(  node  )
        , edge_(  edge  )
    {}

    ~TreeLink() = default;

    // avoid copy constructor and assignment operator.
    // creating copies is maintained by Tree only.

    TreeLink( TreeLink const& ) = delete;
    TreeLink( TreeLink&& )      = delete;

    TreeLink& operator= ( TreeLink const& ) = delete;
    TreeLink& operator= ( TreeLink&& )      = delete;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Return the index of this Link.
     */
    size_t index() const
    {
        return index_;
    }

    /**
     * @brief Return the next TreeLink within the TreeNode of this link.
     */
    TreeLink& next()
    {
        return *next_;
    }

    /**
     * @brief Return the next TreeLink within the TreeNode of this link.
     */
    TreeLink const& next() const
    {
        return *next_;
    }

    /**
     * @brief Return the previous TreeLink within the TreeNode of this link.
     *
     * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
     * As this link first has to be found, this function is not as cheap as next().
     */
    TreeLink& prev()
    {
        TreeLink* res = this;
        while( &res->next() != this ) {
            res = &res->next();
        }
        assert( &res->next() == this );
        return *res;
    }

    /**
     * @brief Return the previous TreeLink within the TreeNode of this link.
     *
     * The previous link of a given link `L` is the one whose next-pointer is pointing to `L`.
     * As this link first has to be found, this function is not as cheap as next().
     */
    TreeLink const& prev() const
    {
        TreeLink const* res = this;
        while( &res->next() != this ) {
            res = &res->next();
        }
        assert( &res->next() == this );
        return *res;
    }

    /**
     * @brief Return the TreeLink of the adjacent TreeNode.
     */
    TreeLink& outer()
    {
        return *outer_;
    }

    /**
     * @brief Return the TreeLink of the adjacent TreeNode.
     */
    TreeLink const& outer() const
    {
        return *outer_;
    }

    /**
     * @brief Return the TreeEdge of this TreeLink.
     */
    TreeEdge& edge()
    {
        return *edge_;
    }

    /**
     * @brief Return the TreeEdge of this TreeLink.
     */
    TreeEdge const& edge() const
    {
        return *edge_;
    }

    /**
     * @brief Return the TreeNode of this TreeLink.
     */
    TreeNode& node()
    {
        return *node_;
    }

    /**
     * @brief Return the TreeNode of this TreeLink.
     */
    TreeNode const& node() const
    {
        return *node_;
    }

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    /**
     * @brief Reset the internal index of this TreeLink.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     * The index is an invariant that needs to be kept, as it needs to match the index in the Tree
     * container.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeLink& reset_index( size_t val )
    {
        index_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the next TreeLink of this TreeLink.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeLink& reset_next(  TreeLink* val )
    {
        next_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the outer TreeLink of this TreeLink.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeLink& reset_outer( TreeLink* val )
    {
        outer_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the TreeNode of this TreeLink.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeLink& reset_node( TreeNode* val )
    {
        node_ = val;
        return *this;
    }

    /**
     * @brief Reset the internal pointer to the TreeEdge of this TreeLink.
     *
     * This is a helper function that needs to be used with care and only in cases where appropriate.
     *
     * This function exists to allow building and modifying a Tree without the need for many friend
     * declarations. However, the function should rarely be needed outside of this context.
     */
    TreeLink& reset_edge( TreeEdge* val )
    {
        edge_ = val;
        return *this;
    }

    // ---------------------------------------------------------------------
    //     Member Variables
    // ---------------------------------------------------------------------

private:

    size_t index_;

    TreeLink* next_;
    TreeLink* outer_;

    TreeNode* node_;
    TreeEdge* edge_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
