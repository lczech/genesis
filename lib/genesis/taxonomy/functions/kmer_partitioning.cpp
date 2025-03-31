/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/kmer_partitioning.hpp"

#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/operators.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/table.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxon Count Partitioning
// =================================================================================================

void partition_taxonomy(
    Taxonomy& tax,
    size_t num_partitions
) {
    // Ad hoc solution that might be good enough for now: split taxa up until their size is below
    // the required average clade size that we want... then merge back the smallest pair of neighbors
    // until we have our desired number of groups. As we add elements in taxonomic order,
    // the hope is that the merged neighbors are taxonomically not too far. Will not always be true,
    // but might be good enough?
    // TODO Improvements:
    //  - Only merge taxa that are not too far apart on the taxonomy.
    //  - Use dynamic programming to find optimal partitioning?

    // Check that the taxonoy has the correct data type everywhere.
    if( ! taxonomy_data_is<KmerTaxonData>( tax ) ) {
        throw std::invalid_argument( "Cannot partition taxonomy without it having KmerTaxonData" );
    }

    // Accumulate all sizes towards the higher ranks.
    accumulate_taxon_sizes( tax );

    // Initialize all group assignments to the unprocessed status.
    preorder_for_each( tax, []( Taxon& taxon )
    {
        auto& data = taxon.data<KmerTaxonData>();
        data.status = KmerTaxonData::Status::kUnprocessed;
        data.index = std::numeric_limits<size_t>::max();
    });

    // Get the total size of the taxonomy.
    size_t total_size = 0;
    for( auto const& taxon : tax ) {
        auto const& data = taxon.data<KmerTaxonData>();
        total_size += data.clade_sum_seq_lengths;
    }

    // We collect partitions (clades) by preorder traversal, basically linearizing the taxonomy,
    // and store the list of clades here. Each clade here does initially not exeed the average clade
    // size that we want. Then, we merge smaller ones until we get the target number of partitions.
    struct Clade {
        size_t clade_size;
        std::vector<Taxon*> clade_taxa;
    };
    std::vector<Clade> clades;

    // Helper function to fill the clades list will all taxa that are small enough
    std::function<void(Taxonomy&)> collect_partitions_ = [&]( Taxonomy& tax )
    {
        for( auto& child : tax ) {
            auto& child_data = child.data<KmerTaxonData>();
            // If the child is smaller than our target avg clade size, we add it
            // as a monophyletic clade. Also, if it does not contain any children,
            // which is an extra condition needed for leaf taxa that are very big.
            // Otherwise, we have a clade that we want to break apart, so it will
            // be paraphyletic.
            if(
                child_data.clade_sum_seq_lengths < total_size / num_partitions ||
                child.size() == 0
            ) {
                child_data.status = KmerTaxonData::Status::kPartitionMonophyletic;
                clades.push_back({ child_data.clade_sum_seq_lengths, { &child }});
            } else {
                child_data.status = KmerTaxonData::Status::kPartitionParaphyletic;
                clades.push_back({ child_data.sum_seq_lengths, { &child }});
                collect_partitions_( child );
            }
        }
    };
    collect_partitions_( tax );
    assert( clades.size() > 0 );

    // Now merge the smallest pair of neighbors until we have the required num of groups.
    while( clades.size() > num_partitions ) {
        // Find the smallest neighboring pair of entries.
        size_t first_index = 0;
        size_t combined_size = std::numeric_limits<size_t>::max();
        for( size_t i = 0; i < clades.size() - 1; ++i ) {
            if( clades[i].clade_size + clades[i+1].clade_size < combined_size ) {
                first_index = i;
                combined_size = clades[i].clade_size + clades[i+1].clade_size;
            }
        }
        assert( combined_size != std::numeric_limits<size_t>::max() );

        // Combine the pair into the first, and erase the second.
        clades[first_index].clade_size += clades[first_index+1].clade_size;
        clades[first_index].clade_taxa.insert(
            clades[first_index].clade_taxa.end(),
            clades[first_index+1].clade_taxa.begin(),
            clades[first_index+1].clade_taxa.end()
        );
        clades.erase(clades.begin() + first_index+1);
    }
    assert( clades.size() == num_partitions );

    // Helper function to recursively set the index for all children of a monophyletic clade.
    std::function<void(Taxon&)> assign_children_to_partition_ = [&]( Taxon& taxon )
    {
        // Get the data and assert that we are in the right type of taxon
        auto& data = taxon.data<KmerTaxonData>();
        assert( data.status == KmerTaxonData::Status::kPartitionMonophyletic );
        assert( data.index != std::numeric_limits<size_t>::max() );

        for( auto& child : taxon ) {
            auto& child_data = child.data<KmerTaxonData>();
            child_data.status = data.status;
            child_data.index = data.index;
            assign_children_to_partition_( child );
        }
    };

    // We have the target number of partitions in the clade vector now. Finally, we need
    // to assign partition indices to each taxon, following the order of the clades list.
    for( size_t i = 0; i < clades.size(); ++i ) {
        for( auto& taxon : clades[i].clade_taxa ) {
            auto& data = taxon->data<KmerTaxonData>();
            assert( data.status != KmerTaxonData::Status::kUnprocessed );
            data.index = i;
            if( data.status == KmerTaxonData::Status::kPartitionMonophyletic ) {
                assign_children_to_partition_( *taxon );
            }
        }
    }
}

} // namespace taxonomy
} // namespace genesis
