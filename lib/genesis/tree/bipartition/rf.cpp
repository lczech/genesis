/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech

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

#include "genesis/tree/bipartition/rf.hpp"

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/utils/core/algorithm.hpp"

#include <cstdint>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <limits>

#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace tree {

// =================================================================================================
//     Taxon Name Handling
// =================================================================================================

std::unordered_map<std::string, size_t> rf_taxon_name_map( Tree const& tree )
{
    std::unordered_map<std::string, size_t> names;

    // Collect all leaf node names and give them unique indices.
    // Make sure that inner nodes do not have names, as this might confuse all downstream steps.
    for( auto const& node : tree.nodes() ) {
        auto const& name = node.data<CommonNodeData>().name;

        // Only consider leaf nodes.
        if( is_leaf( node )) {
            if( name.empty() ) {
                throw std::runtime_error(
                    "Cannot calculate RF distnace with empty leaf node names."
                );
            }
            if( names.count( name ) > 0 ) {
                throw std::runtime_error(
                    "Cannot calculate RF distance with tree that has duplicate node names. "
                    "Name '" + name + "' appears multiple times."
                );
            }

            // We use the current size of the map to get unique indices.
            auto const idx = names.size();
            names[ name ] = idx;
        }

        // For now, we allow inner node names. They do not matter for our algorithm,
        // so we just ignore them. However, this might lead to confusion, because the RF distance
        // needs taxon names to identify nodes, and having names on inner nodes just doesn't make
        // sense in this context. Still, let's allow it and hope for sane users.
        // else if( ! name.empty() ) {
        //     throw std::runtime_error(
        //         "Cannot calculate RF distance with tree that has inner node names. "
        //         "Found name '" + name + "' at inner node."
        //     );
        // }
    }

    return names;
}

// =================================================================================================
//     Finding Split Bitvectors of Trees
// =================================================================================================

/**
 * @brief Local helper function template that constructs all bitvectors for the splits of a tree,
 * but allows to customize what to do with them once constructed.
 *
 * This is meant to avoid code duplication, but still gives performant code due to the local
 * scope of the template: if the compiler is halfway smart, it will inline everything.
 */
template<typename BitvectorProcessor>
void rf_get_bitvectors_template(
    Tree const& tree,
    std::unordered_map<std::string, size_t> const& names,
    BitvectorProcessor const& process_bitvector
) {
    using utils::Bitvector;

    // Prepare intermediate structure for each edge of the tree,
    // which keeps track of all Bitvectors of the edges.
    auto bipartitions = std::vector<Bitvector>( tree.edge_count() );

    // We also keep track of names: each one needs to appear exactly once!
    auto name_check = Bitvector( names.size() );

    // If the tree is rooted, we need to skip one of the two edges next to the root.
    // They induce the same split, and we do not want to count it twice.
    // In order to skip one of the two root edges in rooted trees, we store one of their
    // indices, and skip it later. It does not matter which index we store.
    // In unrooted trees, we instead store a dummy that never gets skipped.
    // This could theoretically also happen at inner nodes, if they just have two neighbors,
    // which is possible, but rare. We ignore it here.
    size_t root_skip = std::numeric_limits<size_t>::max();
    if( is_rooted( tree )) {
        assert( degree( tree.root_node()) == 2 );
        root_skip = tree.root_node().primary_edge().secondary_node().index();
    }

    // Fill bitvectors.
    for( auto it : postorder(tree) ) {
        // We want to iterate edges, so skip the last iteration at the root node,
        // as then we already have processed all edges.
        if( it.is_last_iteration() ) {
            continue;
        }

        // Also skip one of the root edges if the tree is rooted.
        if( it.node().index() == root_skip ) {
            continue;
        }

        // If the iterator is at a leaf, just set one bit in the bitvector.
        if( is_leaf( it.node() ) ) {

            // Get the index of the name of the leaf.
            auto const& name = it.node().data<CommonNodeData>().name;
            auto const nit = names.find( name );
            if( nit == names.end() ) {
                throw std::runtime_error(
                    "Cannot calculate RF distance with inconsistent node names. "
                    "Name '" + name + "' is missing from a tree."
                );
            }

            // Check that the name did not appear yet in the tree.
            if( name_check[ nit->second ] ) {
                throw std::runtime_error(
                    "Cannot calculate RF distance with tree that has duplicate node names. "
                    "Name '"+ name + "' appears multiple times."
                );
            }
            name_check.set( nit->second );

            // Store the result in the intermediate structure.
            // We use a bitvector with just one bit set at the index of the current leaf.
            // This uniquely identifies this trivial split.
            auto const eidx = it.edge().index();
            bipartitions[ eidx ] = Bitvector( names.size() );
            bipartitions[ eidx ].set( nit->second );

        // For inner iterator positions, consider the whole subtree below it.
        } else {
            auto current = Bitvector( names.size() );

            // Here, we could test for inner node names.
            // But as above, we ignore and hence allow them.

            // We do postorder traversal, so all subtrees of the current node have been processed.
            // They store trivial splits as single unique bits in their respective bitvectors.
            // So here, we simply combine them (using or), to get a bitvector of all tips of the
            // current split. This is not normalized yet, meaning that these bits could stand
            // for both ways of denoting that split. We later do the needed normalization.
            auto l = &it.link().next();
            while( l != &it.link() ) {
                current |= bipartitions[ l->edge().index() ];
                l = &l->next();
            }

            // Store at the current edge in the intermediate structure.
            // This needs to be the not yet normalized one, because we are still filling up further
            // inner nodes, and hence need to maintain these bits as they are.
            bipartitions[ it.edge().index() ] = current;

            // Call the bitvector processor functor now, as we just finished constructing a split.
            // We normalize first to make sure that we always get comparable bitvectors in the end.
            current.normalize();
            process_bitvector( current );
        }
    }

    // We have traversed all node names now. If there is still an unset bit in the bitvector,
    // that means that we did not find all names that are in the tree.
    if( name_check.count() != names.size() ) {
        throw std::runtime_error(
            "Cannot calculate RF distance with trees that have different node names. "
            "Some names are missing from one of the trees."
        );
    }
}

std::vector<utils::Bitvector> rf_get_bitvectors(
    Tree const& tree,
    std::unordered_map<std::string, size_t> const& names
) {
    using utils::Bitvector;

    // Prepare result.
    std::vector<Bitvector> result;
    result.reserve( inner_edge_count( tree ));

    // Get all bitvectors and store them in the result.
    rf_get_bitvectors_template( tree, names, [&]( Bitvector const& bitvec ){
        result.push_back( bitvec );
    });

    return result;
}

// =================================================================================================
//     Getting Occurrences of Splits in Trees
// =================================================================================================

std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences( TreeSet const& trees )
{
    using utils::Bitvector;

    // Map from bitvectors of splits to bitvectors of occurences:
    // which bitvector (keys of the map) occurs in which tree (values associated with each key).
    auto result = std::unordered_map<utils::Bitvector, utils::Bitvector>();

    // Edge case.
    if( trees.empty() ) {
        return result;
    }

    // Get a unique ID for each taxon name.
    auto const names = rf_taxon_name_map( trees[0] );

    #pragma omp parallel for
    for( size_t i = 0; i < trees.size(); ++i ) {

        // Get all bitvectors of the tree and add their occurrences to the map.
        // This way, we do not need to actually store them, but can process them on the fly.
        // Saves mem and should be faster as well.
        rf_get_bitvectors_template( trees[i], names, [&]( Bitvector const& bitvec ){
            #pragma omp critical(GENESIS_BIPARTITION_OCCURRENCES)
            {
                if( result.count( bitvec ) == 0 ) {
                    result[bitvec] = Bitvector( trees.size() );
                }
                result[bitvec].set( i );
            }
        });
    }

    return result;
}

std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences(
    Tree const& lhs,
    TreeSet const& rhs
) {
    using utils::Bitvector;

    // Map from bitvectors of splits to bitvectors of occurences:
    // which bitvector (keys of the map) occurs in which tree (values assiciated with each key).
    auto result = std::unordered_map<utils::Bitvector, utils::Bitvector>();

    // Edge case.
    if( rhs.empty() ) {
        return result;
    }

    // Get a unique ID for each taxon name.
    auto const names = rf_taxon_name_map( lhs );

    // Get the split bitvectors for the lhs tree. We initialize with enough room
    // for lhs and rhs trees.
    rf_get_bitvectors_template( lhs, names, [&]( Bitvector const& bitvec ){
        assert( result.count( bitvec ) == 0 );
        result[bitvec] = Bitvector( 1 + rhs.size() );
        result[bitvec].set( 0 );
    });

    // Process the rhs trees, and add their split bitvectors.
    #pragma omp parallel for
    for( size_t i = 0; i < rhs.size(); ++i ) {

        rf_get_bitvectors_template( rhs[i], names, [&]( Bitvector const& bitvec ){
            #pragma omp critical(GENESIS_BIPARTITION_OCCURRENCES)
            {
                // We start indexing 1 off, for the lhs tree.
                if( result.count( bitvec ) == 0 ) {
                    result[bitvec] = utils::Bitvector( 1 + rhs.size() );
                }
                result[bitvec].set( 1 + i );
            }
        });
    }

    return result;
}

std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences(
    Tree const& lhs,
    Tree const& rhs
) {
    using utils::Bitvector;

    // Map from bitvectors of splits to bitvectors of occurences:
    // which bitvector (keys of the map) occurs in which tree (values assiciated with each key).
    auto result = std::unordered_map<utils::Bitvector, utils::Bitvector>();

    // Get a unique ID for each taxon name.
    auto const names = rf_taxon_name_map( lhs );

    // Get the split bitvectors for the lhs tree. We initialize with enough room
    // for lhs and rhs trees.
    rf_get_bitvectors_template( lhs, names, [&]( Bitvector const& bitvec ){
        assert( result.count( bitvec ) == 0 );
        result[bitvec] = Bitvector( 2 );
        result[bitvec].set( 0 );
    });

    // Do the same for the rhs tree. This time we need to make sure not to overwrite any
    // existing splits in the map.
    rf_get_bitvectors_template( rhs, names, [&]( Bitvector const& bitvec ){
        if( result.count( bitvec ) == 0 ) {
            result[bitvec] = utils::Bitvector( 2 );
        }
        result[bitvec].set( 1 );
    });

    return result;
}

// =================================================================================================
//     Absolute RF Distance Functions
// =================================================================================================

utils::Matrix<size_t> rf_distance_absolute( TreeSet const& trees )
{
    using utils::Matrix;

    auto result = Matrix<size_t>( trees.size(), trees.size(), 0 );
    auto const hash_occs = rf_get_occurrences( trees );

    // We test every split that occurred in the trees.
    for( auto const& hash_occ : hash_occs ) {

        // Go through all trees and see if it appeared in them.
        for( size_t i = 0; i < trees.size(); ++i ) {
            if( hash_occ.second[i] == false ) {
                continue;
            }

            // If we are here, we have a split that occured in tree i.
            // Now we check, if it also appeared in tree j (for all j!=i).
            // If not, we have a split that is part of i but not of j,
            // so it adds to their pairwise distance.
            for( size_t j = 0; j < trees.size(); ++j ) {
                if( i == j ) {
                    continue;
                }
                if( hash_occ.second[j] == false ) {
                    ++result( i, j );
                    ++result( j, i );
                }
            }
        }
    }

    return result;
}

std::vector<size_t> rf_distance_absolute( Tree const& lhs, TreeSet const& rhs )
{
    auto result = std::vector<size_t>( rhs.size(), 0 );
    auto const hash_occs = rf_get_occurrences( lhs, rhs );

    // We test every split that occurred in all of the trees.
    for( auto const& hash_occ : hash_occs ) {
        // See if it was in the lhs tree.
        bool const in_lhs = hash_occ.second[0];

        // Now go through all rhs trees and see if it also appeared there.
        for( size_t i = 0; i < rhs.size(); ++i ) {
            bool const in_rhs = hash_occ.second[ 1 + i ];

            // Now, in_lhs and in_rhs indicate in which of the trees the split appeared.
            // It adds to the distance between lhs and rhs[i] only if those two differ,
            // which is the case if in_lhs ^ in_rhs == true.

            // Bool converts to 1, so we can use this to process without branching.
            result[i] += (in_lhs ^ in_rhs);

            // Just in case, this is equivalent, but needs branching:
            // if( in_lhs ^ in_rhs ) {
            //     ++result[i];
            // }
        }
    }

    return result;
}

size_t rf_distance_absolute( Tree const& lhs, Tree const& rhs )
{
    size_t result = 0;

    // Get a map of all splits that appear in the two trees to a bitvector of size two
    // indicating in which of the trees the split appeared.
    auto const hash_occs = rf_get_occurrences( lhs, rhs );

    // We test every split that occurred in all of the trees.
    for( auto const& hash_occ : hash_occs ) {
        assert( hash_occ.second.size() == 2 );

        // See if it was in the lhs tree, and rhs tree, respectively.
        bool const in_lhs = hash_occ.second[ 0 ];
        bool const in_rhs = hash_occ.second[ 1 ];

        // At least one of them needs to be set, otherwise the split should not have ended
        // up in the split list in the first place.
        assert( in_lhs || in_rhs );

        // Now, in_lhs and in_rhs indicate in which of the trees the split appeared.
        // It adds to the distance between lhs and rhs[i] only if those two differ,
        // which is the case if in_lhs ^ in_rhs == true.

        // Bool converts to 1, so we can use this to process without branching.
        result += (in_lhs ^ in_rhs);
    }

    return result;
}

// =================================================================================================
//     Relative RF Distance Functions
// =================================================================================================

utils::Matrix<double> rf_distance_relative( TreeSet const& trees )
{
    // Prepare result.
    auto result = utils::Matrix<double>( trees.size(), trees.size() );
    if( trees.size() == 0 ) {
        return result;
    }

    // Compute abs rf dist.
    auto const rf = rf_distance_absolute( trees );
    assert( rf.rows() == trees.size() );
    assert( rf.cols() == trees.size() );

    // Get norm factor.
    assert( trees.size() > 0 );
    if( trees[0].node_count() < 3 ) {
        throw std::runtime_error(
            "Cannot compute relative RF distance for trees with less than 3 taxa."
        );
    }
    double const norm = 2.0 * static_cast<double>( trees[0].node_count() - 3 );

    // Compute matrix.
    #pragma omp parallel for
    for( size_t i = 0; i < rf.rows(); ++i ) {
        for( size_t j = 0; j < rf.cols(); ++j ) {
            result( i, j ) = static_cast<double>( rf( i, j )) / norm;
        }
    }

    return result;
}

std::vector<double> rf_distance_relative( Tree const& lhs, TreeSet const& rhs )
{
    // Prepare result.
    auto result = std::vector<double>( rhs.size() );

    // Compute abs rf dist.
    auto const rf = rf_distance_absolute( lhs, rhs );
    assert( rf.size() == rhs.size() );

    // Get norm factor.
    if( lhs.node_count() < 3 ) {
        throw std::runtime_error(
            "Cannot compute relative RF distance for trees with less than 3 taxa."
        );
    }
    double const norm = 2.0 * static_cast<double>( lhs.node_count() - 3 );

    // Compute vector.
    #pragma omp parallel for
    for( size_t i = 0; i < rf.size(); ++i ) {
        result[i] = static_cast<double>( rf[i] ) / norm;
    }

    return result;
}

double rf_distance_relative( Tree const& lhs, Tree const& rhs )
{
    // Compute abs rf dist.
    auto const rf = rf_distance_absolute( lhs, rhs );

    // Get norm factor.
    if( lhs.node_count() < 3 ) {
        throw std::runtime_error(
            "Cannot compute relative RF distance for trees with less than 3 taxa."
        );
    }
    double const norm = 2.0 * static_cast<double>( lhs.node_count() - 3 );

    return static_cast<double>( rf ) / norm;
}

} // namespace tree
} // namespace genesis
