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
 * @ingroup sequence
 */

#include "genesis/sequence/functions/entropy.hpp"

#include "genesis/sequence/counts.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Site Entropy
// =================================================================================================

double site_entropy(
    SequenceCounts const& counts,
    size_t                site_idx,
    SiteEntropyOptions    options
) {
    if( site_idx >= counts.length() ) {
        throw std::runtime_error(
            "Invalid site index for calculating site entropy: " + std::to_string( site_idx ) + "."
        );
    }

    // Prepare some constants (speedup).
    auto const ln_2      = log( 2.0 );
    auto const num_seqs  = static_cast<double>( counts.added_sequences_count() );
    auto const num_chars = counts.characters().size();

    // Results: we add up the entropy and the number of counts that we have seen in total.
    double entropy = 0.0;
    SequenceCounts::CountsIntType counts_sum = 0;

    // Accumulate entropy and total counts for the site.
    for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
        auto char_count = counts.count_at( char_idx, site_idx );
        counts_sum += char_count;

        double char_prob = static_cast<double>( char_count ) / num_seqs;
        if( char_prob > 0.0 ) {
            entropy -= char_prob * log( char_prob ) / ln_2;
        }
    }

    // If we want to include gaps, add the entropy for the gap probability.
    if( options & SiteEntropyOptions::kIncludeGaps ) {
        assert( counts_sum <= num_seqs );
        double gap_prob = 1.0 - ( static_cast<double>( counts_sum ) / num_seqs );
        if( gap_prob > 0.0 ) {
            entropy -= gap_prob * log( gap_prob ) / ln_2;
        }
    }

    // If we want to weight using the determined characters, use their proportion as a factor.
    if( options & SiteEntropyOptions::kWeighted ) {
        entropy *= ( static_cast<double>( counts_sum ) / num_seqs );
    }

    // If we want to normalize, calculate the H_max for the used number of characters.
    if( options & SiteEntropyOptions::kNormalized ) {
        double hmax = 1.0;
        if( options & SiteEntropyOptions::kIncludeGaps ) {
            hmax = log( static_cast<double>( num_chars + 1 )) / ln_2;
        } else {
            hmax = log( static_cast<double>( num_chars )) / ln_2;
        }
        return entropy / hmax;

    } else {
        return entropy;
    }
}

// =================================================================================================
//     Site Information
// =================================================================================================

double site_information(
    SequenceCounts const& counts,
    size_t                site_index,
    bool                  use_small_sample_correction,
    SiteEntropyOptions    options
) {
    // Max possible entropy for the given number of characters in the counts object.
    auto const num_chars = static_cast<double>( counts.characters().size() );
    auto const log_num   = log( num_chars ) / log( 2.0 );

    // Small sample correction.
    double e = 0.0;
    if( use_small_sample_correction ) {
        // Approximation for the small-sample correction, according to
        // https://en.wikipedia.org/wiki/Sequence_logo
        e = ( 1.0 / log( 2.0 ) ) * (( num_chars - 1.0 ) / ( 2.0 * counts.added_sequences_count() ));
    }

    // Result, using the entropy.
    return log_num - site_entropy( counts, site_index, options ) - e;
}

// =================================================================================================
//     Absolute Entropy
// =================================================================================================

double absolute_entropy(
    SequenceCounts const& counts,
    SiteEntropyOptions    per_site_options
) {
    double sum = 0.0;
    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        sum += site_entropy( counts, site_idx, per_site_options );
    }
    return sum;
}

// =================================================================================================
//     Averaged Entropy
// =================================================================================================

double averaged_entropy(
    SequenceCounts const& counts,
    bool                  only_determined_sites,
    SiteEntropyOptions    per_site_options
) {
    // Counters.
    double sum = 0.0;
    size_t determined_sites = 0;

    // Consts for speedup.
    auto const num_chars = counts.characters().size();

    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        sum += site_entropy( counts, site_idx, per_site_options );

        // Count determined sites.
        if( only_determined_sites ) {
            bool det = false;
            for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
                det |= ( counts.count_at( char_idx, site_idx ) > 0 );
            }
            if( det ) {
                ++determined_sites;
            }
        }
    }

    if( only_determined_sites ) {
        return sum / static_cast<double>( determined_sites );
    } else {
        return sum / static_cast<double>( counts.length() );
    }
}

} // namespace sequence
} // namespace genesis
