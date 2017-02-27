/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/tree/default/tree.hpp"
#include "genesis/utils/core/logging.hpp"

#include <assert.h>
#include <stdexcept>
#include <typeinfo>

namespace genesis {
namespace tree {

// =================================================================================================
//     Construction and Rule of Five
// =================================================================================================

/**
 * @brief Copy constructor.
 *
 * This function creates all links, nodes and edges new, and shapes them so that the final
 * Tree has the same topology as the input Tree.
 *
 * The data belonging to the edges and nodes is copied using the clone function of the respective
 * data classes for the nodes and edges. As this data might contain pointers and
 * other structures that need a deep copy, it is the responsibility of the clone function of those
 * data classes to make sure its own data is copied correctly.
 *
 * This function internally uses clone_topology() first, and then uses the clone functions of the
 * data for all nodes and edges.
 */
Tree::Tree( const Tree& other )
{
    // Get a copy of the topology. (Copy swap idiom.)
    auto res = other.clone_topology();

    // Instead of using non-covariant clone functions for the data, this might be an option:
    // http://stackoverflow.com/questions/6924754/return-type-covariance-with-smart-pointers
    // The typeid assertion could then also move into the base data classes, like this:
    // http://stackoverflow.com/questions/9477581/force-all-classes-to-implement-override-a-pure-virtual-method-in-multi-level

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

/**
 * @brief Assignment operator.
 *
 * See Tree copy constructor for more information.
 */
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

/**
 * @brief Return a Tree with the same topology, but without any data.
 *
 * All data pointers of the nodes and edges of the returned tree are `nullptr`.
 */
Tree Tree::clone_topology() const
{
    // Prepare resulting tree.
    auto res = Tree();
    res.links_.resize( link_count() );
    res.nodes_.resize( node_count() );
    res.edges_.resize( edge_count() );
    res.root_link_index_ = root_link_index_;

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

    return res;
}

/**
 * @brief Swap.
 */
void Tree::swap( Tree& other )
{
    using std::swap;

    swap( root_link_index_, other.root_link_index_ );

    swap( links_, other.links_ );
    swap( nodes_, other.nodes_ );
    swap( edges_, other.edges_ );
}

/**
 * @brief Deletes all data of the tree, including all links, nodes and edges.
 *
 * This functions results in an empty tree.
 */
void Tree::clear()
{
    root_link_index_ = 0;
    links_.clear();
    nodes_.clear();
    edges_.clear();
}

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return whether the Tree is empty (i.e., has no nodes, edges and links).
 */
bool Tree::empty() const
{
    return links_.empty() && nodes_.empty() && edges_.empty();
}

/**
 * @brief Return the TreeLink at the current root of the Tree.
 *
 * If the tree is empty(), the functions throws an std::out_of_range exception.
 */
TreeLink& Tree::root_link()
{
    if( links_.empty() ) {
        throw std::out_of_range( "Cannot return root link. Tree is empty." );
    }
    return *links_.at( root_link_index_ ).get();
}

/**
 * @brief Return the TreeLink at the current root of the Tree.
 *
 * If the tree is empty(), the functions throws an std::out_of_range exception.
 */
TreeLink const& Tree::root_link() const
{
    if( links_.empty() ) {
        throw std::out_of_range( "Cannot return root link. Tree is empty." );
    }
    return *links_.at( root_link_index_ ).get();
}

/**
 * @brief Return the TreeNode at the current root of the Tree.
 *
 * If the tree is empty(), the functions throws an std::out_of_range exception.
 */
TreeNode& Tree::root_node()
{
    if( links_.empty() ) {
        throw std::out_of_range( "Cannot return root node. Tree is empty." );
    }
    return links_.at( root_link_index_ )->node();
}

/**
 * @brief Return the TreeNode at the current root of the Tree.
 *
 * If the tree is empty(), the functions throws an std::out_of_range exception.
 */
TreeNode const& Tree::root_node() const
{
    if( links_.empty() ) {
        throw std::out_of_range( "Cannot return root node. Tree is empty." );
    }
    return links_.at( root_link_index_ )->node();
}

/**
 * @brief Return the TreeLink at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeLink& Tree::link_at(size_t index)
{
    return *links_.at(index).get();
}

/**
 * @brief Return the TreeLink at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeLink const& Tree::link_at(size_t index) const
{
    return *links_.at(index).get();
}

/**
 * @brief Return the TreeNode at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeNode& Tree::node_at(size_t index)
{
    return *nodes_.at(index).get();
}

/**
 * @brief Return the TreeNode at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeNode const& Tree::node_at(size_t index) const
{
    return *nodes_.at(index).get();
}

/**
 * @brief Return the TreeEdge at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeEdge& Tree::edge_at(size_t index)
{
    return *edges_.at(index).get();
}

/**
 * @brief Return the TreeEdge at a certain index.
 *
 * If the index is invalid, the functions throws an std::out_of_range exception.
 */
TreeEdge const& Tree::edge_at(size_t index) const
{
    return *edges_.at(index).get();
}

/**
 * @brief Return the number of TreeLink%s of the Tree.
 */
size_t Tree::link_count() const
{
    return links_.size();
}

/**
 * @brief Return the number of TreeNode%s of the Tree.
 */
size_t Tree::node_count() const
{
    return nodes_.size();
}

/**
 * @brief Return the number of TreeEdge%s of the Tree.
 */
size_t Tree::edge_count() const
{
    return edges_.size();
}

// =================================================================================================
//     Data Accessors
// =================================================================================================

/**
 * @brief Reset the index of the link that is considered to be the root of the Tree.
 *
 * This function is meant for tree manipulation functions. Use with care!
 *
 * Caveat: This function simply sets the index, but does not change any other properties of the tree.
 * Particularly the correct primary/secondary order of TreeEdge%s and primary links of TreeNode%s
 * needs to be maintained manually when using this function! Otherwise, we end up with an invalid
 * Tree that breaks its invariants!
 */
Tree& Tree::reset_root_link_index( size_t val )
{
    if( val >= links_.size() ) {
        throw std::runtime_error( "Invalid root link index: " + std::to_string( val ) );
    }
    root_link_index_ = val;
    return *this;
}

/**
 * @brief Get the container that stores all TreeLink%s of the Tree.
 *
 * This function gives direct access to the underlying container for the Links. It is meant to
 * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
 * the tree, or for adding elements to it. Use with care!
 */
Tree::LinkContainerType& Tree::expose_link_container()
{
    return links_;
}

/**
 * @brief Get the container that stores all TreeNode%s of the Tree.
 *
 * This function gives direct access to the underlying container for the Links. It is meant to
 * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
 * the tree, or for adding elements to it. Use with care!
 */
Tree::NodeContainerType& Tree::expose_node_container()
{
    return nodes_;
}

/**
 * @brief Get the container that stores all TreeEdge%s of the Tree.
 *
 * This function gives direct access to the underlying container for the Links. It is meant to
 * be used by external functions that need to manupulate the Tree, e.g., for reading or re-rooting
 * the tree, or for adding elements to it. Use with care!
 */
Tree::EdgeContainerType& Tree::expose_edge_container()
{
    return edges_;
}

// =================================================================================================
//     Iterators
// =================================================================================================

// -------------------------------------------------------------------------
//     Links
// -------------------------------------------------------------------------

Tree::IteratorLinks Tree::begin_links()
{
    return links_.begin();
}

Tree::IteratorLinks Tree::end_links()
{
    return links_.end();
}

Tree::ConstIteratorLinks Tree::begin_links() const
{
    return links_.cbegin();
}

Tree::ConstIteratorLinks Tree::end_links() const
{
    return links_.cend();
}

utils::Range<Tree::IteratorLinks> Tree::links()
{
    return { links_ };
}

utils::Range<Tree::ConstIteratorLinks> Tree::links() const
{
    return { links_ };
}

// -------------------------------------------------------------------------
//     Nodes
// -------------------------------------------------------------------------

Tree::IteratorNodes Tree::begin_nodes()
{
    return nodes_.begin();
}

Tree::IteratorNodes Tree::end_nodes()
{
    return nodes_.end();
}

Tree::ConstIteratorNodes Tree::begin_nodes() const
{
    return nodes_.cbegin();
}

Tree::ConstIteratorNodes Tree::end_nodes() const
{
    return nodes_.cend();
}

utils::Range<Tree::IteratorNodes> Tree::nodes()
{
    return { nodes_ };
}

utils::Range<Tree::ConstIteratorNodes> Tree::nodes() const
{
    return { nodes_ };
}

// -------------------------------------------------------------------------
//     Edges
// -------------------------------------------------------------------------

Tree::IteratorEdges Tree::begin_edges()
{
    return edges_.begin();
}

Tree::IteratorEdges Tree::end_edges()
{
    return edges_.end();
}

Tree::ConstIteratorEdges Tree::begin_edges() const
{
    return edges_.cbegin();
}

Tree::ConstIteratorEdges Tree::end_edges() const
{
    return edges_.cend();
}

utils::Range<Tree::IteratorEdges> Tree::edges()
{
    return { edges_ };
}

utils::Range<Tree::ConstIteratorEdges> Tree::edges() const
{
    return { edges_ };
}

} // namespace tree
} // namespace genesis
