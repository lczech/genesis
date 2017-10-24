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
 * @ingroup placement
 */

#include "genesis/placement/function/operators.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/sample.hpp"

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/function/tree_set.hpp"
#include "genesis/tree/mass_tree/functions.hpp"
#include "genesis/tree/mass_tree/tree.hpp"
#include "genesis/tree/printer/compact.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"

#include "genesis/utils/text/table.hpp"

#include <ostream>
#include <sstream>
#include <unordered_map>

namespace genesis {
namespace placement {

// =================================================================================================
//     Comparision and Equality
// =================================================================================================

bool compatible_trees( PlacementTree const& lhs, PlacementTree const& rhs )
{
    auto node_comparator = [] (
        PlacementTreeNode const& node_l,
        PlacementTreeNode const& node_r
    ) {
        auto l_ptr = dynamic_cast< PlacementNodeData const* >( node_l.data_ptr() );
        auto r_ptr = dynamic_cast< PlacementNodeData const* >( node_r.data_ptr() );
        if( l_ptr == nullptr || r_ptr == nullptr ) {
            return false;
        }
        return l_ptr->name    == r_ptr->name &&
               node_l.index() == node_r.index();
    };

    auto edge_comparator = [] (
        PlacementTreeEdge const& edge_l,
        PlacementTreeEdge const& edge_r
    ) {
        auto l_ptr = dynamic_cast< PlacementEdgeData const* >( edge_l.data_ptr() );
        auto r_ptr = dynamic_cast< PlacementEdgeData const* >( edge_r.data_ptr() );
        if( l_ptr == nullptr || r_ptr == nullptr ) {
            return false;
        }
        return l_ptr->edge_num()               == r_ptr->edge_num()               &&
               edge_l.primary_node().index()   == edge_r.primary_node().index()   &&
               edge_l.secondary_node().index() == edge_r.secondary_node().index();
    };

    return tree::equal(
        lhs, rhs, node_comparator, edge_comparator
    );
}

bool compatible_trees( Sample const& lhs, Sample const& rhs )
{
    return compatible_trees( lhs.tree(), rhs.tree() );
}

// =================================================================================================
//     Conversion
// =================================================================================================

PlacementTree convert_default_tree_to_placement_tree( tree::DefaultTree const& source_tree )
{
    auto node_data_converter = [] ( tree::BaseNodeData const& source_node ) {
        auto node_data = PlacementNodeData::create();
        auto& source_data = dynamic_cast< tree::DefaultNodeData const& >( source_node );
        node_data->name = source_data.name;
        return node_data;
    };

    auto edge_data_converter = [] ( tree::BaseEdgeData const& source_edge ) {
        auto edge_data = PlacementEdgeData::create();
        auto& source_data = dynamic_cast< tree::DefaultEdgeData const& >( source_edge );
        edge_data->branch_length = source_data.branch_length;
        return edge_data;
    };

    auto result = tree::convert(
        source_tree,
        node_data_converter,
        edge_data_converter
    );

    // Need to set the edge nums accordingly, as those are not part of Default Tree Edge Data.
    reset_edge_nums( result );
    return result;
}

double add_sample_to_mass_tree(
    Sample const& smp, double const sign, double const scaler, tree::MassTree& target
) {
    double pendant_work = 0.0;

    for( auto const& pqry : smp.pqueries() ) {
        double multiplicity = total_multiplicity( pqry );

        for( auto const& place : pqry.placements() ) {
            auto& edge = target.edge_at( place.edge().index() );

            // Use the relative position of the mass on its original branch to put it to the
            // same position relative to its new branch.
            double position
                = place.proximal_length
                / place.edge().data<PlacementEdgeData>().branch_length
                * edge.data<tree::MassTreeEdgeData>().branch_length;

            // Add the mass at that position, normalized and using the sign.
            edge.data<tree::MassTreeEdgeData>().masses[ position ]
                += sign * place.like_weight_ratio * multiplicity / scaler;

            // Accumulate the work we need to do to move the masses from their pendant
            // positions to the branches.
            pendant_work += place.like_weight_ratio * multiplicity * place.pendant_length / scaler;
        }
    }

    return pendant_work;
}

std::pair< tree::MassTree, double > convert_sample_to_mass_tree( Sample const& sample )
{
    auto mass_tree = tree::convert_default_tree_to_mass_tree( sample.tree() );
    double const total_mass = total_placement_mass_with_multiplicities( sample );
    double const pend_work = add_sample_to_mass_tree(
        sample, +1.0, total_mass, mass_tree
    );
    return { std::move( mass_tree ), pend_work };
}

std::pair<
    std::vector<tree::MassTree>,
    std::vector<double>
> convert_sample_set_to_mass_trees( SampleSet const& sample_set )
{
    // Build an average branch length tree for all trees in the SampleSet.
    // This also serves as a check whether all trees in the set are compatible with each other,
    // as average_branch_length_tree() throws if the trees have different topologies.
    // Then, turn the resulting tree into a MassTree.
    tree::TreeSet avg_tree_set;
    for( auto const& smp : sample_set ) {
        avg_tree_set.add( "", smp.sample.tree() );
    }
    auto const mass_tree = tree::convert_default_tree_to_mass_tree(
        tree::average_branch_length_tree( avg_tree_set )
    );
    avg_tree_set.clear();
    // TODO if we introduce an avg tree calculation that accepts an iterator, we do not need
    // TODO to create a copied tree set of all trees here.

    // Prepare mass trees for all Samples, by copying the average mass tree.
    // This massively speeds up the calculations (at the cost of extra storage for all the trees).
    auto mass_trees = std::vector<tree::MassTree>( sample_set.size(), mass_tree );

    // Also, prepare a vector to store the pendant works.
    auto pend_works = std::vector<double>( sample_set.size(), 0.0 );

    // Add the placement mass of each Sample to its MassTree.
    #pragma omp parallel for schedule( dynamic )
    for( size_t i = 0; i < sample_set.size(); ++i ) {
        // Get the total sum of placement masses for the sample...
        double const total_mass = total_placement_mass_with_multiplicities( sample_set[i].sample );

        // ... and use it as scaler to add the mass to the mass tree for this sample.
        double const pend_work = add_sample_to_mass_tree(
            sample_set[i].sample, +1.0, total_mass, mass_trees[i]
        );

        // Also, store the pend work.
        pend_works[ i ] = pend_work;
    }

    return { std::move( mass_trees ), std::move( pend_works ) };
}

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << (std::ostream& out, Sample const& smp)
{
    auto table = utils::Table();
    auto const kRight = utils::Table::Column::Justification::kRight;

    table.add_column("#").justify(kRight);
    table.add_column("name");
    table.add_column("edge_num").justify(kRight);
    table.add_column("likelihood").justify(kRight);
    table.add_column("like_weight_ratio").justify(kRight);
    table.add_column("proximal_length").justify(kRight);
    table.add_column("pendant_length").justify(kRight);

    size_t i = 0;
    for( auto const& pqry : smp.pqueries() ) {
        std::string name = pqry.name_size() > 0 ? pqry.name_at(0).name : "";
        if( pqry.name_size() > 1 ) {
            name += " (+" + std::to_string( pqry.name_size() - 1 ) + ")";
        }

        for( auto const& p : pqry.placements() ) {
            table << std::to_string(i);
            table << name;
            table << std::to_string( p.edge_num() );
            table << std::to_string( p.likelihood );
            table << std::to_string( p.like_weight_ratio );
            table << std::to_string( p.proximal_length );
            table << std::to_string( p.pendant_length );
        }

        ++i;
    }

    out << utils::simple_layout()(table);
    return out;
}

std::string print_tree( Sample const& smp )
{
    auto place_map = placements_per_edge( smp );

    auto print_line = [ &place_map ]( PlacementTreeNode const& node, PlacementTreeEdge const& edge )
    {
        return node.data<PlacementNodeData>().name
            + " [" + std::to_string(
                edge.data<PlacementEdgeData>().edge_num()
            ) + "]" ": "
            + std::to_string( place_map[ edge.index() ].size() ) + " placements";
    };
    return tree::PrinterCompact().print( smp.tree(), print_line );
}

} // namespace placement
} // namespace genesis
