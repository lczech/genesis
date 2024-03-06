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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/functions/variant_input_stream.hpp"

#include "genesis/population/functions/subsample.hpp"

#include <cassert>
#include <stdexcept>
#include <unordered_set>

namespace genesis {
namespace population {

// =================================================================================================
//     Sample Name Filter
// =================================================================================================

std::vector<bool> make_sample_name_filter(
    std::vector<std::string> const& sample_names,
    std::vector<std::string> const& names_filter,
    bool inverse_filter
) {
    // Turn filter into set for fast access. We don't do a simple iterator-based copy here,
    // as we at the same time also want to check for duplicates.
    std::unordered_set<std::string> filter_set;
    for( auto const& name : names_filter ) {
        if( filter_set.count( name ) != 0 ) {
            throw std::invalid_argument(
                "Cannot apply sample name filter, as filter name \"" + name + "\" appears "
                "multiple times in the list of names used for filtering."
            );
        }
        filter_set.insert( name );
    }
    assert( filter_set.size() == names_filter.size() );

    // Now go through the names, and check if they are to be filtered or not. At the same time,
    // we also build a set of those names, for duplication check. Whenever we have processed a name
    // from the filter set, we remove it there, so that if anything remains at the end, we know
    // that it did not appear in the sample names list.
    std::unordered_set<std::string> names_set;
    auto result = std::vector<bool>( sample_names.size(), false );
    for( size_t i = 0; i < sample_names.size(); ++i ) {
        auto const& name = sample_names[i];

        // Duplicate check.
        if( names_set.count( name ) > 0 ) {
            throw std::invalid_argument(
                "Cannot apply sample name filter, as sample name \"" + name + "\" appears "
                "multiple times in the sample names."
            );
        }
        names_set.insert( name );

        // Filter, and remove from filter.
        bool const found = ( filter_set.count( name ) > 0 );
        result[i] = ( found ^ inverse_filter );
        if( found ) {
            filter_set.erase( name );
        }
    }

    // Check if there are any remaining names in the filter list. If so, that's an error.
    if( filter_set.size() > 0 ) {
        throw std::invalid_argument(
            "Cannot apply sample name filter, as the list of names to filter contains names that "
            "do not appear in the sample names, such as \"" + *filter_set.begin() + "\"."
        );
    }

    // All good, return the filter vector.
    return result;
}

std::function<void(Variant&)> make_variant_input_stream_sample_name_filter_transform(
    std::vector<bool> const& sample_filter
) {
    // We do a pop count once, and store that result in the lambda, so that we do not need
    // to recompute this every time that the filter is being used.
    size_t pop_count = 0;
    for( size_t i = 0; i < sample_filter.size(); ++i ) {
        if( sample_filter[i] ) {
            ++pop_count;
        }
    }
    assert( pop_count <= sample_filter.size() );

    // We make a copy of the filter, to be sure that it is alive when needed.
    return [ sample_filter, pop_count ]( Variant& variant ){
        if( variant.samples.size() != sample_filter.size() ) {
            throw std::runtime_error(
                "Invalid sample filter, which filters a list of " +
                std::to_string( sample_filter.size() ) + " samples, while the Variant has " +
                std::to_string( variant.samples.size() ) + " samples instead."
            );
        }

        // Allocate a samples vector of the right size, and move samples around.
        std::vector<BaseCounts> samples;
        samples.reserve( pop_count );
        for( size_t i = 0; i < sample_filter.size(); ++i ) {
            if( sample_filter[i] ) {
                samples.push_back( std::move( variant.samples[i] ));
            }
        }
        assert( samples.size() == pop_count );
        variant.samples = std::move( samples );
    };
}

// =================================================================================================
//     Sample Subsetting / Subsampling
// =================================================================================================

std::function<void(Variant&)> make_variant_input_stream_sample_subsampling_transform(
    size_t max_coverage,
    SubsamplingMethod method
) {
    // Simply return a function object that applies the transformation.
    switch( method ) {
        case SubsamplingMethod::kSubscale: {
            return [ max_coverage ]( Variant& variant ){
                transform_subscale( variant, max_coverage );
            };
        }
        case SubsamplingMethod::kSubsampleWithReplacement: {
            return [ max_coverage ]( Variant& variant ){
                transform_subsample_with_replacement( variant, max_coverage );
            };
        }
        case SubsamplingMethod::kSubsampleWithoutReplacement: {
            return [ max_coverage ]( Variant& variant ){
                transform_subsample_without_replacement( variant, max_coverage );
            };
        }
    }

    throw std::invalid_argument(
        "Invalid method provided for make_variant_input_stream_sample_subsampling_transform()"
    );
}

// =================================================================================================
//     Observers
// =================================================================================================

std::function<void(Variant const&)> make_variant_input_stream_sequence_order_observer(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict,
    bool check_sequence_lengths
) {
    // We create copies of stuff here using lambda capture.
    // Those will then live on once the lambda is in use.
    GenomeLocus current_locus;
    return [ sequence_dict, check_sequence_lengths, current_locus ]( Variant const& variant ) mutable {
        // When a dict is provided, we need to take care of the special empty case for the chromosome
        // name, which will happen in the very first time that this callback is used.
        // The empty string won't be part of the dict chromosomes. For simplicity,
        // we here always check that case. We later check that the variant itself does not have
        // an empty chromsome name, in order to not regress to this starting condition.
        if( ! current_locus.empty() ) {

            // If a dict is provided, but the chromosome is not in there, the following check throws.
            if( ! locus_greater( variant.chromosome, variant.position, current_locus, sequence_dict )) {
                throw std::runtime_error(
                    "Invalid sorting order of input Variants. By default, we expect lexicographical "
                    "sorting of chromosomes, and then sorting by position within chromosomes. "
                    "Alternatively, when a sequence dictionary is specified (such as from a .dict "
                    "or .fai file, or from a reference genome .fasta file), we expect the order of "
                    "chromosomes as specified there. "
                    "Offending input going from " + to_string( current_locus ) + " to " +
                    to_string( GenomeLocus{ variant.chromosome, variant.position })
                );
            }
        }

        // Now also check the length, potentially.
        if( check_sequence_lengths && sequence_dict ) {
            auto const& entry = sequence_dict->get( variant.chromosome );
            if( variant.position > entry.length ) {
                throw std::runtime_error(
                    "The current position " +
                    to_string( GenomeLocus{ variant.chromosome, variant.position }) +
                    " of the input Variant is greater than the length of the chromosome "
                    "as specified by the SequenceDict, which is " + std::to_string( entry.length )
                );
            }
        }

        // Finally, update the current locus according to the variant.
        // We also check that the current variant is a valid, so that we do not run into
        // the initial condition of an empty chromosome again.
        current_locus = GenomeLocus( variant.chromosome, variant.position );
        if( current_locus.chromosome.empty() || current_locus.position == 0 ) {
            throw std::runtime_error(
                "Invalid empty chromosome or position 0 found in input Variant."
            );
        }
    };
}

std::function<void(Variant const&)> make_variant_input_stream_sequence_length_observer(
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict
) {
    return [ sequence_dict ]( Variant const& variant ) {
        // Assumes the chromosome is in the dict. If not, the get function throws.
        auto const& entry = sequence_dict->get( variant.chromosome );
        if( variant.position > entry.length ) {
            throw std::runtime_error(
                "The current position " +
                to_string( GenomeLocus{ variant.chromosome, variant.position }) +
                " of the input Variant is greater than the length of the chromosome "
                "as specified by the SequenceDict, which is " + std::to_string( entry.length )
            );
        }
    };
}

} // namespace population
} // namespace genesis
