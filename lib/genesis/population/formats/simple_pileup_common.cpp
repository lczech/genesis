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

#include "genesis/population/formats/simple_pileup_common.hpp"

#include "genesis/population/functions/functions.hpp"
#include "genesis/population/formats/simple_pileup_input_stream.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Simple (m)pileup Reader Helper Functions
// =================================================================================================

BaseCounts convert_to_base_counts(
    SimplePileupReader::Sample const& sample,
    unsigned char min_phred_score
) {
    BaseCounts result;

    // Tally up the bases.
    size_t total_count = 0;
    size_t skip_count  = 0;
    size_t rna_count   = 0;
    for( size_t i = 0; i < sample.read_bases.size(); ++i ) {

        // Quality control if available. Skip bases that are below the threshold.
        if( !sample.phred_scores.empty() && sample.phred_scores[i] < min_phred_score ) {
            ++skip_count;
            continue;
        }

        ++total_count;
        switch( sample.read_bases[i] ) {
            case 'a':
            case 'A': {
                ++result.a_count;
                break;
            }
            case 'c':
            case 'C': {
                ++result.c_count;
                break;
            }
            case 'g':
            case 'G': {
                ++result.g_count;
                break;
            }
            case 't':
            case 'T': {
                ++result.t_count;
                break;
            }
            case 'n':
            case 'N': {
                ++result.n_count;
                break;
            }
            case '*':
            case '#': {
                ++result.d_count;
                break;
            }
            case '<':
            case '>': {
                // Skipping RNA symbols. But count them, for sanity check.
                (void) rna_count;
                ++rna_count;
                break;
            }
            default: {
                throw std::runtime_error(
                    "Malformed pileup sample: Invalid allele character " +
                    utils::char_to_hex( sample.read_bases[i] )
                );
            }
        }
    }

    // Sanity checks and assertions.
    (void) total_count;
    assert(
        total_count ==
        result.a_count + result.c_count + result.g_count + result.t_count +
        result.n_count + result.d_count + rna_count
    );
    assert( skip_count + total_count == sample.read_bases.size() );

    // Sum sanity checks. There seems to be a very weird special case (found in the PoPoolation2
    // test dataset) where a line contains a deletion with a low phred score (`*`) that is not
    // counted in the "Number of reads covering this position" counter:
    // `  89795 2R      113608  N       1       T$      A       0       *       *`
    // We account for this here by allowing exactly one such base that is either a deletion
    // or a skip due to low phred score. There is no information that we know of about how
    // "empty" lines should be treated in pileip, so we have to guess, and that here seems to work.
    auto const base_count =
        result.a_count + result.c_count + result.g_count + result.t_count + result.n_count
    ;
    if(
        sample.read_bases.size() != sample.read_coverage &&
        !( base_count == 0 && result.d_count + skip_count == 1 )
    ) {

        throw std::runtime_error(
            "Malformed pileup sample: Given read count (" + std::to_string( sample.read_coverage ) +
            ") does not match the number of bases found in the sample (" +
            std::to_string( sample.read_bases.size() ) + ")"
        );
    }

    return result;
}

Variant convert_to_variant(
    SimplePileupReader::Record const& record,
    unsigned char min_phred_score
) {
    // Set basic data
    Variant result;
    result.chromosome     = record.chromosome;
    result.position       = record.position;
    result.reference_base = utils::to_upper( record.reference_base );

    // Convert the individual samples
    result.samples.reserve( record.samples.size() );
    for( size_t i = 0; i < record.samples.size(); ++i ) {
        result.samples.push_back( convert_to_base_counts( record.samples[i], min_phred_score ));
    }

    // Pileup does not contain ALT bases, so infer them from counts,
    // using the base with the most counts that is not the reference base.
    // We only do this if we have a reference base though, as otherwise, the sorting and alternative
    // is meaningless anyway. Only need to check upper case here, as we converted above.
    // Also, we do not set the alt base if it does not have any counts, and in that case is also
    // meaningless to have an alt base.
    if( is_valid_base( result.reference_base )) {
        auto const sorted = sorted_base_counts( result, true );
        if( sorted[1].count > 0 ) {
            result.alternative_base = utils::to_upper( sorted[1].base );
        }
    }

    return result;
}

genesis::sequence::QualityEncoding guess_pileup_quality_encoding(
    std::shared_ptr< utils::BaseInputSource > source,
    size_t max_lines
) {
    // Make a reader that uses quality scores.
    SimplePileupReader reader;
    reader.with_quality_string( true );

    // Start an iterator over the input.
    size_t line_cnt = 0;
    auto iter = SimplePileupInputStream<SimplePileupReader::Record>( source );
    while( iter ) {
        ++iter;
        ++line_cnt;
        if( max_lines > 0 && line_cnt >= max_lines ) {
            break;
        }
    }

    // Now get the accumulated counts of all quality codes, and guess the encoding.
    return genesis::sequence::guess_quality_encoding( iter.reader().quality_code_counts() );
}

} // namespace population
} // namespace genesis
