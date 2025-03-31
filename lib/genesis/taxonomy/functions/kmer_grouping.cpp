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

#include "genesis/taxonomy/functions/kmer_grouping.hpp"

#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/operators.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/iterator/postorder.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/logging.hpp"
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
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxon Count Grouping
// =================================================================================================

// --------------------------------------------------------------------------
//     group_by_taxon_sizes
// --------------------------------------------------------------------------

/**
 * @brief Local helper function to check if a given taxon size exceeds the limits.
 */
bool exceeds_group_sizes_(
    TaxonGroupingLimits const& limits,
    size_t num_sequences,
    size_t sum_seq_lengths
) {
    bool const exceed_num_sequences = (
        limits.max_group_num_sequences > 0 &&
        num_sequences > limits.max_group_num_sequences
    );
    bool const exceed_sum_seq_lengths = (
        limits.max_group_sum_seq_lengths > 0 &&
        sum_seq_lengths > limits.max_group_sum_seq_lengths
    );
    return exceed_num_sequences || exceed_sum_seq_lengths;
}

/**
 * @brief Local helper function that recursively assigns the group of the parent taxon
 * to all children.
 */
void assign_children_to_group_(
    Taxon& taxon
) {
    // Get the data and assert that we are in the right type of taxon
    auto& data = taxon.data<KmerTaxonData>();
    assert( data.status == KmerTaxonData::Status::kGroupAssigned );
    assert( data.index != std::numeric_limits<size_t>::max() );

    // In some previous iteration of the procedur, we already decided that this taxon here is
    // small enough to fit in a group. So all we have to do is to assign that same group to its
    // children here. That will then also be repeated (in the later iterations of the recursion)
    // for their children, and so on, so that the whole subtree/clade of this taxon gets their
    // data set to point to the same group index.
    for( auto& child : taxon ) {
        auto& child_data = child.data<KmerTaxonData>();
        child_data.status = data.status;
        child_data.index = data.index;
        assign_children_to_group_( child );
    }
}

/**
 * @brief Local helper function to process the children of a Taxon for which we already
 * know that it is too big for a single group, and hence was expanded.
 */
void group_by_taxon_sizes_process_taxon_(
    TaxonGroupingLimits const& limits,
    Taxonomy& tax,
    size_t& next_index
) {
    // We are going to process all children of the current taxon. We are either deciding
    // to expand them if they are too big, or to group them with their siblings up until
    // they reach the max size. To keep track of the latter, we need a list of siblings
    // that have been processed already: This vector stores the group indicies used already
    // for the children of the current taxon, and their cumulative sizes.
    struct GroupSummary
    {
        size_t index = 0;
        size_t clade_num_sequences = 0;
        size_t clade_sum_seq_lengths = 0;
    };
    std::vector<GroupSummary> sibling_groups;

    // Process the children of the taxon, grouping them as needed. There are two outcomes of that:
    // Either we decid to break it apart, because it exceeded some size limit, or we assign
    // it to a group.
    for( auto& child_taxon : tax ) {
        auto& child_data = child_taxon.data<KmerTaxonData>();

        // Determine if this taxon is too big or small enough to be (part of) a single group.
        // If it is too big, and has multiple children, we expand it. It can however happen that
        // a taxon is too big for our limits, but has no children (meaning, all ref sequences are
        // assigned to that one leaf taxon). In that case, we just have to live with a large group.
        bool const exceeds_limits = exceeds_group_sizes_(
            limits, child_data.clade_num_sequences, child_data.clade_sum_seq_lengths
        );
        if( exceeds_limits && !taxon_is_single_lineage( child_taxon )) {
            child_data.status = KmerTaxonData::Status::kGroupExpanded;
            group_by_taxon_sizes_process_taxon_( limits, child_taxon, next_index );

        } else {
            // If the child taxon is not too big (or a leaf), we try to combine it into a group
            // with its other small siblings, in order to reduce the number of groups.
            // Find the first index of the child group list that we can merge this taxon into
            // without exceeding the sizes.
            // In a sense, out of all siblings that are smaller than the max size, we are
            // solving the knappsack problem here, greedily by assigning groups on a first come
            // first served basis to the lowest index group that has space for the new sibling.
            auto sibling_index = std::numeric_limits<size_t>::max();
            if( limits.merge_sibling_taxa ) {
                for( size_t i = 0; i < sibling_groups.size(); ++i ) {
                    auto const& sibling_group = sibling_groups[i];
                    auto const proposed_num_seq = (
                        sibling_group.clade_num_sequences +
                        child_data.clade_num_sequences
                    );
                    auto const proposed_seq_len = (
                        sibling_group.clade_sum_seq_lengths +
                        child_data.clade_sum_seq_lengths
                    );
                    if( ! exceeds_group_sizes_( limits, proposed_num_seq, proposed_seq_len )) {
                        sibling_index = i;
                        break;
                    }
                }
            }

            // If there was no such index, either we do not want to merge siblings, or all groups
            // are already filled to a point where adding this taxon would exceed the limit, or
            // the list is still empty. Either way, we want to make a new group then for the taxon.
            if( sibling_index == std::numeric_limits<size_t>::max() ) {
                sibling_index = sibling_groups.size();
                sibling_groups.emplace_back();
                sibling_groups.back().index = next_index;
                ++next_index;
            }

            // Now we know the group that we want to put this taxon into.
            // Set this, and then recursively set this group for all its children as well.
            auto& sibling_group = sibling_groups[sibling_index];
            sibling_group.clade_num_sequences += child_data.clade_num_sequences;
            sibling_group.clade_sum_seq_lengths += child_data.clade_sum_seq_lengths;
            child_data.status = KmerTaxonData::Status::kGroupAssigned;
            child_data.index = sibling_group.index;
            assign_children_to_group_( child_taxon );
        }
    }
}

void group_by_taxon_sizes(
    TaxonGroupingLimits const& limits,
    Taxonomy& tax
) {
    // Check that the taxonoy has the correct data type everywhere.
    if( ! taxonomy_data_is<KmerTaxonData>( tax ) ) {
        throw std::invalid_argument( "Cannot group taxonomy without it having KmerTaxonData" );
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

    // We will be assigning consecutive indices to groups.
    // This variable indicates the next index to be used, and is incremented for each new group.
    size_t next_index = 0;

    // Now run the main recursion. This effectively is a preorder traversal of the taxonomy, i.e.,
    // start at the root (highest rank), and decent into the lower ranks one by one.
    // At each taxon we visit, we assess its children and decide how to group them.
    group_by_taxon_sizes_process_taxon_( limits, tax, next_index );
}

// --------------------------------------------------------------------------
//     group_with_target_number_of_groups
// --------------------------------------------------------------------------

size_t group_with_target_number_of_groups( TaxonGroupingSearchParams const& params, Taxonomy& tax )
{
    // We can only use one variable to limit our search.
    if( params.initial_group_num_sequences != 0 && params.initial_group_sum_seq_lengths != 0 ) {
        throw std::invalid_argument(
            "Cannot run group_with_target_number_of_groups() with both limits at the same time."
        );
    }
    if( params.initial_group_num_sequences == 0 && params.initial_group_sum_seq_lengths == 0 ) {
        throw std::invalid_argument(
            "Cannot run group_with_target_number_of_groups() with no initial limits set."
        );
    }

    // We run the search in two phases: First, from the starting value of our limit,
    // we expand (or shrink) until the starting value and the expanded (or shrunken) value
    // form a range of limits such that the resulting group size is covered by that range.
    // Then, in the second phase, we run binary search on that range, until the resulting
    // group size is as close as we can get.
    // The initidal direction tells us if in the first phase, starting from the given param
    // initial value, we need to grow or shrink. Then, once we overshoot with that, we know
    // we have found the boundaries.
    int initial_direction = 0;
    bool found_boundaries = false;

    // Current value of our limit, as well as the two boundary values (low and high).
    // We already checked that only one of the initial limits is non-zero, so we can just add them
    // for simplicty to get the one that we actually want.
    size_t limit_c = params.initial_group_num_sequences + params.initial_group_sum_seq_lengths;
    size_t limit_l = limit_c;
    size_t limit_h = limit_c;

    // We loop until either we have found a limit setting that results in the desired number
    // of groups, or until the binary search finishes (both low and high have the same value),
    // in which case we cannot get any closer to our target number of groups.
    while( true ) {

        // The current limit always is in between the low and high
        assert( limit_l <= limit_c );
        assert( limit_c <= limit_h );

        // Construct groups with the current limits.
        // Depending on whether the given params want us to modify the number of sequences,
        // or their total combined length per group, we set the internal "limit" here, which can
        // be either of those two variables.
        TaxonGroupingLimits limits;
        if( params.initial_group_num_sequences != 0 ) {
            limits.max_group_num_sequences = limit_c;
        } else {
            assert( params.initial_group_sum_seq_lengths != 0 );
            limits.max_group_sum_seq_lengths = limit_c;
        }
        limits.merge_sibling_taxa = params.merge_sibling_taxa;
        group_by_taxon_sizes( limits, tax );
        auto const group_cnt = count_taxon_groups_or_partitions( tax );

        // User output. Can be deactivated via logging settings.
        LOG_MSG
            << "Phase " << ((int) found_boundaries) << ": grouping with limit " << limit_c
            << " within [" << limit_l << ", " << limit_h << "], resulted in "
            << group_cnt << " groups"
        ;

        // Exit condition, if low and high are identical, we cannot optimize any more.
        // This is checked after the above grouping, such that the final result is as good as it gets.
        if( found_boundaries && limit_l == limit_h ) {
            break;
        }

        // Update the limit value that we use for the grouping.
        // In the first phase, we search for the set of lower and upper boundaries of that
        // value that leads to including the target group size. In the second phase, once we have
        // established the boundaries, we run a binary search on that.
        if( ! found_boundaries ) {
            // Initially, in the first phase, we need to find the boundaries for the binary search

            if( group_cnt > params.target_group_count ) {
                // Too many groups --> need larger high limit (for the number of sequences in the
                // group, or their total length), so that we get fewer groups.

                if( initial_direction == -1 ) {
                    // We were shrinking first, but now overshot --> found the boundaries!
                    found_boundaries = true;
                    limit_c = ( limit_l + limit_h ) / 2;
                } else {
                    // Otherwise, we are not done yet, and need to increase the high limit.
                    // We also increase the low limit, as it is too low.
                    initial_direction = 1;
                    limit_l = limit_h;
                    limit_h *= 2;
                    limit_c = limit_h;
                }

            } else if( group_cnt < params.target_group_count ) {
                // Too few groups --> need smaller low limit, so that we get more groups

                if( initial_direction == 1 ) {
                    // We were growing first, but now undershot --> found the boundaries!
                    found_boundaries = true;
                    limit_c = ( limit_l + limit_h ) / 2;
                } else {
                    // Otherwise, we are not done yet, and need to decrease the low limit.
                    // We also decrease the high limit, as it is too high.
                    initial_direction = -1;
                    limit_h = limit_l;
                    limit_l /= 2;
                    limit_c = limit_l;
                }

            } else {
                // Exactly right --> we just got lucky, and can stop here
                break;
            }
        } else {
            // In the second phase, we run a binary search on the group sizes: Narrow in
            // on the low and high limit until we hit the exact right resulting group count.
            // Above, we have the additional exit condition that if both low and high are equal,
            // we also stop, as at that point the search revealed that we cannot get closer to
            // the target size than with the limits at that point.
            if( group_cnt > params.target_group_count ) {
                limit_l = limit_c;
            } else if( group_cnt < params.target_group_count ) {
                limit_h = limit_c;
            } else {
                assert( group_cnt == params.target_group_count );
                break;
            }
            limit_c = ( limit_l + limit_h ) / 2;
        }
    }

    return limit_c;
}

} // namespace taxonomy
} // namespace genesis
