/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/population/functions/fst_cathedral.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Compute Data Functions
// =================================================================================================

std::vector<FstCathedralData> compute_fst_cathedral_data_chromosome_(
    VariantInputIterator::Iterator& iterator,
    FstPoolProcessor& processor,
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict
) {
    // Boundary check.
    if( ! iterator ) {
        return {};
    }

    // Make as many fst data objects as we have pairs of samples in the processor.
    // Each item in the vector is one pair of samples, contaning their per-position partial fst data.
    // We cannot resize the value field inside the results, as we do not know how large the
    // chromosome is that we are using beforehand.
    std::vector<FstCathedralData> result;
    result.resize( processor.size() );
    assert( processor.size() == processor.calculators().size() );

    // We are at some chromosome now. We want to iterate while on that chromosome.
    // We have a seq dict, so we know the length of the chromosome. First thought was to use that
    // to pre-allocate the value vectors, but that might waaay over-allocate, as likely non-snp
    // positions will be filtered out beforehand in the input iterator. So we don't do that.
    std::string const chromosome = iterator->chromosome;
    for( auto& data : result ) {
        data.chromosome = chromosome;
    }

    // Process all variants in the input as long as we are on the same chromosome,
    // and run them through the processor, storing all results in the result.
    size_t cur_pos = 0;
    while( iterator && iterator->chromosome == chromosome ) {
        auto const& variant = *iterator;
        processor.reset();
        processor.process( variant );

        // Make sure that we are in order. Otherwise the whole compuation fails.
        if( variant.position <= cur_pos ) {
            throw std::runtime_error(
                "Unsorted positions in input: On chromosome \"" + chromosome + "\", position " +
                std::to_string( variant.position ) + " follows position " +
                std::to_string( cur_pos ) + ", which is not in strict ordering."
            );
        }
        cur_pos = variant.position;

        // We need to cast the calculators in the processor to get the correct derived type,
        // so that we can access the partial pi values from there.
        // Bit hacky, but good enough for now. Then, store the results.
        assert( processor.size() == result.size() );
        for( size_t i = 0; i < processor.size(); ++i ) {
            auto& raw_calc = processor.calculators()[i];
            auto fst_calc = dynamic_cast<FstPoolCalculatorUnbiased const*>( raw_calc.get() );
            if( ! fst_calc ) {
                throw std::runtime_error(
                    "In compute_fst_cathedral_data_chromosome_(): Invalid FstPoolCalculatorUnbiased"
                );
            }

            // Now add the entry for the current calculator to its respective result entry.
            // We rely on the amortized complexity here - cannot pre-allocate the size,
            // as we do not know how many positions will actually be in the input beforehand.
            result[i].entries.emplace_back(
                variant.position,
                fst_calc->get_pi_within(),
                fst_calc->get_pi_between(),
                fst_calc->get_pi_total()
            );
        }

        // Done with the position. Move to the next.
        ++iterator;
    }

    // If we have an entry for it in the seq dict, we use that as the total length,
    // so that downstream plots show the correct length. If not, we use what's in the data.
    if( sequence_dict && sequence_dict->contains( chromosome )) {
        cur_pos = sequence_dict->get( chromosome ).length;
    }
    for( auto& data : result ) {
        data.chromosome_length = cur_pos;
    }

    return result;
}

std::pair<std::string, std::string> get_processor_sample_names_(
    FstPoolProcessor& processor,
    std::vector<std::string> const& sample_names,
    size_t processor_index
) {
    // This function is only called locally, so we can just assert that the index is correct.
    // No need for an exception here.
    assert( processor_index < processor.size() );

    // Without sample names given, we just return an empty pair.
    if( sample_names.empty() ) {
        return {};
    }

    assert( processor.sample_pairs().size() == processor.size() );
    auto& idx_pair = processor.sample_pairs()[processor_index];
    if(
        idx_pair.first  >= sample_names.size() ||
        idx_pair.second >= sample_names.size()
    ) {
        throw std::invalid_argument(
            "In compute_fst_cathedral_data(): sample names at indices " +
            std::to_string( idx_pair.first ) + " and " + std::to_string( idx_pair.second ) +
            " requested, but sample names with " + std::to_string( sample_names.size() ) +
            " entries given."
        );
    }

    return { sample_names[ idx_pair.first ], sample_names[ idx_pair.second ] };
}

std::vector<FstCathedralData> compute_fst_cathedral_data(
    VariantInputIterator& iterator,
    FstPoolProcessor& processor,
    std::vector<std::string> const& sample_names,
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict
) {
    // We make one big result vector with all entries from all pairs of samples and chromosomes.
    std::vector<FstCathedralData> result;

    // Start the iteration, process each chromosome, and move over the results.
    auto it = iterator.begin();
    while( it ) {
        auto chr_result = compute_fst_cathedral_data_chromosome_( it, processor, sequence_dict );
        assert( chr_result.size() == processor.size() );

        // Move the data for one chromsome (for each pair of samples) to the result.
        result.reserve( result.size() + chr_result.size() );
        for( size_t i = 0; i < chr_result.size(); ++i ) {
            // If sample names are given, use those to name the results.
            auto names = get_processor_sample_names_( processor, sample_names, i );
            chr_result[i].sample_name_1 = std::move( names.first );
            chr_result[i].sample_name_2 = std::move( names.second );

            // Now move it to our result vecor.
            result.push_back( std::move( chr_result[i] ));
        }
    }
    assert( it == iterator.end() );

    return result;
}

} // namespace population
} // namespace genesis
