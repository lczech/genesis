/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include "genesis/population/functions/base_counts.hpp"

#include "genesis/utils/io/char.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

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

// =================================================================================================
//     Accumulation and other processing
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

// =================================================================================================
//     Conversion Functions
// =================================================================================================

BaseCounts convert_to_base_counts(
    SimplePileupReader::Sample const& sample,
    unsigned char min_phred_score
) {
    BaseCounts result;

    // Tally up the bases.
    size_t total_count = 0;
    size_t skip_count  = 0;
    size_t rna_count   = 0;
    for( size_t i = 0; i < sample.read_bases.size(); ++i ) {

        // Quality control if available. Skip bases that are below the threshold.
        if( !sample.phred_scores.empty() && sample.phred_scores[i] < min_phred_score ) {
            ++skip_count;
            continue;
        }

        ++total_count;
        switch( sample.read_bases[i] ) {
            case 'a':
            case 'A': {
                ++result.a_count;
                break;
            }
            case 'c':
            case 'C': {
                ++result.c_count;
                break;
            }
            case 'g':
            case 'G': {
                ++result.g_count;
                break;
            }
            case 't':
            case 'T': {
                ++result.t_count;
                break;
            }
            case 'n':
            case 'N': {
                ++result.n_count;
                break;
            }
            case '*':
            case '#': {
                ++result.d_count;
                break;
            }
            case '<':
            case '>': {
                // Skipping RNA symbols. But count them, for sanity check.
                (void) rna_count;
                ++rna_count;
                break;
            }
            default: {
                throw std::runtime_error(
                    "Malformed pileup sample: Invalid allele character " +
                    utils::char_to_hex( sample.read_bases[i] )
                );
            }
        }
    }

    // Sanity checks and assertions.
    (void) total_count;
    assert(
        total_count ==
        result.a_count + result.c_count + result.g_count + result.t_count +
        result.n_count + result.d_count + rna_count
    );
    assert( skip_count + total_count == sample.read_bases.size() );

    // Sum sanity checks. There seems to be a very weird special case (found in the PoPoolation2
    // test dataset) where a line contains a deletion with a low phred score (`*`) that is not
    // counted in the "Number of reads covering this position" counter:
    // `  89795 2R      113608  N       1       T$      A       0       *       *`
    // We account for this here by allowing exactly one such base that is either a deletion
    // or a skip due to low phred score. There is no information that we know of about how
    // "empty" lines should be treated in pileip, so we have to guess, and that here seems to work.
    auto const base_count =
        result.a_count + result.c_count + result.g_count + result.t_count + result.n_count
    ;
    if(
        sample.read_bases.size() != sample.read_coverage &&
        !( base_count == 0 && result.d_count + skip_count == 1 )
    ) {

        throw std::runtime_error(
            "Malformed pileup sample: Given read count (" + std::to_string( sample.read_coverage ) +
            ") does not match the number of bases found in the sample (" +
            std::to_string( sample.read_bases.size() ) + ")"
        );
    }

    return result;
}

std::ostream& operator<<( std::ostream& os, BaseCounts const& bs )
{
    os << "A=" << bs.a_count << ", C=" << bs.c_count << ", G=" << bs.g_count;
    os << ", T=" << bs.t_count << ", N=" << bs.n_count << ", D=" << bs.d_count;
    return os;
}

std::ostream& to_sync( BaseCounts const& bs, std::ostream& os )
{
    os << bs.a_count << ":" << bs.t_count << ":" << bs.c_count << ":" << bs.g_count;
    os << ":" << bs.n_count << ":" << bs.d_count;
    return os;
}

} // namespace population
} // namespace genesis
