#ifndef GENESIS_POPULATION_FUNCTION_FUNCTIONS_H_
#define GENESIS_POPULATION_FUNCTION_FUNCTIONS_H_

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/sample_counts.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/utils/text/char.hpp"

#include <array>
#include <iosfwd>
#include <utility>
#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Bases and Counts
// =================================================================================================

/**
 * @brief Return whether a given base is in `ACGT`, case insensitive.
 */
inline constexpr bool is_valid_base( char c )
{
    // Can't use a function call here, to comply with C++11 constexpr rules.
    // c = utils::to_upper( c );
    return (
        c == 'A' || c == 'a' ||
        c == 'C' || c == 'c' ||
        c == 'G' || c == 'g' ||
        c == 'T' || c == 't'
    );
}

/**
 * @brief Return whether a given base is in `ACGTN`, case insensitive.
 */
inline constexpr bool is_valid_base_or_n( char c )
{
    // Can't use a function call here, to comply with C++11 constexpr rules.
    // c = utils::to_upper( c );
    return (
        c == 'A' || c == 'a' ||
        c == 'C' || c == 'c' ||
        c == 'G' || c == 'g' ||
        c == 'T' || c == 't' ||
        c == 'N' || c == 'n'
    );
}

/**
 * @brief Get the count for a @p base given as a char.
 *
 * The given @p base has to be one of `ACGTDN` (case insensitive), or `*#.` for deletions as well.
 */
SampleCounts::size_type get_base_count( SampleCounts const& sample, char base );

/**
 * @brief Set the count for a @p base given as a char.
 *
 * The given @p base has to be one of `ACGTDN` (case insensitive), or `*#.` for deletions as well.
 */
void set_base_count( SampleCounts& sample, char base, SampleCounts::size_type value );

// =================================================================================================
//     Sorting
// =================================================================================================

/**
 * @brief Return the sorting order of four values, for instance of the four nucleotides `ACGT`,
 * in descending order (largest first).
 *
 * The input are four values, either counts or frequencies. The output are the indices into this
 * array that are sorted so that the largest one comes first:
 *
 *     auto const data = std::array<T, 4>{ 15, 10, 20, 5 };
 *     auto const order = nucleotide_sorting_order( data );
 *
 * yields `{ 2, 0, 1, 3 }`, so that `data[order[0]] = data[2] = 20` is the largest value,
 * `data[order[1]] = data[0] = 15` the second largest, and so forth.
 *
 * Usage with actual data might be as follows:
 *
 *     SampleCounts sample = ...;
 *     auto const data = std::array<T, 4>{
 *         sample.a_count, sample.c_count, sample.g_count, sample.t_count
 *     };
 *     auto const order = nucleotide_sorting_order( data );
 *     // ...
 *
 * See also sample_counts_sorting_order() for an equivalent function that also considers the
 * "any" (`N`) and "deletion" (`D`) counts of a SampleCounts object.
 */
template<typename T>
std::array<size_t, 4> nucleotide_sorting_order( std::array<T, 4> const& values )
{
    // Sort quickly via sorting network, putting large values first.
    // See https://stackoverflow.com/a/25070688/4184258
    auto indices = std::array<size_t, 4>{{ 0, 1, 2, 3 }};
    if( values[indices[0]] < values[indices[1]] ) {
        std::swap( indices[0], indices[1] );
    }
    if( values[indices[2]] < values[indices[3]] ) {
        std::swap( indices[2], indices[3] );
    }
    if( values[indices[0]] < values[indices[2]] ) {
        std::swap( indices[0], indices[2] );
    }
    if( values[indices[1]] < values[indices[3]] ) {
        std::swap( indices[1], indices[3] );
    }
    if( values[indices[1]] < values[indices[2]] ) {
        std::swap( indices[1], indices[2] );
    }

    // Now they are sorted, largest ones first.
    assert( values[indices[0]] >= values[indices[1]] );
    assert( values[indices[1]] >= values[indices[2]] );
    assert( values[indices[2]] >= values[indices[3]] );

    return indices;
}

/**
 * @brief Return the sorting order of six values, for instance of the four nucleotides `ACGT` and
 * the `N` and `D` counts of a SampleCounts object, in descending order (largest first).
 *
 * Same as nucleotide_sorting_order(), but also taking `N` and `D` into account.
 * See there for details.
 */
template<typename T>
std::array<size_t, 6> sample_counts_sorting_order( std::array<T, 6> const& v )
{
    // Implementation inspired by https://stackoverflow.com/a/2792216/4184258
    // We did not test if this is faster than a sorting network here. Fast enough for now anyway.

    // Obtain the index at which each value ends up in the sorting order, largest one first.
    int i0 = (v[0] <  v[1]) + (v[0] <  v[2]) + (v[0] <  v[3]) + (v[0] <  v[4]) + (v[0] <  v[5]);
    int i1 = (v[1] <= v[0]) + (v[1] <  v[2]) + (v[1] <  v[3]) + (v[1] <  v[4]) + (v[1] <  v[5]);
    int i2 = (v[2] <= v[0]) + (v[2] <= v[1]) + (v[2] <  v[3]) + (v[2] <  v[4]) + (v[2] <  v[5]);
    int i3 = (v[3] <= v[0]) + (v[3] <= v[1]) + (v[3] <= v[2]) + (v[3] <  v[4]) + (v[3] <  v[5]);
    int i4 = (v[4] <= v[0]) + (v[4] <= v[1]) + (v[4] <= v[2]) + (v[4] <= v[3]) + (v[4] <  v[5]);
    int i5 = (v[5] <= v[0]) + (v[5] <= v[1]) + (v[5] <= v[2]) + (v[5] <= v[3]) + (v[5] <= v[4]);
    assert( i0 + i1 + i2 + i3 + i4 + i5 == 15 );

    // At those indices in the result, set the position that they need to point to.
    std::array<size_t, 6> order;
    order[i0] = 0;
    order[i1] = 1;
    order[i2] = 2;
    order[i3] = 3;
    order[i4] = 4;
    order[i5] = 5;

    // Now everything is sorted.
    assert( v[order[0]] >= v[order[1]] );
    assert( v[order[1]] >= v[order[2]] );
    assert( v[order[2]] >= v[order[3]] );
    assert( v[order[3]] >= v[order[4]] );
    assert( v[order[4]] >= v[order[5]] );
    return order;
}

/**
 * @brief Return the order of base counts (nucleotides), largest one first.
 */
SortedSampleCounts sorted_sample_counts( SampleCounts const& sample );

/**
 * @brief Return the sorted base counts of both input samples, orderd by the average
 * frequencies of the nucleotide counts in the two samples.
 *
 * Both returned counts will be in the same order, with the nucleotide first that has the highest
 * average count in the two samples, etc.
 */
std::pair<SortedSampleCounts, SortedSampleCounts> sorted_average_sample_counts(
    SampleCounts const& sample_a,
    SampleCounts const& sample_b
);

/**
 * @brief Get a list of bases sorted by their counts.
 *
 * If @p reference_first is set to `true`, the first entry in the resulting array is always
 * the reference base of the Variant, while the other three bases are sorted by counts.
 * If @p reference_first is set to `false`, all four bases are sorted by their counts.
 */
SortedSampleCounts sorted_sample_counts(
    Variant const& variant, bool reference_first, SampleCountsFilterPolicy filter_policy
);

// =================================================================================================
//     Allele Count
// =================================================================================================

/**
 * @brief Return the number of alleles, that is, of non-zero nucleotide counts of the @p sample.
 *
 * This looks at all four nucleotide counts (`ACGT`), and returns the number of them that are
 * non zero. The result hence is between 0 and 4, with 0 = no allele had any counts and 4 = all
 * alleles have a non-zero count.
 */
size_t allele_count( SampleCounts const& sample );

/**
 * @brief Return the number of alleles, taking a @p min_count into consideration, that is,
 * we compute the number of nucleotide counts of the @p sample that are at least the @p min_count.
 *
 * This looks at all four nucleotide counts (`ACGT`), and returns the number of them that are
 * at least the @p min_count. If `min_count == 0`, we instead call the allele_count(SampleCounts const&)
 * overload of this function that does not consider minimum counts.
 */
size_t allele_count( SampleCounts const& sample, size_t min_count );

/**
 * @brief Return the number of alleles, taking a @p min_count and @p max_count into consideration,
 * that is, we compute the number of nucleotide counts of the @p sample that are at least
 * @p min_count and at most @p max_count.
 *
 * This looks at all four nucleotide counts (`ACGT`), and returns the number of them that are
 * at least the @p min_count and at most @p max_count. If either of them is zero, they are not
 * taken into account though.
 */
size_t allele_count( SampleCounts const& sample, size_t min_count, size_t max_count );

// =================================================================================================
//     Merging
// =================================================================================================

/**
 * @brief Merge the counts of two SampleCounts%s, by adding the counts of the second (@p p2)
 * to the first (@p p1).
 */
void merge_inplace( SampleCounts& p1, SampleCounts const& p2 );

/**
 * @brief Merge the counts of two SampleCounts%s.
 */
SampleCounts merge( SampleCounts const& p1, SampleCounts const& p2 );

/**
 * @brief Merge the counts of a vector SampleCounts%s.
 */
SampleCounts merge( std::vector<SampleCounts> const& p, SampleCountsFilterPolicy filter_policy );

/**
 * @brief Merge the counts of a vector SampleCounts%s.
 */
inline SampleCounts merge_sample_counts( Variant const& v, SampleCountsFilterPolicy filter_policy )
{
    return merge( v.samples, filter_policy );
}

/**
 * @brief Count of the pure nucleotide bases at this position, that is,
 * the sum of all `A`, `C`, `G`, and `T`.
 *
 * This is simply the sum of `a_count + c_count + g_count + t_count`, which we often use as the
 * read depth at the given site.
 *
 * NB: In PoPoolation, this variable is called `eucov`.
 */
inline constexpr size_t nucleotide_sum( SampleCounts const& sample )
{
    return sample.a_count + sample.c_count + sample.g_count + sample.t_count;
}

/**
 * @copybrief nucleotide_sum( SampleCounts const& )
 *
 * See nucleotide_sum() for details. This function gives the sum over all samples in the Variant.
 */
inline size_t total_nucleotide_sum( Variant const& variant, SampleCountsFilterPolicy filter_policy )
{
    return nucleotide_sum( merge_sample_counts( variant, filter_policy ));
}

/**
 * @brief Sum up all the base counts at this @p sample, that is, the sum of all `A`, `C`, `G`, `T`,
 * as well as the `N` and `D` count for indetermined and deleted counts.
 *
 * This is simply the sum of `a_count + c_count + g_count + t_count + n_count + d_count`, of the
 * SampleCounts object. See nucleotide_sum() for a function that only sums `ACGT`, but not `N` and `D`.
 */
inline constexpr size_t sample_counts_sum( SampleCounts const& sample )
{
    return
        sample.a_count +
        sample.c_count +
        sample.g_count +
        sample.t_count +
        sample.n_count +
        sample.d_count
    ;
}

/**
 * @copybrief sample_counts_sum( SampleCounts const& )
 *
 * See sample_counts_sum() for details. This function gives the sum over all samples in the Variant.
 */
inline size_t total_sample_counts_sum( Variant const& variant, SampleCountsFilterPolicy filter_policy )
{
    return sample_counts_sum( merge_sample_counts( variant, filter_policy ));
}

// =================================================================================================
//     Consensus
// =================================================================================================

/**
 * @brief Consensus character for a SampleCounts, and its confidence.
 *
 * This is simply the character (out of `ACGT`) that appears most often (or, for ties,
 * the lexicographically smallest character), unless all of (`A`, `C`, `G`, `T`) are zero,
 * in which case the consensus character is `N`.
 * The confidence is the count of the consensus character, divided by the total count
 * of all four nucleotides.
 */
std::pair<char, double> consensus( SampleCounts const& sample );

/**
* @brief Consensus character for a SampleCounts, and its confidence.
*
* This is simply the character (out of `ACGT`) that appears most often (or, for ties,
* the lexicographically smallest character). If @p is_covered is false (meaning, the position is
* not well covered by reads), the consensus character is `N`.
* The confidence is the count of the consensus character, divided by the total count
* of all four nucleotides.
*/
std::pair<char, double> consensus( SampleCounts const& sample, bool is_covered );

/**
 * @brief Guess the reference base of a Variant.
 *
 * If the Variant already has a `reference_base` in `ACGT`, this base is returned (meaning that
 * this function is idempotent; it does not change the reference base if there already is one).
 * However, if the `reference_base` is `N` or any other value not in `ACGT`, or if @p force is `true`,
 * the base with the highest count is returned instead,
 * unless all counts are 0, in which case the returned reference base is `N`.
 */
char guess_reference_base(
    Variant const& variant,
    bool force = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

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
char guess_alternative_base(
    Variant const& variant,
    bool force = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

/**
 * @brief Guess the reference and alternative bases for a Variant, and set them.
 *
 * This uses the same approach as guess_reference_base() and guess_alternative_base(),
 * but is more efficient than calling both in sequence. See there for details.
 */
void guess_and_set_ref_and_alt_bases(
    Variant& variant,
    bool force = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

/**
 * @brief Guess the reference and alternative bases for a Variant, and set them,
 * using a given reference base.
 *
 * This uses the same approach as guess_and_set_ref_and_alt_bases( Variant&, bool ), but
 * additionally consideres the given @p ref_base. If the reference base contains a value
 * in `ACGT` (case insensitive) at the position of the @p variant, it is used as the reference.
 * Note that the function throws an exception should the reference base already be set to a different
 * value that is not code for the base of the Variant, in order to notify users that something is off.
 * That is, we do check for ambiguity codes, and if the reference base is an ambiguous base that
 * contains the one already set in the Variant, this is okay. An exception is thrown on mismatch
 * only.
 *
 * If the reference base is `N` though, the function behaves the same as its reference-free
 * overload of the function. For the alternative base, it always uses the most abundant base
 * that is not the reference, same as its alternative function.
 */
void guess_and_set_ref_and_alt_bases(
    Variant& variant,
    char ref_base,
    bool force = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

/**
 * @brief Guess the reference and alternative bases for a Variant, and set them,
 * using a given reference genme to obtain the base.
 *
 * This simply calls guess_and_set_ref_and_alt_bases( Variant&, char, bool ) with the base given
 * by the @p ref_genome. See there for details.
 */
void guess_and_set_ref_and_alt_bases(
    Variant& variant,
    genesis::sequence::ReferenceGenome const& ref_genome,
    bool force = false,
    SampleCountsFilterPolicy filter_policy = SampleCountsFilterPolicy::kOnlyPassing
);

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Output stream operator for SampleCounts instances.
 */
std::ostream& operator<<( std::ostream& os, SampleCounts const& bs );

} // namespace population
} // namespace genesis

#endif // include guard
