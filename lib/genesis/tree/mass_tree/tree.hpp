#ifndef GENESIS_TREE_MASS_TREE_TREE_H_
#define GENESIS_TREE_MASS_TREE_TREE_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"

#include <map>

namespace genesis {
namespace tree {

// =================================================================================================
//     Typedefs
// =================================================================================================

/**
 * @brief Alias for a Tree that stores masses on its TreeEdge%s.
 *
 * It is for example used to calculate the earth movers distance between to sets of masses
 * distributed on a Tree.
 *
 * See earth_movers_distance( MassTree const& tree ) for more details on the purpose of this tree
 * type and on the earth movers distance in general.
 *
 * The branches of an MassTree hold a list of @link MassTreeEdgeData::masses masses@endlink, sorted
 * along their position on the branch.
 *
 * It is easily possible to merge the masses of two MassTree%s by using mass_tree_merge_trees() or
 * mass_tree_merge_trees_inplace().
 *
 * Lastly, there are some some useful transformation functions:
 *
 *   * mass_tree_center_masses_on_branches()
 *   * mass_tree_transform_to_unit_branch_lengths()
 *   * mass_tree_reverse_signs()
 *   * mass_tree_clear_masses()
 *
 * See there for details.
 */
using MassTree = Tree;

/**
 * @brief Alias for a TreeEdge of a @link MassTree MassTree@endlink. See there for more information.
 */
using MassTreeEdge = TreeEdge;

/**
 * @brief Alias for a TreeLink of a @link MassTree MassTree@endlink. See there for more information.
 */
using MassTreeLink = TreeLink;

/**
 * @brief Alias for a TreeNode of a @link MassTree MassTree@endlink. See there for more information.
 */
using MassTreeNode = TreeNode;

// =================================================================================================
//     Mass Tree Node Data
// =================================================================================================

/**
 * @brief Data class for @link MassTreeNode MassTreeNodes@endlink. Stores nothing.
 *
 * See @link MassTree MassTree@endlink for more information.
 */
class MassTreeNodeData : public BaseNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~MassTreeNodeData() = default;

    // Move ctor and assignment.
    MassTreeNodeData( MassTreeNodeData&& )             = delete;
    MassTreeNodeData& operator= ( MassTreeNodeData&& ) = delete;

protected:

    MassTreeNodeData() = default;

    // Copy ctor and assignment.
    MassTreeNodeData( MassTreeNodeData const& )             = default;
    MassTreeNodeData& operator= ( MassTreeNodeData const& ) = default;

public:

    static std::unique_ptr< MassTreeNodeData > create()
    {
        return std::unique_ptr< MassTreeNodeData >( new MassTreeNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > recreate() const override
    {
        return std::unique_ptr< MassTreeNodeData >( new MassTreeNodeData() );
    }

    virtual std::unique_ptr< BaseNodeData > clone() const override
    {
        return std::unique_ptr< MassTreeNodeData >( new MassTreeNodeData( *this ));
    }

};

// =================================================================================================
//     Mass Tree Edge Data
// =================================================================================================

/**
 * @brief Data class for @link MassTreeEdge MassTreeEdges@endlink. Stores the branch length and
 * a list of masses with their positions along the edge.
 *
 * See @link MassTree MassTree@endlink for more information.
 */
class MassTreeEdgeData : public DefaultEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~MassTreeEdgeData() = default;

    // Move ctor and assignment.
    MassTreeEdgeData( MassTreeEdgeData&& )             = delete;
    MassTreeEdgeData& operator= ( MassTreeEdgeData&& ) = delete;

protected:

    MassTreeEdgeData() = default;

    // Copy ctor and assignment.
    MassTreeEdgeData( MassTreeEdgeData const& )             = default;
    MassTreeEdgeData& operator= ( MassTreeEdgeData const& ) = default;

public:

    static std::unique_ptr< MassTreeEdgeData > create()
    {
        return std::unique_ptr< MassTreeEdgeData >( new MassTreeEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > recreate() const override
    {
        return std::unique_ptr< MassTreeEdgeData >( new MassTreeEdgeData() );
    }

    virtual std::unique_ptr< BaseEdgeData > clone() const override
    {
        return std::unique_ptr< MassTreeEdgeData >( new MassTreeEdgeData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * @brief List of masses stored on this branch, sorted by their position on the branch.
     *
     * This data member maps from a position on the branch to the mass at that position.
     * In order to be valid, the positions have to be in the interval [0.0, branch_length].
     * See @link mass_tree_validate( MassTree const& tree, double valid_total_mass_difference )
     * mass_tree_validate() @endlink for a validation function.
     */
    std::map<double, double> masses;

};

// =================================================================================================
//     Default Converter
// =================================================================================================

/**
 * @brief Helper function that takes a DefaultTree (or any Tree with Node and Edge data derived
 * from it) and turns its data into an MassTree, that is, a Tree with
 * @link MassTreeNodeData MassTreeNodeData@endlink and
 * @link MassTreeEdgeData MassTreeEdgeData@endlink.
 */
inline MassTree convert_default_tree_to_mass_tree( DefaultTree const& source )
{
    return convert(
        source,
        [] ( tree::BaseNodeData const& node_data ) {
            (void) node_data;
            return tree::MassTreeNodeData::create();
        },
        [] ( tree::BaseEdgeData const& edge_data ) {
            auto mass_edge = tree::MassTreeEdgeData::create();
            auto const& orig_edge = dynamic_cast< DefaultEdgeData const& >( edge_data );
            mass_edge->branch_length = orig_edge.branch_length;

            return mass_edge;
        }
    );
}

} // namespace tree
} // namespace genesis

#endif // include guard
