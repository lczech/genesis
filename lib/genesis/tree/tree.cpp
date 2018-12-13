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
 * @brief Implementation of Tree class.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>

namespace genesis {
namespace tree {

// =================================================================================================
//     Construction and Rule of Five
// =================================================================================================

static_assert( std::is_move_constructible<Tree>::value, "Tree is not move constructible." );
static_assert( std::is_move_assignable<Tree>::value, "Tree is not move assignable." );

Tree::Tree( const Tree& other )
{
    // Get a copy of the topology. (Copy swap idiom.)
    auto res = other.clone_topology();

    // Copy data.
    for( size_t i = 0; i < res.links_.size(); ++i ) {
        // res.links_[i]->data = other.links_[i]->data;
    }
    for( size_t i = 0; i < res.nodes_.size(); ++i ) {
        if( other.node_at(i).has_data() ) {
            res.node_at(i).reset_data( other.node_at(i).data_ptr()->clone() );
        }

        // Some assertions.
        if( res.node_at(i).has_data() && other.node_at(i).has_data() ) {
            auto const& res_data = *res.node_at(i).data_ptr();
            auto const& oth_data = *other.node_at(i).data_ptr();
            (void) res_data;
            (void) oth_data;
            assert( typeid( res_data ) == typeid( oth_data ));
        } else {
            assert( ! res.node_at(i).has_data() && ! other.node_at(i).has_data() );
        }
    }
    for( size_t i = 0; i < res.edges_.size(); ++i ) {
        if( other.edge_at(i).has_data() ) {
            res.edge_at(i).reset_data( other.edge_at(i).data_ptr()->clone() );
        }

        // Some assertions.
        if( res.edges_[i]->has_data() && other.edge_at(i).has_data() ) {
            auto const& res_data = *res.edge_at(i).data_ptr();
            auto const& oth_data = *other.edge_at(i).data_ptr();
            (void) res_data;
            (void) oth_data;
            assert( typeid( res_data ) == typeid( oth_data ));
        } else {
            assert( ! res.edge_at(i).has_data() && ! other.edge_at(i).has_data() );
        }
    }

    // Swap with the (currently empty) content of this tree.
    res.swap( *this );
}

Tree& Tree::operator = ( Tree const& other )
{
    // Check for self assignment. Just in case.
    if( &other == this ) {
        return *this;
    }

    // Copy-swap-idiom.
    Tree temp( other );
    temp.swap( *this );
    return *this;
}

Tree Tree::clone_topology() const
{
    // Prepare resulting tree.
    auto res = Tree();
    res.links_.resize( link_count() );
    res.nodes_.resize( node_count() );
    res.edges_.resize( edge_count() );

    // Create all objects. We need two loops per array, because the pointers have to exist
    // in order to be linked to each other.
    for( size_t i = 0; i < links_.size(); ++i ) {
        res.links_[i] = utils::make_unique< TreeLink >();
    }
    for( size_t i = 0; i < nodes_.size(); ++i ) {
        res.nodes_[i] = utils::make_unique< TreeNode >();
    }
    for( size_t i = 0; i < edges_.size(); ++i ) {
        res.edges_[i] = utils::make_unique< TreeEdge >();
    }

    // Set all pointers for the topology in a second round of loops.
    for( size_t i = 0; i < links_.size(); ++i ) {
        auto const& cur_link = link_at(i);
        assert( cur_link.index() == i );

        res.links_[i]->reset_index( i );
        res.links_[i]->reset_next(  res.links_[ cur_link.next().index()  ].get() );
        res.links_[i]->reset_outer( res.links_[ cur_link.outer().index() ].get() );
        res.links_[i]->reset_node(  res.nodes_[ cur_link.node().index()  ].get() );
        res.links_[i]->reset_edge(  res.edges_[ cur_link.edge().index()  ].get() );
    }
    for( size_t i = 0; i < nodes_.size(); ++i ) {
        auto const& cur_node = node_at(i);
        assert( cur_node.index() == i );

        res.nodes_[i]->reset_index( i );
        res.nodes_[i]->reset_primary_link( res.links_[ cur_node.link().index() ].get() );
    }
    for( size_t i = 0; i < edges_.size(); ++i ) {
        auto const& cur_edge = edge_at(i);
        assert( cur_edge.index() == i );

        res.edges_[i]->reset_index( i );
        res.edges_[i]->reset_primary_link(   res.links_[ cur_edge.primary_link().index()   ].get() );
        res.edges_[i]->reset_secondary_link( res.links_[ cur_edge.secondary_link().index() ].get() );
    }

    // Don't forget to set the root link.
    res.root_link_ = res.links_[ root_link_->index() ].get();
    return res;
}

void Tree::swap( Tree& other )
{
    using std::swap;

    swap( root_link_, other.root_link_ );

    swap( links_, other.links_ );
    swap( nodes_, other.nodes_ );
    swap( edges_, other.edges_ );
}

void Tree::clear()
{
    root_link_ = nullptr;
    links_.clear();
    nodes_.clear();
    edges_.clear();
}

// =================================================================================================
//     Data Accessors
// =================================================================================================

Tree& Tree::reset_root_link( TreeLink* root_link )
{
    assert( root_link->index() < links_.size() );
    assert( links_[root_link->index()].get() == root_link );
    root_link_ = root_link;
    return *this;
}

Tree::LinkContainerType& Tree::expose_link_container()
{
    return links_;
}

Tree::NodeContainerType& Tree::expose_node_container()
{
    return nodes_;
}

Tree::EdgeContainerType& Tree::expose_edge_container()
{
    return edges_;
}

} // namespace tree
} // namespace genesis
