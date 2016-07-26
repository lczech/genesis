#ifndef GENESIS_TREE_ITERATOR_reverse_path_H_
#define GENESIS_TREE_ITERATOR_reverse_path_H_

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
        , reverse_path_()
    {}

    IteratorPath( NodeType& start, NodeType& finish )
        : IteratorPath( start.link(), finish.link() )
    {}

    IteratorPath( LinkType& start, LinkType& finish )
        : start_(  &start  )
        , finish_( &finish )
        , reverse_path_()
    {
        // Helper function to find all links between a given link and the root.
        auto path_to_root = [] ( LinkType& link ) {
            std::vector<LinkType*> path;

            // Move towards the root and record all links in between.
            LinkType* cur_link = &link.node().primary_link();
            while( ! cur_link->node().is_root() ) {

                // Assert that the primary direction is correct.
                assert( cur_link == &cur_link->edge().secondary_link() );

                // Add the primary link of the current node to the list.
                path.push_back( cur_link );

                // Move one node towards the root.
                cur_link = &cur_link->edge().primary_link().node().primary_link();
            }

            // Now finally add the root itself and return the list.
            assert( cur_link->node().is_root() );
            path.push_back( cur_link );
            return path;
        };

        // Treat special case start == finish.
        // That makes sure that we do not need to specially check for an empty path later.
        if( &start == &finish ) {
            reverse_path_.push_back( &start );
            return;
        }

        // Get paths to root for both links.
        auto start_to_root  = path_to_root( start );
        auto finish_to_root = path_to_root( finish );

        // We must have at least the two original links in the front and the root in the back.
        assert( start_to_root.size() > 0 && finish_to_root.size() > 0 );
        assert( start_to_root.front()    == &start  );
        assert( finish_to_root.front()   == &finish );
        assert( start_to_root.back()     == finish_to_root.back() );

        // Remove from back as long as the last two elements are the same.
        // At the end of this, the remaining links are the ones on the path between
        // the two original links.
        while(
            start_to_root.size()  > 1 &&
            finish_to_root.size() > 1 &&
            start_to_root.at( start_to_root.size() - 1 ) == finish_to_root.at( finish_to_root.size() - 1 ) &&
            start_to_root.at( start_to_root.size() - 2 ) == finish_to_root.at( finish_to_root.size() - 2 )
        ) {
            start_to_root.pop_back();
            finish_to_root.pop_back();
        }

        // Now, the last elements need to be the same (the LCA of the start and finish node).
        assert( start_to_root.size() > 0 && finish_to_root.size() > 0 );
        assert( start_to_root.back()     == finish_to_root.back() );

        // We store the path backwards, because removing from a vector end is cheaper.
        // Thus, first add the path from finish to root/LCA, then from root/LCA to start (reversed).
        // Also, remove the root/LCA once, otherwise, it would appear twice, as it is in both lists.
        reverse_path_.insert( reverse_path_.end(), finish_to_root.begin(), finish_to_root.end() );
        reverse_path_.pop_back();
        reverse_path_.insert( reverse_path_.end(), start_to_root.rbegin(), start_to_root.rend() );

        assert( reverse_path_.front() == &finish );
        assert( reverse_path_.back()  == &start );
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
        if( reverse_path_.size() > 1 ) {
            reverse_path_.pop_back();
        } else {
            start_  = nullptr;
            finish_ = nullptr;
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
