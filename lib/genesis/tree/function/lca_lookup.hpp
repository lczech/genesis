#ifndef GENESIS_TREE_FUNCTION_LCA_LOOKUP_H_
#define GENESIS_TREE_FUNCTION_LCA_LOOKUP_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/utils/math/range_minimum_query.hpp"

#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;

// =================================================================================================
//     LCA Lookup
// =================================================================================================

/**
 * @brief Fast lookup of the lowest common ancestor (LCA) of two TreeNode%s, relative to an
 * arbitrary root node.
 *
 * The class offers to look up the LCA of two TreeNode%s. It can use the root node of the Tree as
 * base to consider which nodes are "lower", or any arbitrary other node of the tree.
 * See operator() for the lookup functions.
 *
 * Internally, the class uses a @link utils::RangeMinimumQuery RangeMinimumQuery@endlink (RMQ) and
 * indices of nodes during an eulertour() of the tree. This makes it fast even for large trees.
 *
 * Caveat: The Tree object is referenced from inside this class. Its livetime thus needs to be
 * longer than an instance of this class.
 */
class LcaLookup
{
public:

    // -------------------------------------------------------------------------
    //     Construction and Rule of Five
    // -------------------------------------------------------------------------

    LcaLookup() = default;
    explicit LcaLookup( Tree const& tree );

    ~LcaLookup() = default;

    LcaLookup( LcaLookup const& other ) = default;
    LcaLookup( LcaLookup&& )            = default;

    LcaLookup& operator= ( LcaLookup const& other ) = default;
    LcaLookup& operator= ( LcaLookup&& )            = default;

    // -------------------------------------------------------------------------
    //     Lookup
    // -------------------------------------------------------------------------

    std::size_t operator()( size_t node_index_a, size_t node_index_b, size_t root_index ) const;
    TreeNode const& operator()( TreeNode const& node_a, TreeNode const& node_b, TreeNode const& root_node ) const;

    size_t operator()( size_t node_index_a, size_t node_index_b ) const;
    TreeNode const& operator()( TreeNode const& node_a, TreeNode const& node_b ) const;

    // -------------------------------------------------------------------------
    //     Internal Helper Functions
    // -------------------------------------------------------------------------

private:

    void init_( Tree const& tree );

    /**
     * @brief Find index of the smallest entry between position @p i and @p j in the eulertour levels.
     *
     * Query the RangeMinimumQuery to find the index of the smallest entry between
     * positions i and j in the eulertour levels. This returns the index of the lowest common ancestor
     * of the nodes with indices i and j, in respect to the tree's root node.
     */
    size_t eulertour_query_( size_t i, size_t j ) const;

    /**
     * @brief Return the index of the lowest common ancestor (LCA) of the nodes at indices
     * @p node_index_a and @p node_index_a, using @p root_index as the root node.
     */
    size_t lookup_( size_t node_index_a, size_t node_index_b, size_t root_index ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    /**
     * @brief RMQ helper struct that enables to find the smallest entry between two
     * positions in an eulertour of the tree.
     */
    utils::RangeMinimumQuery eulertour_rmq_;

    /**
     * @brief Indices of nodes in the order of an eulertour of the tree.
     */
    std::vector<size_t> eulertour_order_;

    /**
     * @brief The first occurence of each node (by index) in an eulertour of the tree.
     */
	std::vector<size_t> eulertour_first_occurrence_;

    Tree const& tree_;
    size_t      root_idx_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
