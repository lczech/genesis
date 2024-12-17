/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/taxonomy/functions/kmer.hpp"

#include "genesis/taxonomy/formats/json_reader.hpp"
#include "genesis/taxonomy/formats/json_writer.hpp"
#include "genesis/taxonomy/formats/taxopath_generator.hpp"
#include "genesis/taxonomy/functions/operators.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/iterator/levelorder.hpp"
#include "genesis/taxonomy/iterator/postorder.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/formats/json/document.hpp"
#include "genesis/utils/formats/json/reader.hpp"
#include "genesis/utils/formats/json/writer.hpp"
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
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Taxon Count Grouping
// =================================================================================================

// --------------------------------------------------------------------------
//     accumulate_taxon_sizes
// --------------------------------------------------------------------------

void accumulate_taxon_sizes( Taxonomy& tax )
{
    // Postorder travesal, so that we accumulate values from the outside in,
    // i.e., from lower ranks upwards to the higher ranks.
    postorder_for_each( tax, []( Taxon& taxon ){
        // Cast to our data type. Throws an exception if this is not the data type of the taxon.
        auto& data = taxon.data<KmerTaxonData>();

        // Initialize the totals to the values found in the taxon itself.
        data.clade_num_sequences = data.num_sequences;
        data.clade_sum_seq_lengths = data.sum_seq_lengths;

        // Now traverse the immediate children, and add their sums.
        // As we do postorder traversal here, those have already been set in previous iterations.
        // For leaves of the taxonomy, this just does nothing.
        for( auto const& child : taxon ) {
            auto const& child_data = child.data<KmerTaxonData>();
            data.clade_num_sequences += child_data.clade_num_sequences;
            data.clade_sum_seq_lengths += child_data.clade_sum_seq_lengths;
        }
    });
}

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
    assert( data.group_status == KmerTaxonData::GroupStatus::kAssigned );
    assert( data.group_index != std::numeric_limits<size_t>::max() );

    // In some previous iteration of the procedur, we already decided that this taxon here is
    // small enough to fit in a group. So all we have to do is to assign that same group to its
    // children here. That will then also be repeated (in the later iterations of the recursion)
    // for their children, and so on, so that the whole subtree/clade of this taxon gets their
    // data set to point to the same group index.
    for( auto& child : taxon ) {
        auto& child_data = child.data<KmerTaxonData>();
        child_data.group_status = data.group_status;
        child_data.group_index = data.group_index;
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
            child_data.group_status = KmerTaxonData::GroupStatus::kExpanded;
            group_by_taxon_sizes_process_taxon_( limits, child_taxon, next_index );

        } else {
            // If the child taxon is not too big (or a leaf), we try to combine it into a group
            // with its other small siblings, in order to reduce the number of groups.
            // Find the first index of the child group list that we can merge this taxon into
            // without exceeding the sizes.
            // In a sense, out of all siblings that are smaller than the max size, we are
            // solving the knappsack problem here, greedily by assigning groups on a first come
            // first served basis to the lowest index group that has space for the new sibling.
            auto sibling_group_index = std::numeric_limits<size_t>::max();
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
                        sibling_group_index = i;
                        break;
                    }
                }
            }

            // If there was no such index, either we do not want to merge siblings, or all groups
            // are already filled to a point where adding this taxon would exceed the limit, or
            // the list is still empty. Either way, we want to make a new group then for the taxon.
            if( sibling_group_index == std::numeric_limits<size_t>::max() ) {
                sibling_group_index = sibling_groups.size();
                sibling_groups.emplace_back();
                sibling_groups.back().index = next_index;
                ++next_index;
            }

            // Now we know the group that we want to put this taxon into.
            // Set this, and then recursively set this group for all its children as well.
            auto& sibling_group = sibling_groups[sibling_group_index];
            sibling_group.clade_num_sequences += child_data.clade_num_sequences;
            sibling_group.clade_sum_seq_lengths += child_data.clade_sum_seq_lengths;
            child_data.group_status = KmerTaxonData::GroupStatus::kAssigned;
            child_data.group_index = sibling_group.index;
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
    preorder_for_each( tax, []( Taxon& taxon ){
        auto& data = taxon.data<KmerTaxonData>();
        data.group_status = KmerTaxonData::GroupStatus::kUnprocessed;
        data.group_index = std::numeric_limits<size_t>::max();
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
        auto const group_cnt = count_taxon_groups( tax );

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

// --------------------------------------------------------------------------
//     count_taxon_groups
// --------------------------------------------------------------------------

size_t count_taxon_groups( Taxonomy const& tax )
{
    // Iterate the taxonomy, recursing on expanded taxa, and counting unique group indices
    // for the taxa that are assigned to groups.
    std::unordered_set<size_t> group_indices;
    std::function<void(Taxonomy const&)> recursion_ = [&]( Taxonomy const& taxon ){
        for( auto const& child : taxon ) {
            auto& data = child.data<KmerTaxonData>();
            switch( data.group_status ) {
                case KmerTaxonData::GroupStatus::kAssigned: {
                    // For taxa that have been assigne to a group, collect their indices.
                    // As multiple taxa can be assigned to the same group (if the combined
                    // sizes are still within the limits of TaxonGroupingLimits),
                    // we need to count unique group indices here.
                    if( data.group_index == std::numeric_limits<size_t>::max() ) {
                        throw std::invalid_argument(
                            "Invalid KmerTaxonData::GroupStatus, invalid group index"
                        );
                    }
                    group_indices.insert( data.group_index );
                    break;
                }
                case KmerTaxonData::GroupStatus::kExpanded: {
                    // For taxa that have been expanded (because they are too big),
                    // we recurse instead.
                    recursion_( child );
                    break;
                }
                case KmerTaxonData::GroupStatus::kUnprocessed:
                default: {
                    throw std::invalid_argument(
                        "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                    );
                }
            }
        }
    };
    recursion_( tax );

    // Assert that group indices are consecutive.
    assert( [&](){
        for( size_t i = 0; i < group_indices.size(); ++i ) {
            if( group_indices.count( i ) == 0 ) {
                return false;
            }
        }
        return true;
    }() );

    // The number of groups is given by the number of unique group indices.
    return group_indices.size();
}

// =================================================================================================
//     User Reporting
// =================================================================================================

// --------------------------------------------------------------------------
//     grouped_taxonomy_trunk
// --------------------------------------------------------------------------

void grouped_taxonomy_trunk_( Taxonomy const& tax, Taxonomy& result )
{
    // Parallel recursion on the original Taxonomy and the resulting one:
    // If we decend into a taxon of the input, we also do so in the result,
    // and so at each stage we are in the same rank in both.
    // Basically, we are using the function call stack as our recursion pointer.
    // This way, in each step, we only have to deal with entries at the same level.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // For a taxon that is part of a group, we want to add a group as a pseudo-taxon
                // to the resulting taxonomy. This is a bit cumbersome, as it does not really fit
                // well with how taxonomies usually work...

                // First, make sure that a pseudo-taxon with the group name exists.
                std::string const group_name = "group_" + std::to_string( data.group_index );
                Taxon* group_tax = nullptr;
                if( result.has_child( group_name )) {
                    group_tax = &result.get_child( group_name );
                    assert( group_tax->data<KmerTaxonData>().group_index == data.group_index );
                } else {
                    group_tax = &result.add_child( group_name );
                    group_tax->reset_data( KmerTaxonData::create() );
                    auto& group_tax_data = group_tax->data<KmerTaxonData>();
                    group_tax_data.group_status = KmerTaxonData::GroupStatus::kAssigned;
                    group_tax_data.group_index = data.group_index;
                }

                // Then, accumulate the sizes of the current taxon to the group.
                auto& group_tax_data = group_tax->data<KmerTaxonData>();
                group_tax_data.clade_num_sequences += data.clade_num_sequences;
                group_tax_data.clade_sum_seq_lengths += data.clade_sum_seq_lengths;
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                assert( !result.has_child( child.name() ));
                auto& exp_tax = result.add_child( child.name() );
                exp_tax.reset_data( child.data<KmerTaxonData>().clone() );
                grouped_taxonomy_trunk_( child, exp_tax );
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed:
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                );
            }
        }
    }
}

Taxonomy grouped_taxonomy_trunk( Taxonomy const& tax )
{
    Taxonomy result;
    grouped_taxonomy_trunk_( tax, result );
    return result;
}

// --------------------------------------------------------------------------
//     grouped_taxonomy_report
// --------------------------------------------------------------------------

std::string grouped_taxonomy_report( Taxonomy const& tax )
{
    // Group statistics
    size_t num_groups = 0;
    std::vector<size_t> group_num_sequences;
    std::vector<size_t> group_sum_seq_lengths;

    // Ungrouped (expanded) statistics
    size_t num_ungrouped_taxa = 0;
    size_t ungrouped_num_sequences = 0;
    size_t ungrouped_sum_seq_lengths = 0;

    // Traverse the taxonomy to get all data
    for( auto const& taxon : preorder( tax )) {
        auto& data = taxon.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // The input to this function shall come from grouped_taxonomy_trunk(),
                // which creates pseudo-groups with this name prefix. Check this.
                if( !utils::starts_with( taxon.name(), "group_" )) {
                    throw std::invalid_argument( "Taxonomy is not the trunk" );
                }

                // Accumulate the statistics
                ++num_groups;
                group_num_sequences.push_back( data.clade_num_sequences );
                group_sum_seq_lengths.push_back( data.clade_sum_seq_lengths );
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                ++num_ungrouped_taxa;
                ungrouped_num_sequences += data.num_sequences;
                ungrouped_sum_seq_lengths += data.sum_seq_lengths;
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed:
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                );
            }
        }
    }

    // Quartiles computation needs sorted ranges
    std::sort( group_num_sequences.begin(), group_num_sequences.end() );
    std::sort( group_sum_seq_lengths.begin(), group_sum_seq_lengths.end() );

    // Produce a report from the accumulated statistics.
    size_t const grouped_num_sequences = std::accumulate(
        group_num_sequences.begin(), group_num_sequences.end(), static_cast<size_t>(0)
    );
    size_t const grouped_sum_seq_lengths = std::accumulate(
        group_sum_seq_lengths.begin(), group_sum_seq_lengths.end(), static_cast<size_t>(0)
    );
    auto const arithmetic_mean_ns = utils::arithmetic_mean(
        group_num_sequences.begin(), group_num_sequences.end()
    );
    auto const arithmetic_mean_sl = utils::arithmetic_mean(
        group_sum_seq_lengths.begin(), group_sum_seq_lengths.end()
    );
    auto const quartiles_ns = utils::quartiles(
        group_num_sequences.begin(), group_num_sequences.end()
    );
    auto const quartiles_sl = utils::quartiles(
        group_sum_seq_lengths.begin(), group_sum_seq_lengths.end()
    );
    std::stringstream ss;
    ss << num_groups << " groups:\n\n";
    utils::Table summary;
    summary.add_column( "stat" );
    summary.add_column( "num_sequences",   utils::Table::Column::Justification::kRight );
    summary.add_column( "sum_seq_lengths", utils::Table::Column::Justification::kRight );
    summary << "sum" << grouped_num_sequences << grouped_sum_seq_lengths;
    summary << "avg" << arithmetic_mean_ns << arithmetic_mean_sl;
    summary << "q0"  << quartiles_ns.q0 << quartiles_sl.q0;
    summary << "q1"  << quartiles_ns.q1 << quartiles_sl.q1;
    summary << "q2"  << quartiles_ns.q2 << quartiles_sl.q2;
    summary << "q3"  << quartiles_ns.q3 << quartiles_sl.q3;
    summary << "q4"  << quartiles_ns.q4 << quartiles_sl.q4;
    ss << summary.to_string( utils::simple_grid() );
    ss << "\n" << num_ungrouped_taxa << " ungrouped taxa:\n\n";
    summary.clear_content();
    summary << "sum" << ungrouped_num_sequences << ungrouped_sum_seq_lengths;
    ss << summary.to_string( utils::simple_grid() );
    ss << "\n";
    return ss.str();
}

// =================================================================================================
//     Json Format
// =================================================================================================

// --------------------------------------------------------------------------
//     write_taxonomy_grouping_to_json
// --------------------------------------------------------------------------

void fill_json_array_with_taxonomy_groups_(
    Taxonomy const& tax,
    utils::JsonDocument::ArrayType& array
) {
    using utils::JsonDocument;

    // Iterate the children, adding their data to the groups they were assigned to.
    for( auto const& child : tax ) {
        auto& data = child.data<KmerTaxonData>();
        switch( data.group_status ) {
            case KmerTaxonData::GroupStatus::kAssigned: {
                // Create entries in the resulting json array for the indices of groups up until
                // we have an entry for the group of the current taxon, and init as needed.
                while( array.size() <= data.group_index ) {
                    array.push_back( JsonDocument::object() );
                    auto& obj = array.back().get_object();
                    obj["group_index"] = JsonDocument::number_unsigned( array.size() - 1 );
                    obj["taxa"] = JsonDocument::array();
                    obj["num_sequences"] = JsonDocument::number_unsigned( 0 );
                    obj["sum_seq_lengths"] = JsonDocument::number_unsigned( 0 );
                }

                // We now have an entry in the array for the group index we need.
                // Amend that entry with the data of the current taxon.
                assert( array.size() > data.group_index );
                auto& entry = array[data.group_index].get_object();
                assert( entry["group_index"].get_number_unsigned() == data.group_index );
                entry["taxa"].get_array().push_back( JsonDocument::string(
                    TaxopathGenerator().to_string( child )
                ));
                entry["num_sequences"].get_number_unsigned() += data.clade_num_sequences;
                entry["sum_seq_lengths"].get_number_unsigned() += data.clade_sum_seq_lengths;

                // No recursion here - we already have all the information we need from the clade
                // sum variables, and we only want to store the highest tax rank of the taxon.
                break;
            }
            case KmerTaxonData::GroupStatus::kExpanded: {
                // If the child is not in a group itself, but was expanded, we recurse.
                fill_json_array_with_taxonomy_groups_( child, array );
                break;
            }
            case KmerTaxonData::GroupStatus::kUnprocessed:
            default: {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                );
            }
        }
    }
}

void write_taxonomy_grouping_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target
) {
    auto doc = utils::JsonDocument::array();
    fill_json_array_with_taxonomy_groups_( tax, doc.get_array() );
    utils::JsonWriter().write( doc, target );
}

// --------------------------------------------------------------------------
//     read_taxonomy_grouping_from_json
// --------------------------------------------------------------------------

std::vector<TaxonomyGroupData> read_taxonomy_grouping_from_json(
    std::shared_ptr<utils::BaseInputSource> source
) {
    std::vector<TaxonomyGroupData> result;
    auto doc = utils::JsonReader().read( source );
    auto& arr = doc.get_array();
    result.reserve( arr.size() );
    for( auto& child : arr ) {
        TaxonomyGroupData elem;
        elem.group_index = child.at( "group_index" ).get_number_unsigned();
        if( elem.group_index != result.size() ) {
            throw std::runtime_error(
                "Taxonomy grouping json file contains " + std::to_string( result.size() ) +
                "entries, but with non-consecutive group indices. "
                "Found group_index" + std::to_string( elem.group_index )
            );
        }
        elem.num_sequences = child.at( "num_sequences" ).get_number_unsigned();
        elem.sum_seq_lengths = child.at( "sum_seq_lengths" ).get_number_unsigned();
        for( auto const& tax : child.at( "taxa" ).get_array() ) {
            elem.taxa.push_back( tax.get_string() );
        }
        result.push_back( std::move( elem ));
        child.clear();
    }
    return result;
}

// --------------------------------------------------------------------------
//     write_kmer_taxonomy_to_json
// --------------------------------------------------------------------------

void write_kmer_taxonomy_to_json(
    Taxonomy const& tax,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool with_group_data,
    bool only_trunk
) {
    using utils::JsonDocument;

    auto json_writer = TaxonomyJsonWriter();
    json_writer.taxon_to_json = [with_group_data](
        Taxon const& tax, JsonDocument::ObjectType& obj
    ){
        auto const& data = tax.data<KmerTaxonData>();
        obj[ "num_sequences" ] = JsonDocument::number_unsigned(
            data.num_sequences
        );
        obj[ "sum_seq_lengths" ] = JsonDocument::number_unsigned(
            data.sum_seq_lengths
        );
        if( with_group_data ) {
            obj[ "clade_num_sequences" ] = JsonDocument::number_unsigned(
                data.clade_num_sequences
            );
            obj[ "clade_sum_seq_lengths" ] = JsonDocument::number_unsigned(
                data.clade_sum_seq_lengths
            );
            switch( data.group_status ) {
                case KmerTaxonData::GroupStatus::kUnprocessed: {
                    obj[ "group_status" ] = JsonDocument::string( "unprocessed" );
                    break;
                }
                case KmerTaxonData::GroupStatus::kAssigned: {
                    obj[ "group_status" ] = JsonDocument::string( "assigned" );
                    obj[ "group_index" ] = JsonDocument::number_unsigned(
                        data.group_index
                    );
                    break;
                }
                case KmerTaxonData::GroupStatus::kExpanded: {
                    obj[ "group_status" ] = JsonDocument::string( "expanded" );
                    break;
                }
                default: {
                    throw std::invalid_argument( "Invalid KmerTaxonData::GroupStatus" );
                }
            }
        }
    };
    if( only_trunk ) {
        json_writer.recurse_taxon_condition = []( Taxon const& tax ){
            // If we only want to print the scaffold of the taxonomy after grouping,
            // we set a condition on the recursion such that we only recurse if the taxon
            // is expanded. If it is not expanded, we still write it (as it will be visited
            // in the TaxonomyJsonWriter), but do not recurse.
            auto const& data = tax.data<KmerTaxonData>();
            if(
                data.group_status != KmerTaxonData::GroupStatus::kAssigned &&
                data.group_status != KmerTaxonData::GroupStatus::kExpanded
            ) {
                throw std::invalid_argument(
                    "Taxonomy has not been grouped, as the group status of a taxon is set to "
                    "KmerTaxonData::GroupStatus::kUnprocessed"
                );
            }
            return ( data.group_status == KmerTaxonData::GroupStatus::kExpanded );
        };
    }
    json_writer.write( tax, target );
}

// --------------------------------------------------------------------------
//     read_kmer_taxonomy_from_json
// --------------------------------------------------------------------------

Taxonomy read_kmer_taxonomy_from_json(
    std::shared_ptr<utils::BaseInputSource> source
) {
    auto json_reader = TaxonomyJsonReader();
    json_reader.json_to_taxon = []( utils::JsonDocument::ObjectType const& obj, Taxon& tax ){
        tax.reset_data( KmerTaxonData::create() );
        auto& data = tax.data<KmerTaxonData>();
        data.num_sequences = obj.at( "num_sequences" ).get_number_unsigned();
        data.sum_seq_lengths = obj.at( "sum_seq_lengths" ).get_number_unsigned();

        // Check if the optional grouping-related elements are there, and set them.
        auto it = obj.begin();
        if(( it = obj.find( "clade_num_sequences" )) != obj.end() ) {
            data.clade_num_sequences = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "clade_sum_seq_lengths" )) != obj.end() ) {
            data.clade_sum_seq_lengths = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "group_index" )) != obj.end() ) {
            data.group_index = it->second.get_number_unsigned();
        }
        if(( it = obj.find( "group_status" )) != obj.end() ) {
            auto const& group_status = it->second.get_string();
            if( group_status == "unprocessed" ) {
                data.group_status = KmerTaxonData::GroupStatus::kUnprocessed;
            } else if( group_status == "assigned" ) {
                data.group_status = KmerTaxonData::GroupStatus::kAssigned;
            } else if( group_status == "expanded" ) {
                data.group_status = KmerTaxonData::GroupStatus::kExpanded;
            } else {
                throw std::invalid_argument(
                    "Invalid KmerTaxonData::GroupStatus == " + group_status
                );
            }
        }
    };
    return json_reader.read( source );
}

} // namespace taxonomy
} // namespace genesis
