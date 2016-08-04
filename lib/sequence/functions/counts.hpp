#ifndef GENESIS_SEQUENCE_FUNCTIONS_COUNTS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_COUNTS_H_

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

#include <cstddef> // size_t
#include <type_traits> // underlying_type_t
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceCounts;
class SequenceSet;

// =================================================================================================
//     Consensus
// =================================================================================================

std::string consensus_sequence(
    SequenceCounts const& counts,
    char                  gap_char        = '-',
    bool                  prefer_non_gaps = false
);

// =================================================================================================
//     Entropy
// =================================================================================================

/**
 * @brief Option flags to refine the calculation of site_entropy().
 *
 * The flags can be combined via the binary or operator `|`:
 *
 *     auto flags = SiteEntropyOptions::kIncludeGaps | SiteEntropyOptions::kNormalized;
 *
 * For checking whether a partcular option is set, use the binary and operator `&`:
 *
 *     if( flags & SiteEntropyOptions::kIncludeGaps ) {
 *         // ...
 *     }
 *
 * The option flags can be used with all functions that calculate the entropy. They are applied
 * on a per-site basis; i.e., they are used for calculating the site_entropy(), which is the
 * basis for higher level entropy functions like absolute_entropy() and averaged_entropy().
 */
enum class SiteEntropyOptions : unsigned char
{
    /**
     * @brief Default option, simply calculate the site entropy using the characters used in the
     * SequenceCounts object.
     */
    kDefault     = 0,

    /**
     * @brief In addition to the characters of the SequenceCounts object, use the undetermined and
     * gap characters.
     *
     * With this option, an additional term is added to the entropy, using the "rest" probability
     * of the site. The counts of all characters at a site in the SequenceCounts do not always add
     * up to the number of sequences that have been added. In cases where a Sequence contains gaps
     * or characters that are not set in the SequenceCounts object, those counts are simply ignored
     * when processing the Sequence and counting its sites.
     *
     * Using this ignored rest, an additional entropy term is calculated and added to the total
     * entropy, if this option is used.
     *
     * For example, if the SequenceCounts object counts all occurences of `A`, `C`, `G` and `T`,
     * and a Sequence added to it contains a gap `-` or a degenerated character like `W`, those
     * character are ignored in the counts. The total number of those characters is then:
     *
     *     auto gap_count = counts.added_sequences_count()
     *                    - counts.count_at( site_idx, 'A' )
     *                    - counts.count_at( site_idx, 'C' )
     *                    - counts.count_at( site_idx, 'G' )
     *                    - counts.count_at( site_idx, 'T' );
     *
     * This number is then used as an additional summand for the entropy:
     * \f$ H_{i} \f$ `+=` \f$ -f_{{-,i}}\times \log _{2}f_{{-,i}} \f$, with
     * \f$ f_{-,i} = \f$ `gap_count/counts.added_sequences_count()` being the relative frequency
     * of those gaps and undetermined characters.
     */
    kIncludeGaps = 1,

    /**
     * @brief Weight the entropy using the summed relative frequencies of the characters.
     *
     * The entropy per site depends on the frequencies of the chracters. However, per default,
     * gaps and other undetermined characters (those which are not used in the SequenceCounts object)
     * are igonred. Thus, the entropy for sites that contain mostly gaps might still have quite a
     * high value. In cases where mostly-gap sites shall add little to the total entropy, this
     * option can be used to reduce their influence. The site_entropy() is then weighted using the
     * sum of the frequencies of the determined sites.
     *
     * For example, if a site has fractions of 0.1 for all four characters `A`, `C`, `G` and `T`,
     * the resulting entropy is weighted with a factor of 0.4.
     *
     * See also SiteEntropyOptions::kIncludeGaps for a related option to take those into accoount.
     */
    kWeighted    = 2,

    /**
     * @brief Normalize the resulting entropy using the maximum entropy possible.
     *
     * This option results in entropy values in the range `[ 0.0, 1.0 ]`. This is achieved by
     * dividing the entropy by the maxmimal value that is possible given the used characters
     * of the SequenceCounts object. For example, with the four characters `A`, `C`, `G` and `T`,
     * the entropy is divided by \f$ log_2{(4)} \f$.
     *
     * If additionally the SiteEntropyOptions::kIncludeGaps flag is set, the divisor is calculated
     * using one additional value, e.g., \f$ log_2{(4+1)} \f$.
     */
    kNormalized  = 4
};

inline SiteEntropyOptions operator | ( SiteEntropyOptions lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    return static_cast< SiteEntropyOptions >(
        static_cast< T >( lhs ) | static_cast< T >( rhs )
    );
}

inline SiteEntropyOptions& operator |= ( SiteEntropyOptions& lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    lhs = static_cast< SiteEntropyOptions >(
        static_cast< T >( lhs ) | static_cast< T >( rhs )
    );
    return lhs;
}

inline bool operator & ( SiteEntropyOptions lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    return static_cast< T >( lhs ) & static_cast< T >( rhs );
}

double site_entropy(
    SequenceCounts const& counts,
    size_t                site_index,
    SiteEntropyOptions    options = SiteEntropyOptions::kDefault
);

double site_information(
    SequenceCounts const& counts,
    size_t                site_index,
    bool                  use_small_sample_correction = false,
    SiteEntropyOptions    options = SiteEntropyOptions::kDefault
);

double absolute_entropy(
    SequenceCounts const& counts,
    SiteEntropyOptions    per_site_options = SiteEntropyOptions::kDefault
);

double averaged_entropy(
    SequenceCounts const& counts,
    bool                  only_determined_sites = false,
    SiteEntropyOptions    per_site_options = SiteEntropyOptions::kDefault
);

} // namespace sequence
} // namespace genesis

#endif // include guard
