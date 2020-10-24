#ifndef GENESIS_POPULATION_FUNCTIONS_POOL_SAMPLE_H_
#define GENESIS_POPULATION_FUNCTIONS_POOL_SAMPLE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#include "genesis/population/pool_sample.hpp"
#include "genesis/population/formats/simple_pileup_reader.hpp"

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Status and Information
// =================================================================================================

struct PoolSampleStatus
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
 * @brief Compute a simple status with useful properties from the counts of a PoolSample.
 *
 * ### <code>min_coverage</code>
 *
 * Minimum coverage expected for a PoolSample to be considered "covered".
 * If the number of nucleotides (`A`, `C`, `G`, `T`) in the reads of a sample is less then the
 * here provided @p min_coverage, then the PoolSample is not considered sufficiently covered,
 * and the PoolSampleStatus::is_covered flag will be set to `false`.
 *
 * ### <code>max_coverage</code>
 *
 * Same as @p min_coverage, but the upper bound on coverage; maximum coverage
 * expected for a PoolSample to be considered "covered".
 * If the number of nucleotides exceeds this bound, the PoolSampleStatus::is_covered flag will
 * be set to `false`.
 * If provided with a value of `0` (default), max_coverage is not used.
 *
 * Only if the nucleotide count is in between (or equal to either) these two bounds (@p min_coverage
 * and @p max_coverage), it is considered to be covered, and PoolSampleStatus::is_covered
 * will be set to `true`.
 *
 * ### <code>min_count</code>
 *
 * This value is used to determine whether a PoolSample has too many deletions,
 * and unless tolerate_deletions() is set to `true`, the PoolSampleStatus::is_ignored will be set
 * to `true` in that case (too many deletions, as given by PoolSample::d_count), while the values for
 * PoolSampleStatus::is_covered, PoolSampleStatus::is_snp, and PoolSampleStatus::is_biallelic
 * will be set to `false`.
 *
 * Typically, if this function is used after calling filter_min_count() on the PoolSample, the
 * @p min_count is set to the same value for consistency.
 *
 * ### <code>tolerate_deletions</code>
 *
 * Set whether we tolerate PoolSample%s with a high amount of deletions.
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
PoolSampleStatus status(
    PoolSample const& sample,
    size_t min_coverage = 0,
    size_t max_coverage = 0,
    size_t min_count = 0,
    bool tolerate_deletions = false
);

// =================================================================================================
//     Accumulation, Filtering, etc
// =================================================================================================

/**
 * @brief Count of the pure nucleotide bases at this position, that is,
 * the sum of all `A`, `C`, `G`, and `T`.
 *
 * This is simply the sum of `a_count + c_count + g_count + t_count`.
 *
 * NB: In PoPoolation, this variable is called `eucov`.
 */
inline size_t nucleotide_sum( PoolSample const& sample )
{
    return sample.a_count + sample.c_count + sample.g_count + sample.t_count;
}

/**
 * @brief Merge the counts of two PoolSample%s.
 */
PoolSample merge( PoolSample const& p1, PoolSample const& p2 );

/**
 * @brief Merge the counts of a vector PoolSample%s.
 */
PoolSample merge( std::vector<PoolSample> const& p );

/**
 * @brief Filter by minimum count that we need for a type of nucleotide (`A`, `C`, `G`, `T`)
 * to be considered; set to zero if @p min_count is not reached.
 *
 * This filter is used as a type of quality control filter. All nucleotide counts (that is,
 * PoolSample::a_count, PoolSample::c_count, PoolSample::g_count, and PoolSample::t_count) that are
 * below the given @p min_count are set to zero.
 */
void filter_min_count( PoolSample& sample, size_t min_count );

/**
 * @brief Consensus character for a PoolSample, and its confidence.
 *
 * This is simply the character (out of `ACGT`) that appears most often (or, for ties,
 * the lexicographically smallest character), unless all of (`A`, `C`, `G`, `T`) are zero,
 * in which case the consensus character is `N`.
 * The confidence is the count of the consensus character, divided by the total count
 * of all four nucleotides.
 */
std::pair<char, double> consensus( PoolSample const& sample );

/**
* @brief Consensus character for a PoolSample, and its confidence.
*
* This is simply the character (out of `ACGT`) that appears most often (or, for ties,
* the lexicographically smallest character). If the PoolSample is not well covered by reads
* (that is, if its PoolSampleStatus::is_covered is `false`), the consensus character is `N`.
* The confidence is the count of the consensus character, divided by the total count
* of all four nucleotides.
*/
std::pair<char, double> consensus( PoolSample const& sample, PoolSampleStatus const& status );

// =================================================================================================
//     Conversion Functions
// =================================================================================================

PoolSample convert_to_pool_sample( SimplePileupReader::Sample const& sample );
PoolSampleSet convert_to_pool_samples( SimplePileupReader::Record const& record );

} // namespace population
} // namespace genesis

#endif // include guard
