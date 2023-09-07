/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/text/char.hpp"

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Counts
// =================================================================================================

BaseCounts::size_type get_base_count( BaseCounts const& sample, char base )
{
    switch( base ) {
        case 'a':
        case 'A': {
            return sample.a_count;
        }
        case 'c':
        case 'C': {
            return sample.c_count;
        }
        case 'g':
        case 'G': {
            return sample.g_count;
        }
        case 't':
        case 'T': {
            return sample.t_count;
        }
        case 'n':
        case 'N': {
            return sample.n_count;
        }
        case 'd':
        case 'D':
        case '*':
        case '.':
        case '#': {
            return sample.d_count;
        }
    }
    throw std::runtime_error(
        "Invalid base character " + utils::char_to_hex( base )
    );
}

void set_base_count( BaseCounts& sample, char base, BaseCounts::size_type value )
{
    switch( base ) {
        case 'a':
        case 'A': {
            sample.a_count = value;
            break;
        }
        case 'c':
        case 'C': {
            sample.c_count = value;
            break;
        }
        case 'g':
        case 'G': {
            sample.g_count = value;
            break;
        }
        case 't':
        case 'T': {
            sample.t_count = value;
            break;
        }
        case 'n':
        case 'N': {
            sample.n_count = value;
            break;
        }
        case 'd':
        case 'D':
        case '*':
        case '.':
        case '#': {
            sample.d_count = value;
            break;
        }
        default: {
            throw std::runtime_error(
                "Invalid base character " + utils::char_to_hex( base )
            );
        }
    }
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
    std::array<double, 4> const avg_freqs = {{
        ( s1_freqs[0] + s2_freqs[0] ) / 2.0,
        ( s1_freqs[1] + s2_freqs[1] ) / 2.0,
        ( s1_freqs[2] + s2_freqs[2] ) / 2.0,
        ( s1_freqs[3] + s2_freqs[3] ) / 2.0
    }};

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

/**
 * @brief Local helper function that takes an already computed @p total from merge_base_counts(),
 * so that it can be re-used internally here.
 */
SortedBaseCounts sorted_base_counts_(
    Variant const& variant, bool reference_first, BaseCounts const& total
) {
    // We use sorting networks for speed here. See f_st_asymptotically_unbiased_a1n1a2n2()
    // for details on the technique.

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
                    "Cannot use reference base " + utils::char_to_hex( variant.reference_base ) +
                    "to sort base counts."
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

SortedBaseCounts sorted_base_counts(
    Variant const& variant, bool reference_first
) {
    auto const total = merge_base_counts( variant );
    return sorted_base_counts_( variant, reference_first, total );
}

// =================================================================================================
//     Merging
// =================================================================================================

size_t allele_count( BaseCounts const& sample )
{
    // We use bool to int conversion for simplicity, and to avoid branching.
    // For this, we use the fact that bool converts to 1/0 int.
    static_assert( static_cast<int>( true )  == 1, "Invalid bool(true) to int(1) conversion." );
    static_assert( static_cast<int>( false ) == 0, "Invalid bool(false) to int(0) conversion." );

    // Sum up the number of different ACGT counts that are present, to determine whether
    // this is a SNP, and whether it's biallelic.
    size_t al_count = 0;
    al_count += static_cast<int>( sample.a_count > 0 );
    al_count += static_cast<int>( sample.c_count > 0 );
    al_count += static_cast<int>( sample.g_count > 0 );
    al_count += static_cast<int>( sample.t_count > 0 );

    // Check and return the result.
    assert( al_count <= 4 );
    return al_count;
}

size_t allele_count( BaseCounts const& sample, size_t min_count )
{
    // We need to separate out the min_count == 0 case,
    // as we do not want to count alelles that are exactly 0...
    // We do this by setting min_count to 1, so that only true alelles are counted.
    if( min_count == 0 ) {
        min_count = 1;
    }

    // Sum up the number of different ACGT counts that are at least the min count,
    // to determine whether this is a SNP, and whether it's biallelic.
    size_t al_count = 0;
    al_count += static_cast<int>( sample.a_count >= min_count );
    al_count += static_cast<int>( sample.c_count >= min_count );
    al_count += static_cast<int>( sample.g_count >= min_count );
    al_count += static_cast<int>( sample.t_count >= min_count );

    // Check and return the result.
    assert( al_count <= 4 );
    return al_count;
}

size_t allele_count( BaseCounts const& sample, size_t min_count, size_t max_count )
{
    // Edge cases checks.
    if( max_count == 0 ) {
        return allele_count( sample, min_count );
    }
    if( max_count < min_count ) {
        throw std::invalid_argument(
            "Cannot compute allele_count() with max_count < min_count. "
            "min_count == " + std::to_string( min_count ) + ", "
            "max_count == " + std::to_string( max_count )
        );
    }

    // Same edge case as above.
    if( min_count == 0 ) {
        min_count = 1;
    }

    // Similar to the other functions above.
    size_t al_count = 0;
    al_count += static_cast<int>( sample.a_count >= min_count && sample.a_count <= max_count );
    al_count += static_cast<int>( sample.c_count >= min_count && sample.c_count <= max_count );
    al_count += static_cast<int>( sample.g_count >= min_count && sample.g_count <= max_count );
    al_count += static_cast<int>( sample.t_count >= min_count && sample.t_count <= max_count );
    return al_count;
}

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

std::pair<char, double> consensus( BaseCounts const& sample, bool is_covered )
{
    if( is_covered ) {
        return consensus( sample );
    } else {
        return { 'N', 0.0 };
    }
}

char guess_reference_base( Variant const& variant, bool force )
{
    auto const ref = utils::to_upper( variant.reference_base );
    if( ! force && is_valid_base( ref )) {
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
    if( ! force && is_valid_base( alt )) {
        return alt;
    } else {
        auto const ref = utils::to_upper( variant.reference_base );
        if( is_valid_base( ref )) {
            auto const sorted = sorted_base_counts( variant, true );
            if( sorted[1].count > 0 ) {
                return utils::to_upper( sorted[1].base );
            }
        }
    }

    // Else case outside, so that compilers always see that this function returns a value.
    return 'N';
}

void guess_and_set_ref_and_alt_bases( Variant& variant, bool force )
{
    // Get base data.
    auto ref = utils::to_upper( variant.reference_base );
    auto const alt = utils::to_upper( variant.alternative_base );

    // We only want to compute the total counts if necessary.
    BaseCounts total;
    bool computed_total = false;

    // Set the reference, unless it is already a good value (and we don't force it).
    // We don't flip the condition here, to keept it consistent with the above functions.
    if( ! force && is_valid_base( ref )) {
        // Nothing to do.
    } else {
        variant.reference_base = 'N';

        // Now we need the total base counts.
        total = merge_base_counts( variant );
        computed_total = true;

        // Use them to define our ref base.
        auto const sorted = sorted_base_counts( total );
        if( sorted[0].count > 0 ) {
            // Update the ref base. Also update our internal `ref` here, as we need it below.
            ref = utils::to_upper( sorted[0].base );
            variant.reference_base = ref;
        }
    }

    // Set the alternative.
    if( ! force && is_valid_base( alt )) {
        // Nothing to do.
    } else {
        variant.alternative_base = 'N';
        if( is_valid_base( ref )) {
            // Only compute the total if needed.
            if( ! computed_total ) {
                total = merge_base_counts( variant );
            }

            // Use it to define our alt base.
            auto const sorted = sorted_base_counts_( variant, true, total );
            if( sorted[1].count > 0 ) {
                variant.alternative_base = utils::to_upper( sorted[1].base );
            }
        }
    }
}

void guess_and_set_ref_and_alt_bases(
    Variant& variant,
    genesis::sequence::ReferenceGenome const& ref_genome,
    bool force
) {
    // Shouldn't happen from our parsing etc, but better safe than sorry.
    if( variant.position == 0 ) {
        throw std::runtime_error( "Invalid position 0 in Variant." );
    }

    // Get the reference sequence, and see if it is long enough. Throws if seq or base not present.
    assert( variant.position > 0 );
    auto const ref_base = ref_genome.get_base( variant.chromosome, variant.position );

    // Now use that reference base. If it is in ACGT, we use it as ref; if not, we check against
    // ambiguity codes to see if it fits with our count-based ref and alt bases instead.
    if( is_valid_base( ref_base )) {
        if( variant.reference_base != 'N' && variant.reference_base != ref_base ) {
            throw std::runtime_error(
                "At chromosome \"" + variant.chromosome + "\" position " +
                std::to_string( variant.position ) + ", the Reference Genome has base '" +
                std::string( 1, ref_base ) + "', while the Variant already has mismatching base '" +
                std::string( 1, variant.reference_base ) + "' set"
            );
        }

        // Now set the base, and obtain the alternative via our normal counting method.
        variant.reference_base = ref_base;
        variant.alternative_base = guess_alternative_base( variant, force );

    } else {
        // No usable ref base. Run the normal guessing.
        guess_and_set_ref_and_alt_bases( variant, force );

        // Now we cross check that the ref genome base is a valid base,
        // and also that it is an ambiguity char that contains either the ref or alt that we found.
        // If not, something is likely off...
        // This might be too rigurous though - will have to see in practice, and might change later.
        bool contains = false;
        try {
            using genesis::sequence::nucleic_acid_code_containment;
            contains |= nucleic_acid_code_containment( ref_base, variant.reference_base );
            contains |= nucleic_acid_code_containment( ref_base, variant.alternative_base );
        } catch(...) {
            // The above throws an error if the given bases are not valid.
            // Catch this, and re-throw a nicer, more understandable exception instead.
            throw std::runtime_error(
                "At chromosome \"" + variant.chromosome + "\" position " +
                std::to_string( variant.position ) + ", the Reference Genome has base '" +
                std::string( 1, ref_base ) + "', which is not a valid nucleic acid code"
            );
        }
        if( ! contains ) {
            throw std::runtime_error(
                "At chromosome \"" + variant.chromosome + "\" position " +
                std::to_string( variant.position ) + ", the reference base is '" +
                std::string( 1, variant.reference_base ) + "' and the alternative base is '" +
                std::string( 1, variant.alternative_base ) +
                "', determined from nucleotide counts in the data at this position. " +
                "However, the Reference Genome has base '" + std::string( 1, ref_base ) +
                "', which does not code for either of them, " +
                "and hence likely points to some kind of mismatch"
            );
        }
    }
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
