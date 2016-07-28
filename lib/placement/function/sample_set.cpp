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
 * @ingroup placement
 */

#include "placement/function/sample_set.hpp"

#include "placement/function/functions.hpp"
#include "placement/function/operators.hpp"
#include "tree/function/tree_set.hpp"
#include "tree/tree_set.hpp"

#include <ostream>

namespace genesis {
namespace placement {

// =================================================================================================
//     Sample Set Functions
// =================================================================================================

/**
* @brief Get the first Sample in a SampleSet that has a given name, or `nullptr` if not found.
*/
Sample* find_sample( SampleSet& sset, std::string const& name )
{
    for (auto& nsmp : sset) {
        if( nsmp.name == name ) {
            return &nsmp.sample;
        }
    }
    return nullptr;
}

/**
* @brief Get the first Sample in a SampleSet that has a given name, or `nullptr` if not found.
*/
Sample const* find_sample( SampleSet const& sset, std::string const& name )
{
    for (auto& nsmp : sset) {
        if( nsmp.name == name ) {
            return &nsmp.sample;
        }
    }
    return nullptr;
}

/**
 * @brief Returns true iff all Trees of the Samples in the set are identical.
 *
 * This is the case if they have the same topology, node names and edge_nums. However, branch
 * lengths are not checked, because usually those differ slightly.
 */
bool all_identical_trees( SampleSet const& sset )
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

    return all_equal( tree_set( sset ), node_comparator, edge_comparator );
}

/**
 * @brief Return a TreeSet containing all the trees of the SampleSet.
 */
tree::TreeSet tree_set( SampleSet const& sset )
{
    tree::TreeSet tset;
    for( auto const& smp : sset ) {
        tset.add( smp.name, smp.sample.tree() );
    }
    return tset;
}

/**
 * @brief Returns a Sample where all maps of this set have been merged into.
 *
 * For this method to succeed, all Samples need to have the same topology, including identical
 * edge_nums and node names. The Tree of the returned Sample has the average branch lenghts
 * from the input trees, using TreeSet::average_branch_length_tree().
 */
Sample merge_all( SampleSet const& sset )
{
    // The following operations do a lot of traversals on all trees: first some for the
    // average_branch_length_tree, then for the merging again. This could be turned into
    // less traversals by copying code and doing all in one run. However, at the current point, this
    // method will be called once in the beginning of a program run, and thus it is not necessary to
    // optimize for speed. Instead, we opt for clean, separated and easy code here.

    if( sset.size() == 0 ) {
        return Sample();
    }

    // Create a new Sample and initialize it with the average branch length tree of all
    // maps in this set, but without any placements.
    auto res = Sample( average_branch_length_tree( tree_set( sset )));

    // Copy the rest of the data from the first tree to the averaged tree.
    // This is necessary, because the tree copy constructor does not do this for us.
    // TODO fix this!
    for (size_t i = 0; i < res.tree().node_count(); ++i) {
        res.tree().node_at(i).data<PlacementNodeData>().name
            = sset[0].sample.tree().node_at(i).data<PlacementNodeData>().name;
    }
    for (size_t i = 0; i < res.tree().edge_count(); ++i) {
        res.tree().edge_at(i).data<PlacementEdgeData>().reset_edge_num(
            sset[0].sample.tree().edge_at(i).data<PlacementEdgeData>().edge_num()
        );
    }

    // Add the placements from all maps of this set.
    // In the merge method, we also check for identical topology (again), but mainly for identical
    // taxa names and edge_nums, which is important for correct merging.
    for (auto& smp : sset) {
        copy_pqueries( smp.sample, res );
    }

    return res;
}

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator << ( std::ostream& out, SampleSet const& sset )
{
    // TODO this was meant for full output. turn it into a printer instead!
    bool full = false;

    size_t i = 0;
    for( auto const& cm : sset ) {
        out << std::to_string(i) << ": " << cm.name << "\n";
        if (full) {
            out << cm.sample << "\n";
        }
        ++i;
    }
    return out;
}

} // namespace placement
} // namespace genesis
