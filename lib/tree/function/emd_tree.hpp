#ifndef GENESIS_TREE_FUNCTION_EMD_TREE_H_
#define GENESIS_TREE_FUNCTION_EMD_TREE_H_

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
#include "tree/default/tree.hpp"

#include <map>

namespace genesis {
namespace tree {

// =============================================================================
//     Typedefs
// =============================================================================

/**
 * @brief Alias for a Tree used to calculate the earth movers distance between to sets of masses
 * distributed on a Tree.
 *
 * See earth_movers_distance( EmdTree const& tree ) for more details on the purpose of this tree
 * type and on the earth movers distance in general.
 *
 * The branches of an EmdTree hold a list of @link EmdEdgeData::masses masses@endlink, sorted
 * along their position on the branch.
 */
using EmdTree = Tree;

/**
 * @brief Alias for a TreeEdge of an @link EmdTree EmdTree@endlink. See there for more information.
 */
using EmdTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of an @link EmdTree EmdTree@endlink. See there for more information.
 */
using EmdTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of an @link EmdTree EmdTree@endlink. See there for more information.
 */
using EmdTreeNode = TreeNode;

// =============================================================================
//     Emd Tree Node Data
// =============================================================================

/**
 * @brief Data class for @link EmdTreeNode EmdTreeNodes@endlink. Stores nothing.
 *
 * See @link EmdTree EmdTree@endlink for more information.
 */
class EmdNodeData : public BaseNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~EmdNodeData() = default;

    // Move ctor and assignment.
    EmdNodeData( EmdNodeData&& )             = delete;
    EmdNodeData& operator= ( EmdNodeData&& ) = delete;

protected:

    EmdNodeData() = default;

    // Copy ctor and assignment.
    EmdNodeData( EmdNodeData const& )             = default;
    EmdNodeData& operator= ( EmdNodeData const& ) = default;

public:

    static std::unique_ptr< EmdNodeData > create()
    {
        return std::unique_ptr< EmdNodeData >( new EmdNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< EmdNodeData >( new EmdNodeData( *this ));
    }

};

// =============================================================================
//     Emd Tree Edge Data
// =============================================================================

/**
 * @brief Data class for @link EmdTreeEdge EmdTreeEdges@endlink. Stores the branch length and
 * a list of masses with their positions along the edge.
 *
 * See @link EmdTree EmdTree@endlink for more information.
 */
class EmdEdgeData : public DefaultEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~EmdEdgeData() = default;

    // Move ctor and assignment.
    EmdEdgeData( EmdEdgeData&& )             = delete;
    EmdEdgeData& operator= ( EmdEdgeData&& ) = delete;

protected:

    EmdEdgeData() = default;

    // Copy ctor and assignment.
    EmdEdgeData( EmdEdgeData const& )             = default;
    EmdEdgeData& operator= ( EmdEdgeData const& ) = default;

public:

    static std::unique_ptr< EmdEdgeData > create()
    {
        return std::unique_ptr< EmdEdgeData >( new EmdEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< EmdEdgeData >( new EmdEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief List of masses stored on this branch, sorted by their position on the branch.
     *
     * This data member maps from a position on the branch to the mass at that position.
     * In order to be valid, the positions have to be in the interval [0.0, branch_length].
     * See @link validate_emd_tree( EmdTree const& tree, double valid_total_mass_difference )
     * validate_emd_tree() @endlink for a validation function.
     */
    std::map<double, double> masses;

};

} // namespace tree
} // namespace genesis

#endif // include guard
