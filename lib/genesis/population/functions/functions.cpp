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

#include "genesis/population/functions/functions.hpp"

#include "genesis/utils/io/char.hpp"

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Status and Information
// =================================================================================================

BaseCountsStatus status(
    BaseCounts const& sample,
    size_t min_coverage,
    size_t max_coverage,
    size_t min_count,
    bool tolerate_deletions
) {
    BaseCountsStatus result;
    auto const nucleotide_count = nucleotide_sum( sample );

    // Set the min/max coverage related values.
    if(
        nucleotide_count > 0 && nucleotide_count >= min_coverage &&
        ( max_coverage == 0 || nucleotide_count <= max_coverage )
    ) {
        result.is_covered = true;

        // Sum up the number of different ACGT counts that are present, to determine whether
        // this is a SNP, and whether it's biallelic. We use bool to int conversion for simplicity,
        // and to avoid branching. For this, we use the fact that bool converts to 1/0 int.
        // We have a special case here for min_count == 0, in which case we do
        // not want to count a 0 as being "above" the min count. That would be riddiculous.
        static_assert( static_cast<int>( true )  == 1, "Invalid bool(true) to int(1) conversion." );
        static_assert( static_cast<int>( false ) == 0, "Invalid bool(false) to int(0) conversion." );
        size_t al_count = 0;
        al_count += static_cast<int>( sample.a_count > 0 && sample.a_count >= min_count );
        al_count += static_cast<int>( sample.c_count > 0 && sample.c_count >= min_count );
        al_count += static_cast<int>( sample.g_count > 0 && sample.g_count >= min_count );
        al_count += static_cast<int>( sample.t_count > 0 && sample.t_count >= min_count );

        // Determine type of SNP.
        if( al_count >= 2 ) {
            result.is_snp = true;
        }
        if( al_count == 2 ) {
            result.is_biallelic = true;
        }

        // Check deletions. We have the same special case as above here.
        if( sample.d_count > 0 && sample.d_count >= min_count && !tolerate_deletions ) {
            result.is_covered   = false;
            result.is_snp       = false;
            result.is_biallelic = false;
            result.is_ignored   = true;
        }
    }

    return result;
}

// =================================================================================================
//     Counts
// =================================================================================================

size_t get_base_count( BaseCounts const& bc, char base )
{
    switch( base ) {
        case 'a':
        case 'A': {
            return bc.a_count;
        }
        case 'c':
        case 'C': {
            return bc.c_count;
        }
        case 'g':
        case 'G': {
            return bc.g_count;
        }
        case 't':
        case 'T': {
            return bc.t_count;
        }
        case 'n':
        case 'N': {
            return bc.n_count;
        }
        case 'd':
        case 'D':
        case '*':
        case '.':
        case '#': {
            return bc.d_count;
        }
    }
    throw std::runtime_error(
        "Invalid base character " + utils::char_to_hex( base )
    );
}

BaseCounts total_base_counts( Variant const& variant )
{
    return merge( variant.samples );
}

// =================================================================================================
//     Sorting
// =================================================================================================

/**
 * @brief Local helper function that runs a sorting network to sort four values,
 * coming from the four nucleotides.
 *
 * The input are four values, either counts or frequencies. The output are the indices into this
 * array that are sorted so that the largest one comes first:
 *
 *     auto const data = std::array<T, 4>{ 15, 10, 20, 5 };
 *     auto const order = nucleotide_sorting_order_( data );
 *
 * yields `{ 2, 0, 1, 3 }`, so that `data[order[0]] = data[2] = 20` is the largest value,
 * `data[order[1]] = data[0] = 15` the second largest, and so forth.
 */
template<typename T>
std::array<size_t, 4> nucleotide_sorting_order_( std::array<T, 4> const& values )
{
    // Sort quickly via sorting network, putting large values first.
    // See https://stackoverflow.com/a/25070688/4184258
    auto indices = std::array<size_t, 4>{ 0, 1, 2, 3 };
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

SortedBaseCounts sorted_base_counts( BaseCounts const& sample )
{
    // Sort quickly via sorting network, putting large values first.
    // See https://stackoverflow.com/a/25070688/4184258
    // This is the same as above in nucleotide_sorting_order_(), but we here swap directly,
    // for speed, as a tradeoff against code duplication...
    SortedBaseCounts result = {
        'A', sample.a_count,
        'C', sample.c_count,
        'G', sample.g_count,
        'T', sample.t_count
    };
    if( result[0].count < result[1].count ) {
        std::swap( result[0], result[1] );
    }
    if( result[2].count < result[3].count ) {
        std::swap( result[2], result[3] );
    }
    if( result[0].count < result[2].count ) {
        std::swap( result[0], result[2] );
    }
    if( result[1].count < result[3].count ) {
        std::swap( result[1], result[3] );
    }
    if( result[1].count < result[2].count ) {
        std::swap( result[1], result[2] );
    }
    return result;
}

std::pair<SortedBaseCounts, SortedBaseCounts> sorted_average_base_counts(
    BaseCounts const& sample_a,
    BaseCounts const& sample_b
) {
    // Prepare result.
    auto result = std::pair<SortedBaseCounts, SortedBaseCounts>{};

    // Get frequencies in sample 1
    size_t const s1_cnts[] = {
        sample_a.a_count, sample_a.c_count, sample_a.g_count, sample_a.t_count
    };
    double const s1_nt_cnt = static_cast<double>( nucleotide_sum( sample_a )); // eucov
    double const s1_freqs[] = {
        static_cast<double>( sample_a.a_count ) / s1_nt_cnt,
        static_cast<double>( sample_a.c_count ) / s1_nt_cnt,
        static_cast<double>( sample_a.g_count ) / s1_nt_cnt,
        static_cast<double>( sample_a.t_count ) / s1_nt_cnt
    };

    // Get frequencies in sample 2
    size_t const s2_cnts[] = {
        sample_b.a_count, sample_b.c_count, sample_b.g_count, sample_b.t_count
    };
    double const s2_nt_cnt = static_cast<double>( nucleotide_sum( sample_b )); // eucov
    double const s2_freqs[] = {
        static_cast<double>( sample_b.a_count ) / s2_nt_cnt,
        static_cast<double>( sample_b.c_count ) / s2_nt_cnt,
        static_cast<double>( sample_b.g_count ) / s2_nt_cnt,
        static_cast<double>( sample_b.t_count ) / s2_nt_cnt
    };

    // Edge case. If there are no counts at all, we return empty.
    // The follow up function f_st_asymptotically_unbiased_nkdk() will also catch this edge case,
    // return zeros as well, and nothing will be added to the total F_ST sum.
    if( s1_nt_cnt == 0.0 || s2_nt_cnt == 0.0 ) {
        return result;
    }

    // Compute their averages.
    std::array<double, 4> const avg_freqs = {
        ( s1_freqs[0] + s2_freqs[0] ) / 2.0,
        ( s1_freqs[1] + s2_freqs[1] ) / 2.0,
        ( s1_freqs[2] + s2_freqs[2] ) / 2.0,
        ( s1_freqs[3] + s2_freqs[3] ) / 2.0
    };

    // Get the sorting order, based on the averages.
    auto const order = nucleotide_sorting_order_( avg_freqs );

    // Now they are sorted, largest ones first.
    assert( avg_freqs[order[0]] >= avg_freqs[order[1]] );
    assert( avg_freqs[order[1]] >= avg_freqs[order[2]] );
    assert( avg_freqs[order[2]] >= avg_freqs[order[3]] );

    // Prepare result. We use an array of the nucleotides to get them in the order as needed.
    static const char nts[] = {'A', 'C', 'G', 'T'};
    result.first[0]  = { nts[order[0]], s1_cnts[order[0]] };
    result.first[1]  = { nts[order[1]], s1_cnts[order[1]] };
    result.first[2]  = { nts[order[2]], s1_cnts[order[2]] };
    result.first[3]  = { nts[order[3]], s1_cnts[order[3]] };
    result.second[0] = { nts[order[0]], s2_cnts[order[0]] };
    result.second[1] = { nts[order[1]], s2_cnts[order[1]] };
    result.second[2] = { nts[order[2]], s2_cnts[order[2]] };
    result.second[3] = { nts[order[3]], s2_cnts[order[3]] };
    return result;
}

SortedBaseCounts sorted_base_counts(
    Variant const& variant, bool reference_first
) {
    // We use sorting networks for speed here. See f_st_asymptotically_unbiased_a1n1a2n2()
    // for details on the technique.

    auto const total = total_base_counts( variant );
    if( reference_first ) {
        SortedBaseCounts result;
        switch( variant.reference_base ) {
            case 'a':
            case 'A': {
                result[0] = { 'A', total.a_count };
                result[1] = { 'C', total.c_count };
                result[2] = { 'G', total.g_count };
                result[3] = { 'T', total.t_count };
                break;
            }
            case 'c':
            case 'C': {
                result[0] = { 'C', total.c_count };
                result[1] = { 'A', total.a_count };
                result[2] = { 'G', total.g_count };
                result[3] = { 'T', total.t_count };
                break;
            }
            case 'g':
            case 'G': {
                result[0] = { 'G', total.g_count };
                result[1] = { 'A', total.a_count };
                result[2] = { 'C', total.c_count };
                result[3] = { 'T', total.t_count };
                break;
            }
            case 't':
            case 'T': {
                result[0] = { 'T', total.t_count };
                result[1] = { 'A', total.a_count };
                result[2] = { 'C', total.c_count };
                result[3] = { 'G', total.g_count };
                break;
            }
            default: {
                throw std::runtime_error(
                    "Invalid reference base character " + utils::char_to_hex( variant.reference_base )
                );
            }
        }
        if( result[1].count < result[2].count ) {
            std::swap( result[1], result[2] );
        }
        if( result[1].count < result[3].count ) {
            std::swap( result[1], result[3] );
        }
        if( result[2].count < result[3].count ) {
            std::swap( result[2], result[3] );
        }
        return result;
    } else {
        return sorted_base_counts( total );
    }
}

// =================================================================================================
//     Merging
// =================================================================================================

void merge_inplace( BaseCounts& p1, BaseCounts const& p2 )
{
    // Make sure that we do not forget any fields in case of later refactoring of the struct.
    // Nope, can't do, as the compiler might allocate more to get proper memory alignment...
    // static_assert(
    //     sizeof( BaseCounts ) == 6 * sizeof( size_t ),
    //     "Unexpected number of member variables in BaseCounts class"
    // );

    p1.a_count += p2.a_count;
    p1.c_count += p2.c_count;
    p1.g_count += p2.g_count;
    p1.t_count += p2.t_count;
    p1.n_count += p2.n_count;
    p1.d_count += p2.d_count;
}

BaseCounts merge( BaseCounts const& p1, BaseCounts const& p2 )
{
    BaseCounts result = p1;
    merge_inplace( result, p2 );
    return result;
}

BaseCounts merge( std::vector<BaseCounts> const& p )
{
    BaseCounts result;
    for( auto const& ps : p ) {
        result.a_count += ps.a_count;
        result.c_count += ps.c_count;
        result.g_count += ps.g_count;
        result.t_count += ps.t_count;
        result.n_count += ps.n_count;
        result.d_count += ps.d_count;
    }
    return result;
}

// =================================================================================================
//     Consensus
// =================================================================================================

std::pair<char, double> consensus( BaseCounts const& sample )
{
    // Get total count/coverage with nucleotides.
    auto const nucleotide_count = nucleotide_sum( sample );

    // Only compute consensus if we have enough coverage. This can only be if we have
    // at least some counts. Assert that.
    if( nucleotide_count == 0 ) {
        return { 'N', 0.0 };
    }
    assert( nucleotide_count > 0 );
    assert( sample.a_count > 0 || sample.c_count > 0 || sample.g_count > 0 || sample.t_count > 0 );
    assert(
        sample.a_count + sample.c_count + sample.g_count + sample.t_count == nucleotide_count
    );

    // Fast way of comparing four variables and finding the name of the max one.
    // We don't want any expensive sorting on dynamic memory (vecors or the like),
    // but just do pairwise comparisons with indices instead. Let's not even use a loop
    // (loop unrolling), to be even faster. Four int copies and three comparisons. So smart.
    size_t const counts[] = { sample.a_count, sample.c_count, sample.g_count, sample.t_count };
    size_t max_idx = 0;
    if( counts[1] > counts[max_idx] ) {
        max_idx = 1;
    }
    if( counts[2] > counts[max_idx] ) {
        max_idx = 2;
    }
    if( counts[3] > counts[max_idx] ) {
        max_idx = 3;
    }

    // Now use the index to get the consensus character from a static lookup, and the confidence.
    static const char nts[] = {'A', 'C', 'G', 'T'};
    auto const conf
        = static_cast<double>( counts[max_idx] )
        / static_cast<double>( nucleotide_count )
    ;
    return { nts[max_idx], conf };
}

std::pair<char, double> consensus( BaseCounts const& sample, BaseCountsStatus const& status )
{
    if( status.is_covered ) {
        return consensus( sample );
    } else {
        return { 'N', 0.0 };
    }
}

char guess_reference_base( Variant const& variant )
{
    auto const ref = utils::to_upper( variant.reference_base );
    if( ref == 'A' || ref == 'C' || ref == 'G' || ref == 'T' ) {
        return ref;
    } else {
        auto const sorted = sorted_base_counts( variant, false );
        if( sorted[0].count > 0 ) {
            return utils::to_upper( sorted[0].base );
        }
    }

    // Last else case outside, so that compilers always see that this function returns a value.
    return 'N';
}

char guess_alternative_base( Variant const& variant, bool force )
{
    auto const alt = utils::to_upper( variant.alternative_base );
    if( ! force && ( alt == 'A' || alt == 'C' || alt == 'G' || alt == 'T' )) {
        return alt;
    } else {
        auto const ref = utils::to_upper( variant.reference_base );
        if( ref == 'A' || ref == 'C' || ref == 'G' || ref == 'T' ) {
            auto const sorted = sorted_base_counts( variant, true );
            if( sorted[1].count > 0 ) {
                return utils::to_upper( sorted[1].base );
            }
        }
    }

    // Else case outside, so that compilers always see that this function returns a value.
    return 'N';
}

// =================================================================================================
//     Output
// =================================================================================================

std::ostream& operator<<( std::ostream& os, BaseCounts const& bs )
{
    os << "A=" << bs.a_count << ", C=" << bs.c_count << ", G=" << bs.g_count;
    os << ", T=" << bs.t_count << ", N=" << bs.n_count << ", D=" << bs.d_count;
    return os;
}

} // namespace population
} // namespace genesis
