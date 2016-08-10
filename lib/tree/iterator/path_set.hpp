#ifndef GENESIS_TREE_ITERATOR_PATH_SET_H_
#define GENESIS_TREE_ITERATOR_PATH_SET_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/tree.hpp"
#include "utils/core/range.hpp"

#include <assert.h>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Path Set Iterator
// =================================================================================================

/**
 * @brief Iterate the path between two TreeNode%s (non-linearily), given their lowest common
 * ancestor (LCA).
 *
 * This is a fast alternative to IteratorPath, with two differences.
 *
 * Firstly, this iterator needs to know the LCA of both nodes. This allows skipping its calculation,
 * for speed reasons. This is mainly useful if you maintain a list of the LCAs of pairs of nodes,
 * e.g., when using a RMQ data structure or the like. It is also also possible to calculate
 * the LCA, using lowest_common_ancestor(). In this case, however, using the IteratorPath is
 * probably the better and more straight forward option.
 *
 * Secondly, the path is not traversed from start to finish, but in the order:
 *
 *     [ start, lca ), [ finish, lca ]
 *
 * That is, starting at the `start` node, first we move up the Tree in the direction of the root
 * until we encounter the LCA. However, instead of visiting it immediately, we first go to the
 * `finish` node, move up again in the direction of the root, until we finally reach the LCA, and
 * this time also visit it.
 *
 * If you do not want to visit the LCA while iteration, use is_lca() to detect when the iterator
 * reaches the LCA, and simply skip it then.
 *
 * This iterator is mainly useful if the order of visiting nodes/edges is not imporant, e.g., if
 * you simply need to update some value for all of them, but not depending on their order.
 *
 * Remark: The iterator assumes that the provided LCA is correct. It is able to find some cases of
 * wrong LCAs, but not all of them. If the provided LCA is somewhere on the path between the actual
 * LCA and the root, this not detected and will lead to visiting those additional nodes.
 * In cases where a wrong LCA is detected, an exception is thrown.
 * We only use this simple error checking for speed reasons, as doing a full check would require
 * the extra amount of work that IteratorPath is doing. Thus, the user is responsible for providing
 * a correct LCA.
 */
template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorPathSet
{
public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorPathSet<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPathSet()
        : start_(  nullptr )
        , finish_( nullptr )
        , lca_(    nullptr )
        , link_(   nullptr )
    {}

    IteratorPathSet( NodeType& start, NodeType& finish, NodeType& lca )
        : IteratorPathSet( start.link(), finish.link(), lca.link() )
    {}

    IteratorPathSet( LinkType& start, LinkType& finish, LinkType& lca )
        : start_(  &start.node().primary_link()  )
        , finish_( &finish.node().primary_link() )
        , lca_(    &lca.node().primary_link()  )
        , link_(   start_  )
    {
        // Edge case: No real path.
        if( start_ == finish_ ) {
            // In this case, the LCA has also to be the same, otherwise it is a wrong LCA.
            if( lca_ != start_ ) {
                throw std::runtime_error(
                    "Invalid LCA provided for iterating a path with identical start/finish node."
                );
            }

            // We will just traverse one node, so start at the finish_ node,
            // and after one incrementation, we will be done.
            doing_first_part_ = false;
            return;

        // Special case: The start is also the LCA. In this case, we go immediately to the
        // second path (finish_ to lca_), and the LCA (= start) will be visited at the end.
        } else if( start_ == lca_ ) {
            doing_first_part_ = false;
            link_ = finish_;
        }
    }

    ~IteratorPathSet() = default;

    IteratorPathSet( IteratorPathSet const& ) = default;
    IteratorPathSet( IteratorPathSet&& )      = default;

    IteratorPathSet& operator= ( IteratorPathSet const& ) = default;
    IteratorPathSet& operator= ( IteratorPathSet&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        // If we are on the second path (from finish_ to lca_) ...
        if( ! doing_first_part_ ) {

            // ... and we reach the LCA, this means, we are completely done iterating, so stop here.
            if( link_ == lca_ ) {
                link_ = nullptr;
                return *this;
            }

            // ... and we reach the root, we have an error! The LCA cannot be more upwards than the
            // root, but even if the root was the LCA, we would have checked this in the above
            // condition. Thus, the LCA is wrong.
            if( link_->node().is_root() ) {
                throw std::runtime_error( "Found invalid LCA while iterating path." );
            }
        }

        // Go to the next node towards the LCA.
        link_ = &link_->outer().node().primary_link();

        // If we are on the first path (from start_ to lca_) ...
        if( doing_first_part_ ) {

            // ... and we reach the LCA, we are done with the first path.
            // Start again at finish_ for the second path.
            if( link_ == lca_ ) {
                doing_first_part_ = false;
                link_ = finish_;
                return *this;
            }

            // ... and we reach the root in the firth path, the LCA is wrong,
            // by the same reasoning as above.
            if( link_->node().is_root() ) {
                throw std::runtime_error( "Found invalid LCA while iterating path." );
            }
        }

        return *this;
    }

    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator == (const self_type &other) const
    {
        return other.link_ == link_;
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    bool is_last_common_ancestor() const
    {
        return link_ == lca_;
    }

    bool is_lca() const
    {
        return is_last_common_ancestor();
    }

    LinkType& link() const
    {
        return *link_;
    }

    NodeType& node() const
    {
        return link_->node();
    }

    EdgeType& edge() const
    {
        return link_->edge();
    }

    LinkType& start_link() const
    {
        return *start_;
    }

    NodeType& start_node() const
    {
        return start_->node();
    }

    LinkType& finish_link() const
    {
        return *finish_;
    }

    NodeType& finish_node() const
    {
        return finish_->node();
    }

    LinkType& lca_link() const
    {
        return *lca_;
    }

    NodeType& lca_node() const
    {
        return lca_->node();
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    LinkType* const        start_;
    LinkType* const        finish_;
    LinkType* const        lca_;

    LinkType*              link_;
    bool                   doing_first_part_ = true;
};

// =================================================================================================
//     Path Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorPathSet< TreeLink const, TreeNode const, TreeEdge const >>
path_set( ElementType const& start, ElementType const& finish, ElementType const& lca )
{
    return {
        IteratorPathSet< const TreeLink, const TreeNode, const TreeEdge >( start, finish, lca ),
        IteratorPathSet< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorPathSet< TreeLink, TreeNode, TreeEdge >>
path_set( ElementType& start, ElementType& finish, ElementType& lca )
{
    return {
        IteratorPathSet< TreeLink, TreeNode, TreeEdge >( start, finish, lca ),
        IteratorPathSet< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
