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
 * @ingroup population
 */

#include "genesis/population/stream/variant_input_stream_adapters.hpp"

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Parallel Input Stream
// =================================================================================================

VariantInputStream make_variant_input_stream_from_variant_parallel_input_stream(
    VariantParallelInputStream const& parallel_input,
    VariantParallelInputStream::JoinedVariantParams const& joined_variant_params
) {
    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantParallelInputStream>( parallel_input );

    // Get the iterators.
    VariantParallelInputStream::Iterator cur;
    VariantParallelInputStream::Iterator end;
    bool has_started = false;

    // We do not have a single file here, so make a list of all sample names from the inputs.
    // Leave file_path and source_name at their empty defaults.
    VariantInputStreamData data;
    std::unordered_set<std::string> uniq_names;
    for( auto const& source : input->inputs() ) {
        // auto const& source_name = source.data().source_name;
        // for( auto const& sample_name : source.data().sample_names ) {
        //     data.sample_names.push_back( source_name + source_sample_separator + sample_name );
        // }

        for( auto const& sample_name : source.data().sample_names ) {
            if( uniq_names.count( sample_name ) > 0 ) {
                throw std::runtime_error(
                    "Cannot iterate input sources in parallel, as sample name \"" + sample_name +
                    "\" occurs multiple times in the inputs."
                );
            }
            uniq_names.insert( sample_name );
            data.sample_names.push_back( sample_name );
        }
    }
    assert( uniq_names.size() == data.sample_names.size() );

    // Set the params as needed. We want to move samples here, as we know we are not going to use
    // them any more in this function!
    auto params = joined_variant_params;
    params.move_samples = true;

    // The input is copied over to the lambda, and that copy is kept alive.
    return VariantInputStream(
        [ input, cur, end, has_started, params ]
        ( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }
            if( cur != end ) {
                variant = cur.joined_variant( params );
                ++cur;
                return true;
            } else {
                return false;
            }
        },
        std::move( data )
    );
}

// =================================================================================================
//     Variant Gapless Input Stream
// =================================================================================================

VariantInputStream make_variant_input_stream_from_variant_gapless_input_stream(
    VariantGaplessInputStream const& gapless_input
) {
    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantGaplessInputStream>( gapless_input );

    // Get the iterators.
    VariantGaplessInputStream::Iterator cur;
    VariantGaplessInputStream::Iterator end;
    bool has_started = false;

    // The input is copied over to the lambda, and that copy is kept alive.
    // The VariantInputStreamData is simply copied over.
    return VariantInputStream(
        [ input, cur, end, has_started ]
        ( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }
            if( cur != end ) {
                variant = std::move( *cur );
                ++cur;
                return true;
            } else {
                return false;
            }
        },
        input->input().data()
    );
}

// =================================================================================================
//     Merging Input Stream
// =================================================================================================

/**
 * @brief Internal helper to keep information needed for make_variant_merging_input_stream()
 */
struct VariantMergeGroupAssignment
{
    /**
     * @brief Assignment of samples (indicies in the vector) to their group (values).
     */
    std::vector<size_t> group_assignments;

    /**
     * @brief Names of the groups.
     *
     * The entries in the group_assignment refer to this list of groups.
     */
    std::vector<std::string> group_names;
};

/**
 * @brief Helper function to create a mapping from sample indices to group indices.
 */
VariantMergeGroupAssignment make_variant_merging_input_stream_group_assignment_(
    VariantInputStream const& variant_input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples
) {
    // Shorthands and checks
    auto const& sample_names = variant_input.data().sample_names;
    if( sample_names.size() == 0 ) {
        throw std::runtime_error( "Cannot merge sample groups if no sample names are provided" );
    }

    // Make a vector assinging sample indices to group indices.
    // We also keep track of the group names (and their indices) and samples names we have processed.
    VariantMergeGroupAssignment grouping;
    grouping.group_assignments = std::vector<size_t>( sample_names.size() );
    std::unordered_map<std::string, size_t> group_to_index;
    std::unordered_set<std::string> uniq_sample_names;

    // Do the assignment
    for( size_t i = 0; i < sample_names.size(); ++i ) {
        auto const& sample_name = sample_names[i];
        if( sample_name.empty() ) {
            throw std::runtime_error( "Cannot merge sample groups with empty sample names." );
        }

        // Check uniqueness of names. We also use that set later, so it's okay to have these
        // extra copies or the names around.
        if( uniq_sample_names.count( sample_name ) > 0 ) {
            throw std::runtime_error(
                "Cannot merge sample groups with duplicate sample names. Sample name \"" +
                sample_name + "\" occurs multiple times in the input."
            );
        }
        uniq_sample_names.insert( sample_name );

        // Get the group name for the sample. If it's in the given list, we use that.
        // If not, we either fail, or use the sample name as the group name.
        // In the latter case, there is some scenario where some provided sample names is also
        // a group name - that could be a bit chaotic and have weird consequences, but should
        // work, and so we just leave that as a user error and do not check for this here.
        std::string group_name;
        if( sample_name_to_group.count( sample_name ) > 0 ) {
            group_name = sample_name_to_group.at( sample_name );
            if( group_name.empty() ) {
                throw std::runtime_error(
                    "Cannot merge sample groups, as sample name \"" + sample_name + "\" has an " +
                    "empty group name assigned in the provided mapping of sample names to group names."
                );
            }
        } else if( allow_ungrouped_samples ) {
            group_name = sample_name;
        } else {
            throw std::runtime_error(
                "Cannot merge sample groups, as sample name \"" + sample_name +
                "\" does not occur in the provided mapping of sample names to group names."
            );
        }
        assert( !group_name.empty() );

        // Now we have a group name. Check if we already have an index for this group,
        // or whether this is a new group, so that we need to give it the next index.
        // Then assign this as the group index for the sample.
        if( group_to_index.count( group_name ) == 0 ) {
            auto const next_idx = group_to_index.size();
            group_to_index[ group_name ] = next_idx;
            grouping.group_names.push_back( group_name );
        }
        auto const group_idx = group_to_index.at( group_name );
        assert( group_idx < group_to_index.size() );
        assert( group_idx < grouping.group_names.size() );
        grouping.group_assignments[i] = group_idx;
    }
    assert( grouping.group_names.size() > 0 );
    assert( grouping.group_names.size() == group_to_index.size() );
    assert( uniq_sample_names.size() == sample_names.size() );

    // Finally, we warn about any names that have an assignment to a group, but did not appear.
    // Might indicate that something is wrong, but might also just mean that the user has some
    // larger mapping that does not always contain all names. Either way, better to tell.
    std::unordered_set<std::string> samples_names_to_warn;
    for( auto const& ng : sample_name_to_group ) {
        if( uniq_sample_names.count( ng.first ) == 0 ) {
            samples_names_to_warn.insert( ng.first );
        }
    }
    if( !samples_names_to_warn.empty() ) {
        LOG_WARN << "In the provided list of samples to merge into groups, there were "
                 << std::to_string( samples_names_to_warn.size() )
                 << " sample names that did not occur in the input sample names:\n"
                 << "  - " << genesis::utils::join( samples_names_to_warn, "  - " )
        ;
    }

    return grouping;
}

VariantInputStream make_variant_merging_input_stream(
    VariantInputStream const& variant_input,
    std::unordered_map<std::string, std::string> const& sample_name_to_group,
    bool allow_ungrouped_samples,
    SampleCountsFilterPolicy filter_policy
) {

    // Make a mapping from sample indices to group indices. That is, each entry in the returned
    // vector corresponds to a sample (by its index) of the input, and the value at each entry
    // is the group number we assign to this.
    auto const grouping = make_variant_merging_input_stream_group_assignment_(
        variant_input, sample_name_to_group, allow_ungrouped_samples
    );

    // As before, make a shared pointer (with a copy of the input) that stays alive.
    auto input = std::make_shared<VariantInputStream>( variant_input );

    // Get iterators to the data.
    VariantInputStream::Iterator cur;
    VariantInputStream::Iterator end;
    bool has_started = false;

    // We copy the original variant data, but replace the sample names by our group names.
    auto data = variant_input.data();
    data.sample_names = grouping.group_names;

    // Now we can create our iterator. As before, the iterators are copied over to the lambda,
    // and those copies are kept alive when returning from this function.
    return VariantInputStream(
        [ input, cur, end, has_started, grouping, filter_policy ]( Variant& variant ) mutable {
            if( ! has_started ) {
                assert( input );
                cur = input->begin();
                end = input->end();
                has_started = true;
            }

            // Nothing to do if we are at the end. Indicate that to the caller.
            if( cur == end ) {
                return false;
            }

            // For efficiency, we do not want to make a full copy of the input variant,
            // as that would entail an unncessary copy of the full samples vector.
            // However, we do want a copy of all other members of Variant, and listing them here
            // is a bit tedious. So instead we trick around a bit: We move out the samples,
            // then copy the rest, and then move the samples back. From the point of view of
            // the underlying input, that should appear as if nothing happened.
            // This also makes sure that any late additions to the Variant, such as the filter
            // status are correclty copied over.
            auto& cur_var = *cur;
            auto const sample_count = cur_var.samples.size();
            auto tmp_samples = std::move( cur_var.samples );
            variant = cur_var;
            cur_var.samples = std::move( tmp_samples );
            assert( cur_var.samples.size() == sample_count );

            // Consistency check the number of samples in the input.
            if( sample_count != grouping.group_assignments.size() ) {
                throw std::runtime_error(
                    "Based on sample names and groups, " +
                    std::to_string( grouping.group_assignments.size() ) +
                    " samples are expected to be found in the input, but " +
                    std::to_string( sample_count ) + " samples were found at " +
                    cur_var.chromosome + ":" + std::to_string( cur_var.position )
                );
            }

            // Now we create a new samples vector (no way around that), with SampleCounts instances
            // that are all initialized to 0 at all counts, and merge the samples into it,
            // as given by our group assignment.
            variant.samples = std::vector<SampleCounts>( grouping.group_names.size() );
            for( size_t i = 0; i < sample_count; ++i ) {
                auto const group_idx = grouping.group_assignments[i];

                // Validity check. Pretty sure that this could just be an assertion, as this
                // is all internal and not dependent on user input, but seems important enough
                // to just always check.
                if( group_idx >= variant.samples.size() ) {
                    throw std::runtime_error(
                        "Invalid group index " + std::to_string( group_idx ) +
                        " in Variant with " + std::to_string( variant.samples.size() ) +
                        " samples."
                    );
                }

                // Merge the sample, but only if we do not want to skip it due to it being filtered.
                if(
                    filter_policy == SampleCountsFilterPolicy::kOnlyPassing &&
                    ! cur_var.samples[i].status.passing()
                ) {
                    continue;
                }
                merge_inplace( variant.samples[group_idx], cur_var.samples[i] );
            }

            // We are done, move to the next position in the input,
            // and signal to the caller that we found data.
            ++cur;
            return true;
        },
        std::move( data )
    );
}

} // namespace population
} // namespace genesis
