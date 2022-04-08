#ifndef GENESIS_POPULATION_FUNCTIONS_FUNCTIONS_H_
#define GENESIS_POPULATION_FUNCTIONS_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/variant.hpp"

#include <array>
#include <iosfwd>
#include <utility>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Status and Information
// =================================================================================================

struct BaseCountsStatus
{
    /**
     * @brief Is the Sample covered by enough reads/nucleotides?
     *
     * This value is set to `true` iff the total Sample::nucleotide_count (sum of Sample::a_count,
     * Sample::c_count, Sample::g_count, and Sample::t_count after testing that they are at least
     * min_count()) is in between the min_coverage() and max_coverage() values (inclusive),
     * and iff the amount of deletions (Sample::d_count) is not higher than min_count() as well
     * (unless, tolerate_deletions() is also set to `true`).
     *
     * That was a mouthful. Basically, a Sample is covered, if the sum of `A`, `C`, `G`, `T`
     * is in between min_coverage() and max_coverage(), and (unless we tolerate that) the amount
     * of deletions is not too high.
     */
    bool is_covered = false;

    /**
     * @brief Does the Sample have two or more alleles?
     *
     * That is the case if at least two of the `A`, `C`, `G`, `T` counts (Sample::a_count,
     * Sample::c_count, Sample::g_count, and Sample::t_count ) are above zero, after testing
     * that they are at least min_count().
     *
     * This value is also `false` if the amount of deletions (Sample::d_count) is too high
     * (using min_count() as the inclusive threshold), unless tolerate_deletions() is `true`.
     */
    bool is_snp = false;

    /**
     * @brief Is the Sample biallelic?
     *
     * This is closely related to Sample::is_snp, but only `true` iff the number of nucleotide
     * counts above zero is exactly two - that is, if there are only reads of two of `A`, `C`,
     * `G`, `T` in the sample.
     *
     * This value is also `false` if the amount of deletions (Sample::d_count) is too high
     * (using min_count() as the inclusive threshold), unless tolerate_deletions() is `true`.
     */
    bool is_biallelic = false;

    /**
     * @brief Is the Sample ignored due to high deletions count?
     *
     * This value is only set to `true` iff the Sample is well covered (as determined by
     * Sample::is_covered), but also has a high amount of deletions (at least min_count() many),
     * and if not also tolerate_deletions() is `true`.
     *
     * It is hence an indicator that there are too many deletions in the sample (if we decide
     * not to tolerate them).
     */
    bool is_ignored = false;
};

/**
 * @brief Compute a simple status with useful properties from the counts of a BaseCounts.
 *
 * ### <code>min_coverage</code>
 *
 * Minimum coverage expected for a BaseCounts to be considered "covered".
 * If the number of nucleotides (`A`, `C`, `G`, `T`) in the reads of a sample is less then the
 * here provided @p min_coverage, then the BaseCounts is not considered sufficiently covered,
 * and the BaseCountsStatus::is_covered flag will be set to `false`.
 *
 * ### <code>max_coverage</code>
 *
 * Same as @p min_coverage, but the upper bound on coverage; maximum coverage
 * expected for a BaseCounts to be considered "covered".
 * If the number of nucleotides exceeds this bound, the BaseCountsStatus::is_covered flag will
 * be set to `false`.
 * If provided with a value of `0` (default), max_coverage is not used.
 *
 * Only if the nucleotide count is in between (or equal to either) these two bounds (@p min_coverage
 * and @p max_coverage), it is considered to be covered, and BaseCountsStatus::is_covered
 * will be set to `true`.
 *
 * ### <code>min_count</code>
 *
 * This value is used to determine whether a BaseCounts has too many deletions,
 * and unless tolerate_deletions() is set to `true`, the BaseCountsStatus::is_ignored will be set
 * to `true` in that case (too many deletions, as given by BaseCounts::d_count), while the values for
 * BaseCountsStatus::is_covered, BaseCountsStatus::is_snp, and BaseCountsStatus::is_biallelic
 * will be set to `false`.
 *
 * Typically, if this function is used after calling filter_min_count() on the BaseCounts, the
 * @p min_count is set to the same value for consistency.
 *
 * ### <code>tolerate_deletions</code>
 *
 * Set whether we tolerate BaseCounts%s with a high amount of deletions.
 *
 * If set to `false` (default), we do not tolerate deletions. In that case, if the number of
 * deletions in a Sample (given by Sample::d_count) is higher than or equal to min_count(),
 * the Sample will be considered ignored (Sample::is_ignored set to `true`), and considered
 * not covered (Sample::is_covered, Sample::is_snp, and Sample::is_biallelic will all be set
 * to `false`).
 *
 * If however set to `true`, we tolerate high amounts of deletions, and the values for the above
 * properties will be set as usual by considering the nucleotide counts (Sample::a_count,
 * Sample::c_count, Sample::g_count, and Sample::t_count) instead.
 */
BaseCountsStatus status(
    BaseCounts const& sample,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
);

// =================================================================================================
//     Counts
// =================================================================================================

/**
 * @brief Get the count for a @p base given as a char.
 *
 * The given @p base has to be one of `ACGTDN` (case insensitive), or `*#.` for deletions as well.
 */
size_t get_base_count( BaseCounts const& bc, char base );

/**
 * @brief Get the summed up total base counts of a Variant.
 *
 * This is the same as calling merge() on the samples in the Variant.
 */
BaseCounts total_base_counts( Variant const& variant );

// =================================================================================================
//     Sorting
// =================================================================================================

/**
 * @brief Return the order of base counts (nucleotides), largest one first.
 */
SortedBaseCounts sorted_base_counts( BaseCounts const& sample );

/**
 * @brief Return the sorted base counts of both input samples, orderd by the average
 * frequencies of the nucleotide counts in the two samples.
 *
 * Both returned counts will be in the same order, with the nucleotide first that has the highest
 * average count in the two samples, etc.
 */
std::pair<SortedBaseCounts, SortedBaseCounts> sorted_average_base_counts(
    BaseCounts const& sample_a,
    BaseCounts const& sample_b
);

/**
 * @brief Get a list of bases sorted by their counts.
 *
 * If @p reference_first is set to `true`, the first entry in the resulting array is always
 * the reference base of the Variant, while the other three bases are sorted by counts.
 * If @p reference_first is set to `false`, all four bases are sorted by their counts.
 */
SortedBaseCounts sorted_base_counts(
    Variant const& variant, bool reference_first
);

// =================================================================================================
//     Merging
// =================================================================================================

/**
 * @brief Count of the pure nucleotide bases at this position, that is,
 * the sum of all `A`, `C`, `G`, and `T`.
 *
 * This is simply the sum of `a_count + c_count + g_count + t_count`, which we often use as the
 * coverage at the given site.
 *
 * NB: In PoPoolation, this variable is called `eucov`.
 */
inline size_t nucleotide_sum( BaseCounts const& sample )
{
    return sample.a_count + sample.c_count + sample.g_count + sample.t_count;
}

/**
 * @copybrief nucleotide_sum( BaseCounts const& )
 *
 * See nucleotide_sum() for details. This function gives the sum over all samples in the Variant.
 */
inline size_t total_nucleotide_sum( Variant const& variant )
{
    return nucleotide_sum( total_base_counts( variant ));
}

/**
 * @brief Merge the counts of two BaseCounts%s, by adding the counts of the second (@p p2)
 * to the first (@p p1).
 */
void merge_inplace( BaseCounts& p1, BaseCounts const& p2 );

/**
 * @brief Merge the counts of two BaseCounts%s.
 */
BaseCounts merge( BaseCounts const& p1, BaseCounts const& p2 );

/**
 * @brief Merge the counts of a vector BaseCounts%s.
 */
BaseCounts merge( std::vector<BaseCounts> const& p );

// =================================================================================================
//     Consensus
// =================================================================================================

/**
 * @brief Consensus character for a BaseCounts, and its confidence.
 *
 * This is simply the character (out of `ACGT`) that appears most often (or, for ties,
 * the lexicographically smallest character), unless all of (`A`, `C`, `G`, `T`) are zero,
 * in which case the consensus character is `N`.
 * The confidence is the count of the consensus character, divided by the total count
 * of all four nucleotides.
 */
std::pair<char, double> consensus( BaseCounts const& sample );

/**
* @brief Consensus character for a BaseCounts, and its confidence.
*
* This is simply the character (out of `ACGT`) that appears most often (or, for ties,
* the lexicographically smallest character). If the BaseCounts is not well covered by reads
* (that is, if its BaseCountsStatus::is_covered is `false`), the consensus character is `N`.
* The confidence is the count of the consensus character, divided by the total count
* of all four nucleotides.
*/
std::pair<char, double> consensus( BaseCounts const& sample, BaseCountsStatus const& status );

/**
 * @brief Guess the reference base of a Variant.
 *
 * If the Variant already has a `reference_base` in `ACGT`, this base is returned (meaning that
 * this function is idempotent; it does not change the reference base if there already is one).
 * However, if the `reference_base` is `N` or any other value not in `ACGT`,
 * the base with the highest count is returned instead,
 * unless all counts are 0, in which case the returned reference base is `N`.
 */
char guess_reference_base( Variant const& variant );

/**
 * @brief Guess the alternative base of a Variant.
 *
 * If the Variant already has an `alternative_base` in `ACGT` and @p force is not `true`,
 * this original base is returned (meaning that this function is idempotent; it does not change
 * the alternative base if there already is one).
 * However, if the `alternative_base` is `N` or any other char not in `ACGT`, or if @p force is `true`,
 * the base with the highest count that is not the reference base is returned instead.
 * This also means that the reference base has to be set to a value in `ACGT`,
 * as otherwise the concept of an alternative base is meaningless anyway.
 * If the reference base is not one of `ACGT`, the returned alternative base is `N`.
 * Furthermore, if all three non-reference bases have count 0, the returned alternative base is `N`.
 */
char guess_alternative_base( Variant const& variant, bool force = true );

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Output stream operator for BaseCounts instances.
 */
std::ostream& operator<<( std::ostream& os, BaseCounts const& bs );

} // namespace population
} // namespace genesis

#endif // include guard
