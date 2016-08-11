#ifndef GENESIS_TREE_ITERATOR_PATH_H_
#define GENESIS_TREE_ITERATOR_PATH_H_

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
//     Path Iterator
// =================================================================================================

template <typename LinkType, typename NodeType, typename EdgeType>
class IteratorPath
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorPath<LinkType, NodeType, EdgeType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPath()
        : start_(  nullptr )
        , finish_( nullptr )
        , lca_(    nullptr )
        , reverse_path_()
    {}

    IteratorPath( NodeType& start, NodeType& finish )
        : IteratorPath( start.link(), finish.link() )
    {}

    IteratorPath( LinkType& start, LinkType& finish )
        : start_(  &start.node().primary_link()  )
        , finish_( &finish.node().primary_link() )
        , lca_(    nullptr  )
        , reverse_path_()
    {
        // In this constructor, we find and store the primary links of the nodes on the path.
        // Then, when moving along the path with this iterator (operator++), we simply need to
        // move along that list of links.
        // Furthermore, by using the primary links of the nodes (that is, the ones pointing towards
        // the root), we can easily spot the last common ancestor (LCA) of the start and finish
        // node.

        // TODO there is a lot of code duplication between this and the lowest common ancestor
        // TODO function. this is bad and could be resolved some how...

        // Helper function to find all links between a given link and the root.
        auto path_to_root = [] ( LinkType* link ) {
            std::vector<LinkType*> path;

            // Move towards the root and record all links in between.
            LinkType* cur_link = link;
            while( &cur_link->edge().secondary_link() == cur_link ) {

                // The above while condition means: is it the root?! Assert, that the default way of
                // checking for the root by using the node gives the same result.
                assert( ! cur_link->node().is_root() );

                // Assert that the primary direction is correct.
                assert( cur_link == &cur_link->edge().secondary_link() );

                // Add the primary link of the current node to the list.
                path.push_back( cur_link );

                // Move one node towards the root.
                // Assert that the default way of finding the next node towards the root (by using
                // the edge) gives the same result as simply using the link's outer node.
                // This is the case because the cur link is the one that points towards the root
                // (which was asserted above).
                assert( &cur_link->edge().primary_link() == &cur_link->outer() );
                cur_link = &cur_link->outer().node().primary_link();
            }

            // Now finally add the root itself and return the list.
            assert( cur_link->node().is_root() );
            path.push_back( cur_link );
            return path;
        };

        // Treat special case start == finish.
        // That makes sure that we do not need to specially check for an empty path later.
        if( start_ == finish_ ) {
            reverse_path_.push_back( start_ );
            lca_ = start_;
            return;
        }

        // Get paths to root for both links.
        auto start_path  = path_to_root( start_  );
        auto finish_path = path_to_root( finish_ );

        // We must have at least the two original links in the front and the root in the back.
        assert( start_path.size() > 0 && finish_path.size() > 0 );
        assert( start_path.front()    == start_  );
        assert( finish_path.front()   == finish_ );
        assert( start_path.back()     == finish_path.back() );

        // Remove from back as long as the last two elements are the same.
        // At the end of this, the remaining links are the ones on the path between
        // the two original links.
        while(
            start_path.size()  > 1 &&
            finish_path.size() > 1 &&
            start_path.at( start_path.size() - 1 ) == finish_path.at( finish_path.size() - 1 ) &&
            start_path.at( start_path.size() - 2 ) == finish_path.at( finish_path.size() - 2 )
        ) {
            start_path.pop_back();
            finish_path.pop_back();
        }

        // Now, the last elements need to be the same (the LCA of the start and finish node).
        assert( start_path.size() > 0 && finish_path.size() > 0 );
        assert( start_path.back()     == finish_path.back() );

        // The LCA (last common ancestor) is the node that both paths have in common. Store it.
        lca_ = start_path.back();

        // We store the path backwards, because removing from a vector's end is faster.
        // Thus, first add the path from finish to root/LCA, then from root/LCA to start (reversed).
        // Also, remove the root/LCA once, otherwise, it would appear twice, as it is in both lists.
        reverse_path_ = std::move( finish_path );
        reverse_path_.pop_back();
        reverse_path_.insert( reverse_path_.end(), start_path.rbegin(), start_path.rend() );

        assert( reverse_path_.front() == finish_ );
        assert( reverse_path_.back()  == start_  );
    }

    ~IteratorPath() = default;

    IteratorPath( IteratorPath const& ) = default;
    IteratorPath( IteratorPath&& )      = default;

    IteratorPath& operator= ( IteratorPath const& ) = default;
    IteratorPath& operator= ( IteratorPath&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        // If there is more than one element in the path, move to the next one.
        if( reverse_path_.size() > 1 ) {
            reverse_path_.pop_back();

        // If there is only one element left (which was just visited), we are done.
        } else {
            start_  = nullptr;
            finish_ = nullptr;
            lca_    = nullptr;
            reverse_path_.clear();
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
        return other.start_               == start_ &&
               other.finish_              == finish_ &&
               other.reverse_path_.size() == reverse_path_.size();
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    /**
     * @brief Return whether the current iterator position (node) is the last common ancestor of
     * the two start and finish nodes.
     *
     * This is useful in many cases:
     *
     *   * Find the LCA (obviously).
     *   * Check when the path is moving away from the root again.
     *   * Iterating edges instead of nodes.
     *
     * The last bullet point may need some explanation:
     *
     * The iterator visits all nodes between the start and the finish (both included).
     * On the path between them, there is however one edge fewer than the number of visited nodes.
     * That means, if you want to visit each *edge* on the path between two nodes
     * (instead of each *node*), you need a way to spot this superflous edge.
     * This function indicates the edge that needs to be skipped in this case.
     */
    bool is_last_common_ancestor() const
    {
        return reverse_path_.back() == lca_;
    }

    /**
     * @brief Alias for is_last_common_ancestor(). See there for more information.
     */
    bool is_lca() const
    {
        return is_last_common_ancestor();
    }

    LinkType& link() const
    {
        return *reverse_path_.back();
    }

    NodeType& node() const
    {
        return reverse_path_.back()->node();
    }

    EdgeType& edge() const
    {
        return reverse_path_.back()->edge();
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

private:

    LinkType*              start_;
    LinkType*              finish_;
    LinkType*              lca_;

    // Store the path between the finish and the start (thus, reversed). We do it this way
    // as we can then simply pop elements from the vector's end while iterating, which is fast.
    // All links stored in this vector are the primary links of their nodes (that is, they
    // are pointing towards the root).
    std::vector<LinkType*> reverse_path_;

};

// =================================================================================================
//     Path Wrapper Functions
// =================================================================================================

template<typename ElementType>
utils::Range< IteratorPath< TreeLink const, TreeNode const, TreeEdge const >>
path( ElementType const& start, ElementType const& finish )
{
    return {
        IteratorPath< const TreeLink, const TreeNode, const TreeEdge >( start, finish ),
        IteratorPath< const TreeLink, const TreeNode, const TreeEdge >()
    };
}

template<typename ElementType>
utils::Range< IteratorPath< TreeLink, TreeNode, TreeEdge >>
path( ElementType& start, ElementType& finish )
{
    return {
        IteratorPath< TreeLink, TreeNode, TreeEdge >( start, finish ),
        IteratorPath< TreeLink, TreeNode, TreeEdge >()
    };
}

} // namespace tree
} // namespace genesis

#endif // include guard
