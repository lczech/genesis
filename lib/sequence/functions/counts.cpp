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
 * @ingroup sequence
 */

#include "sequence/functions/counts.hpp"

#include "sequence/counts.hpp"
#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"

#include "utils/core/logging.hpp"

namespace genesis {
namespace sequence {

// =================================================================================================
//     Consensus
// =================================================================================================

/**
 * @brief Calculate the consensus sequence by using the most frequent character at each site.
 *
 * The optional parameter `gap_char` (default value '-') is used for sites where no counts are
 * available, e.g. because all of them where undetermined or contained invalid characters in the
 * original sequences.
 *
 * Furthermore, if two or more characters have the same frequency, the first one is used. That is,
 * the one that appears first in SequenceCounts::characters().
 */
std::string consensus_sequence( SequenceCounts const& counts, char gap_char )
{
    std::string res;
    res.reserve( counts.length() );

    // Prepare some constants (speedup).
    auto const chars     = counts.characters();
    auto const num_chars = counts.characters().size();

    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {

        size_t                        max_pos = 0;
        SequenceCounts::CountsIntType max_val = 0;

        for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
            if( counts.count_at( site_idx, char_idx ) > max_val ) {
                max_pos = char_idx;
                max_val = counts.count_at( site_idx, char_idx );
            }
        }

        if( max_val > 0 ) {
            res += chars[ max_pos ];
        } else {
            res += gap_char;
        }
    }

    return res;
}

// =================================================================================================
//     Entropy
// =================================================================================================

/**
 * @brief Calculate the entropy at one site of a SequenceCounts object.
 *
 * The entropy \f$ H \f$ (uncertainty) at site \f$ i \f$ (= `site_idx`) is calculated as
 * \f$ H_{i}=-\sum f_{{c,i}}\times \log _{2}f_{{c,i}} \f$, where
 * \f$ f_{c,i} \f$ is the relative frequency of character \f$ c \f$ at site \f$ i \f$, summed
 * over all characters in the SequenceCounts object.
 */
double site_entropy( SequenceCounts const& counts, size_t site_idx )
{
    if( site_idx >= counts.length() ) {
        throw std::runtime_error(
            "Invalid site index for calculating site entropy: " + std::to_string( site_idx ) + "."
        );
    }

    // Prepare some constants (speedup).
    auto const ln_2      = log( 2.0 );
    auto const num_seqs  = static_cast<double>( counts.added_sequences_count() );
    auto const num_chars = counts.characters().size();

    double entropy = 0.0;
    for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
        double prob = static_cast<double>( counts.count_at( site_idx, char_idx )) / num_seqs;
        if( prob > 0.0 ) {
            entropy -= prob * log( prob ) / ln_2;
        }
    }
    return entropy;
}

/**
 * @brief Calculate the information content at one site of a SequenceCounts object.
 *
 * The information content \f$ R \f$ at site \f$ i \f$ (= `site_index`) is calculated as
 * \f$ R_{i} = \log_{2}( s ) - (H_{i}+e_{n}) \f$.
 *
 * Here, \f$ s \f$ is the number of possible characters in the sequences
 * (usually, 4 for nucleic acids and 20 for amino acids), which is taken from the
 * @link SequenceCounts::characters() characters@endlink used in the SequenceCounts object.
 * Furthermore, \f$ H_{i} \f$ is the site_entropy() at the given site.
 *
 * The optional term \f$ e_{n} \f$ is the small-sample correction, calculated as
 * \f$ e_{n}={\frac{1}{\ln {2}}}\times {\frac{s-1}{2n}} \f$, with \f$ n \f$ being the
 * @link SequenceCounts::added_sequences_count() number of sequences@endlink. It is only used if
 * `use_small_sample_correction` is set to `true` (default is `false`).
 */
double site_information(
    SequenceCounts const& counts,
    size_t site_index,
    bool use_small_sample_correction
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
    return log_num - site_entropy( counts, site_index ) - e;
}

/**
 * @brief Return the sum of all site entropies.
 *
 * This function simply sums up up the site_entropy() for all sites of the SequenceCount object.
 */
double absolute_entropy( SequenceCounts const& counts )
{
    double sum = 0.0;
    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        sum += site_entropy( counts, site_idx );
    }
    return sum;
}

/**
 * @brief Return the normalized sum of all site entropies.
 *
 * This function sums up up the site_entropy() for all sites of the SequenceCount object and
 * normalizes the result.
 *
 * If `only_determined_sites` is `false` (default), the normalization is done using the number of
 * sites, that is, it calculates the average entropy per site.
 *
 * If `only_determined_sites` is `true`, the normalization is done using the number of determined
 * sites only, that is, sites that only contain zeroes in all counts are skipped.
 * Those sites do not contribute entropy anyway. Thus, it calcuates the average entropy per
 * determiend site.
 */
double averaged_entropy( SequenceCounts const& counts, bool only_determined_sites )
{
    // Counters.
    double sum = 0.0;
    size_t determined_sites = 0;

    // Consts for speedup.
    auto const num_chars = counts.characters().size();

    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        sum += site_entropy( counts, site_idx );

        // Count determined sites.
        if( only_determined_sites ) {
            bool det = false;
            for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
                det |= ( counts.count_at( site_idx, char_idx ) > 0 );
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
