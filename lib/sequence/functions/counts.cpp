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
#include "sequence/functions/codes.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Consensus
// =================================================================================================

/**
 * @brief Calculate the majority rule consensus sequence by using the most frequent character at
 * each site.
 *
 * The function creates a consensus sequence by using the character at each position that has the
 * highest count (or frequency). It does not assume any character codes. Thus, it works for all
 * kinds of sequence codes, e.g., nucleic acid or amino acid codes.
 *
 * The optional parameter `allow_gaps` (default is `true`) determines whether gaps in the consensus
 * sequence are allowed. By default, if a site consists mostly of gaps, the consensus sequence also
 * contains a gap at that site. If however this option is set to `false`, the consensus sequence
 * will contain the most frequent non-gap character, even if there are more gaps at this site than
 * the character itself. In other words, if the parameter is set to `false`, gaps are treated as
 * missing characters instead of another type of character for computing the consensus.
 * The only exception are gaps-only sites; in this case, the resulting sites contain a gap
 * characters even if the parameter is set to `false`.
 *
 * The optional parameter `gap_char` (default value '-') is used for sites where no counts are
 * available (i.e., are all zero), or, if `allow_gaps` is set to `true`, for sites that contain
 * mostly gaps.
 *
 * Furthermore, if two or more characters have the same frequency, the first one is used. That is,
 * the one that appears first in SequenceCounts::characters().
 *
 * For an alternative version of this function that takes those ambiguities into account, see
 * consensus_sequence_with_ambiguities(). Also, for a version of this function that takes a
 * threshold for the character frequency into account, see consensus_sequence_with_threshold().
 * However, both of them currently only work for nucleic acid codes (`ACGT`).
 */
std::string consensus_sequence_with_majorities(
    SequenceCounts const& counts,
    bool                  allow_gaps,
    char                  gap_char
) {
    std::string result;
    result.reserve( counts.length() );

    // Prepare some constants for simplicity.
    auto const chars     = counts.characters();
    auto const seq_count = counts.added_sequences_count();
    auto const num_chars = counts.characters().size();

    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {

        size_t                        max_pos    = 0;
        SequenceCounts::CountsIntType max_val    = 0;
        SequenceCounts::CountsIntType counts_sum = 0;

        for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
            auto const char_count = counts.count_at( site_idx, char_idx );
            counts_sum += char_count;

            if( char_count > max_val ) {
                max_pos = char_idx;
                max_val = char_count;
            }
        }

        // We can never have a max higher than the total sum of counts, and this again cannot be
        // higher then the number of sequences that were added to the counts object.
        assert( max_val    <= counts_sum );
        assert( counts_sum <= seq_count  );

        // We write a code char if it is the majority, that is, > 0 and > all other code counts.
        // In other cases, write a gap. That is, either no code has a count > 0, or, if we allow
        // gaps and gaps are more frquent than actual codes.
        // Here, seq_count - counts_sum is the number of gaps at the current site.
        if(( max_val > 0 ) && (( ! allow_gaps ) || ( max_val > seq_count - counts_sum ))) {
            result += chars[ max_pos ];
        } else {
            result += gap_char;
        }
    }

    return result;
}

/**
 * @brief Calculate a consensus sequence by using the the most frequent characters at each site.
 *
 * The function calculates a consensus sequence for nucleic acid codes (`ACGT`), using their
 * ambiguity codes (e.g., `W` for "weak" == `AT`) if the counts (i.e., character frequencies) are
 * similar at a site. It uses `allowed_deviation` to decide which counts are close enough to each
 * other in order to be consiered ambiguous.
 *
 * For example, with `allowed_deviation == 0.0`, only exact matches are used, that is, if two
 * counts are exactly the same. Let `count('A') == 42` and `count('T') == 42`, and both other
 * counts be 0, this results in the code `W` at that site. If however `count('T') == 41`, only `A`
 * is used for the site. Thus, with `allowed_deviation == 0.0`, this function behaves very similar
 * to consensus_sequence_with_majorities(), except in cases were two counts are exaclty the same.
 *
 * On the other hand, with `allowed_deviation == 1.0`, all codes that are present at a site are
 * considered to be ambiguous. That is, if a site contains counts `> 0` for `A`, `G` and `T`, the
 * resulting site gets the code `D` (not C).
 *
 * For intermediate values, e.g., the default `0.1`, the value is used as a threshold to decide
 * the ambiguities. For example, let `count('A') == 42` and `count('T') == 38`, and both other
 * counts be 0. Then, the allowed deviation from the maximum 42 is `42 - 0.1 * 42 = 37.8`. Thus,
 * as the count for `T` is above this value, those two codes are considered ambiguous, resulting
 * in a `W` at that site. We call this threshold the allowed deviation range.
 *
 * The optional parameter `allow_gaps` (default is `true`) behaves similar to its counterpart in
 * consensus_sequence_with_majorities(). If set to `true`, the count of the gap character is also
 * considered. If then the count of no character is within the deviation range of the gap count, the
 * result contains a gap at that site. If however there are codes within the range, those are used
 * instead, even if gaps are more frequent.
 *
 * If `allow_gaps` is set to `false` instead, gaps are not considered. That means, the ambiguities
 * are calculated as if there were no gaps. So even if a site contains mostly gaps, but only a few
 * other characters, those will be used. Solely all-gap sites result in a gap at that site.
 *
 * Remark: As this function expects nucleic acid codes, the gap character is fixed to '-' here.
 * The ambiguity codes are converted using nucleic_acid_ambiguity_code(). See there for more
 * information on the used codes.
 *
 * If the provided SequenceCounts object does not use nucleic acid codes, or if the
 * `allowed_deviation` is not within the range `[ 0.0, 1.0 ]`, an exception is thrown.
 */
std::string consensus_sequence_with_ambiguities(
    SequenceCounts const& counts,
    double                allowed_deviation,
    bool                  allow_gaps
) {
    // Check whether the counts object uses the needed character codes for this function.
    // The characters in the counts object are sorted, so we can directly compare then like this.
    if( counts.characters() != "ACGT" ) {
        throw std::runtime_error(
            "Computation of consensus_sequence_with_ambiguities() only possible "
            "for nucleic acid codes (ACGT)."
        );
    }

    // Check the deviation range.
    if( allowed_deviation < 0.0 || allowed_deviation > 1.0 ) {
        throw std::invalid_argument(
            "Value of allowed_deviation has to be in range [ 0.0, 1.0 ]."
        );
    }

    std::string result;
    result.reserve( counts.length() );

    // Prepare some constants for simplicity.
    auto const chars     = counts.characters();
    auto const seq_count = counts.added_sequences_count();
    auto const num_chars = counts.characters().size();

    // Use a hard coded gap char here, as we have fixed character codes anyway.
    auto const gap_char  = '-';

    // We expect ACGT here.
    assert( num_chars == 4 );

    // Helper struct to store the codes sorted by count.
    using CountPair = std::pair< size_t, char >;
    auto count_pair_compare = []( CountPair const& lhs, CountPair const& rhs ) {
        return lhs.first > rhs.first;
    };

    // Process all sites of the sequence.
    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {

        // Total sum of counts. Used for getting the number of gaps.
        SequenceCounts::CountsIntType counts_sum = 0;

        // Map from counts to positions. We use this for sorting by count. It's a vector, because
        // it will only have 4 or 5 elements, thus this should be faster than complex containers.
        std::vector< CountPair > counts_map;
        counts_map.reserve( 4 );

        // Add all chars with their counts to the map.
        for( size_t char_idx = 0; char_idx < num_chars; ++char_idx ) {
            auto const char_count = counts.count_at( site_idx, char_idx );
            counts_sum += char_count;
            counts_map.push_back({ char_count, chars[ char_idx ] });
        }

        // We can never have a sum of counts higher then the number of sequences that were added
        // to the counts object.
        assert( counts_sum <= seq_count );

        // We already checked that the counts object is for nucleic acids.
        // Thus, we expect four values here.
        assert( counts_map.size() == 4 );

        // If we want to use gaps as a normal character, add their count to the map.
        // This special case is a bit tricky to handle. We want to compare the gap count with all
        // other frequencies, so it makes sense to just treat it as a normal character here.
        // However, we want to avoid ending up with an invalid ambiguity code like "-AT", so we
        // still need to treat gaps separately (see below).
        if( allow_gaps ) {
            counts_map.push_back({ seq_count - counts_sum, gap_char });
        }

        // Sort the counts so that the highest one is first.
        std::sort( counts_map.begin(), counts_map.end(), count_pair_compare );

        // Prepare a string of characters codes for the ambiguities.
        std::string ambiguity_codes;

        // Check if this is an all gaps site. If not, we can do the following computation.
        if( counts_map[0].first > 0 ) {

            // Initialize the ambiguity string to the most frequent character code, unless
            // the most frequent one is the gap.
            // This is because if there are other characters that are within the deviation range,
            // we want to prefer them, and not end up with an ill-formed list like "-AT", but just
            // "AT". Only if no other character is as frequent as the gap (and not in the deviation
            // range), we want to use the gap char. If this is the case, the following loop will
            // not add any char to the ambiguity codes, thus it ends up being empty.
            // This is later checked and the gap char is added if needed.
            if( counts_map[0].second != gap_char ) {
                ambiguity_codes = std::string( 1, counts_map[0].second );
            }

            // Every character that has at least this count is added to the ambiguity.
            auto const deviation_threshold = ( 1.0 - allowed_deviation )
                                           * static_cast<double>( counts_map[0].first );

            // Compare the less frequent codes to the most frequent one and
            // decide whether to add them to the ambiguities.
            for( size_t i = 1; i < counts_map.size(); ++i ) {
                auto const cur_count = static_cast<double>( counts_map[i].first );

                // If the count is within the deviation range (and not a gap), add it.
                // We also avoid zero counts, as this leads to wrong results with an
                // allowed_deviation of 1.0. It would then just add all, ending up with all "N"s,
                // instead of just all codes that appear in the sequence.
                if( cur_count > 0.0 && cur_count >= deviation_threshold ) {
                    if( counts_map[i].second != gap_char ) {
                        ambiguity_codes += counts_map[i].second;
                    }

                // If not, we can stop here. The map is sorted,
                // so no other count will be high enough.
                } else {
                    break;
                }
            }

        // If the first entry in the map has zero counts, that means that this is an all-gaps site:
        // The range is sorted, so if the first one is zero, all are. That also implies that
        // allow_gaps is false, because otherwise an all-gaps site would have the highest count in
        // the map for gaps (unless the counts object did not have any sequences added to it...).
        } else {
            assert( ! allow_gaps || seq_count == 0 );
        }

        // Special case: If the most frequent char is the gap, and no other char is withing the
        // deviation range, we want to output a gap char. We did not add it before, so do it now.
        if( ambiguity_codes.size() == 0 ) {
            ambiguity_codes += gap_char;
        }

        // Get the ambiguity code that represents the selected characters and add it to the seq.
        result += nucleic_acid_ambiguity_code( ambiguity_codes );
    }

    return result;
}

std::string consensus_sequence_with_threshold(
    SequenceCounts const& counts,
    double                frequency_threshold,
    bool                  allow_gaps
) {
    (void) counts;
    (void) frequency_threshold;
    (void) allow_gaps;
    throw std::runtime_error( "Not yet implemented." );
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
 *
 * The function additionally takes optional flags to refine the calculation, see
 * ::SiteEntropyOptions for their explanation.
 */
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
        auto char_count = counts.count_at( site_idx, char_idx );
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

/**
 * @brief Calculate the information content at one site of a SequenceCounts object.
 *
 * The information content \f$ R \f$ at site \f$ i \f$ (= `site_index`) is calculated as
 * \f$ R_{i} = \log_{2}( s ) - (H_{i}+e_{n}) \f$.
 *
 * Here, \f$ s \f$ is the number of possible characters in the sequences
 * (usually, 4 for nucleic acids and 20 for amino acids), which is taken from the
 * @link SequenceCounts::characters() characters()@endlink used in the SequenceCounts object.
 * Furthermore, \f$ H_{i} \f$ is the site_entropy() at the given site.
 *
 * The optional term \f$ e_{n} \f$ is the small-sample correction, calculated as
 * \f$ e_{n}={\frac{1}{\ln {2}}}\times {\frac{s-1}{2n}} \f$, with \f$ n \f$ being the
 * @link SequenceCounts::added_sequences_count() number of sequences@endlink. It is only used if
 * `use_small_sample_correction` is set to `true` (default is `false`).
 *
 * The function additionally takes optional flags to refine the site entropy calculation,
 * see ::SiteEntropyOptions for their explanation.
 */
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

/**
 * @brief Return the sum of all site entropies.
 *
 * This function simply sums up up the site_entropy() for all sites of the SequenceCount object.
 * The function additionally takes optional flags to refine the site entropy calculation,
 * see ::SiteEntropyOptions for their explanation.
 */
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

/**
 * @brief Return the averaged sum of all site entropies.
 *
 * This function sums up up the site_entropy() for all sites of the SequenceCount object and
 * returns the average result per site.
 *
 * If `only_determined_sites` is `false` (default), the average is calculated using the total
 * number of sites, that is, it simply calculates the average entropy per site.
 *
 * If `only_determined_sites` is `true`, the average is calculated using the number of determined
 * sites only; that is, sites that only contain zeroes in all counts are skipped.
 * Those sites do not contribute entropy anyway. Thus, it calcuates the average entropy per
 * determiend site.
 *
 * The function additionally takes optional flags to refine the site entropy calculation,
 * see ::SiteEntropyOptions for their explanation.
 */
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
