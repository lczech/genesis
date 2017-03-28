#ifndef GENESIS_SEQUENCE_FUNCTIONS_ENTROPY_H_
#define GENESIS_SEQUENCE_FUNCTIONS_ENTROPY_H_

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

#include <cstddef>     // size_t
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
     *                    - counts.count_of( 'A', site_idx )
     *                    - counts.count_of( 'C', site_idx )
     *                    - counts.count_of( 'G', site_idx )
     *                    - counts.count_of( 'T', site_idx );
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

/**
 * @brief Or-operator to combine two SiteEntropyOptions%s.
 *
 * Typical usage:
 *
 *     auto options = SiteEntropyOptions::kWeighted | SiteEntropyOptions::kNormalized;
 *
 * Use the @link operator&( SiteEntropyOptions, SiteEntropyOptions ) and-operator@endlink
 * in order to check whether an option is set.
 */
inline SiteEntropyOptions operator | ( SiteEntropyOptions lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    return static_cast< SiteEntropyOptions >(
        static_cast< T >( lhs ) | static_cast< T >( rhs )
    );
}

/**
 * @brief Or-assignment-operator to combine two SiteEntropyOptions%s.
 *
 * Typical usage:
 *
 *     SiteEntropyOptions options;
 *     options |= SiteEntropyOptions::kWeighted;
 *
 * Use the @link operator&( SiteEntropyOptions, SiteEntropyOptions ) and-operator@endlink
 * in order to check whether an option is set.
 */
inline SiteEntropyOptions& operator |= ( SiteEntropyOptions& lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    lhs = static_cast< SiteEntropyOptions >(
        static_cast< T >( lhs ) | static_cast< T >( rhs )
    );
    return lhs;
}

/**
 * @brief And-operator to check whether a SiteEntropyOptions is set.
 *
 * Typical usage:
 *
 *     SiteEntropyOptions options;
 *     if( options & SiteEntropyOptions::kWeighted ) {
 *         // Do stuff...
 *     }
 *
 * Use the @link operator|( SiteEntropyOptions, SiteEntropyOptions ) or-operator@endlink
 * in order to set and combine options.
 */
inline bool operator & ( SiteEntropyOptions lhs, SiteEntropyOptions rhs )
{
    using T = std::underlying_type< SiteEntropyOptions >::type;
    return static_cast< T >( lhs ) & static_cast< T >( rhs );
}

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
    size_t                site_index,
    SiteEntropyOptions    options = SiteEntropyOptions::kDefault
);

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
    bool                  use_small_sample_correction = false,
    SiteEntropyOptions    options = SiteEntropyOptions::kDefault
);

/**
 * @brief Return the sum of all site entropies.
 *
 * This function simply sums up up the site_entropy() for all sites of the SequenceCount object.
 * The function additionally takes optional flags to refine the site entropy calculation,
 * see ::SiteEntropyOptions for their explanation.
 */
double absolute_entropy(
    SequenceCounts const& counts,
    SiteEntropyOptions    per_site_options = SiteEntropyOptions::kDefault
);

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
    bool                  only_determined_sites = false,
    SiteEntropyOptions    per_site_options = SiteEntropyOptions::kDefault
);

} // namespace sequence
} // namespace genesis

#endif // include guard
