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

#include "genesis/population/functions/pool_sample.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Status and Information
// =================================================================================================

PoolSampleStatus status(
    PoolSample const& sample,
    size_t min_coverage,
    size_t max_coverage,
    size_t min_count,
    bool tolerate_deletions
) {
    PoolSampleStatus result;
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
//     Accumulation, Filtering, etc
// =================================================================================================

PoolSample merge( PoolSample const& p1, PoolSample const& p2 )
{
    // Make sure that we do not forget any fields in case of later refactoring of the struct.
    static_assert(
        sizeof( PoolSample ) == 6 * sizeof( size_t ),
        "Unexpected number of member variables in PoolSample class"
    );

    PoolSample result = p1;
    result.a_count += p2.a_count;
    result.c_count += p2.c_count;
    result.g_count += p2.g_count;
    result.t_count += p2.t_count;
    result.n_count += p2.n_count;
    result.d_count += p2.d_count;
    return result;
}

PoolSample merge( std::vector<PoolSample> const& p )
{
    PoolSample result;
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

void filter_min_count( PoolSample& sample, size_t min_count )
{
    // Reset counts if needed, according to min count setting.
    if( sample.a_count < min_count ) {
        sample.a_count = 0;
    }
    if( sample.c_count < min_count ) {
        sample.c_count = 0;
    }
    if( sample.g_count < min_count ) {
        sample.g_count = 0;
    }
    if( sample.t_count < min_count ) {
        sample.t_count = 0;
    }
}

std::pair<char, double> consensus( PoolSample const& sample )
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

std::pair<char, double> consensus( PoolSample const& sample, PoolSampleStatus const& status )
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

PoolSample convert_to_pool_sample( SimplePileupReader::Sample const& sample )
{
    PoolSample result;
    result.a_count = sample.a_count;
    result.c_count = sample.c_count;
    result.g_count = sample.g_count;
    result.t_count = sample.t_count;
    result.n_count = sample.n_count;
    result.d_count = sample.d_count;
    return result;
}

PoolSampleSet convert_to_pool_samples( SimplePileupReader::Record const& record )
{
    PoolSampleSet result;
    result.chromosome     = record.chromosome;
    result.position       = record.position;
    result.reference_base = record.reference_base;

    result.samples.reserve( record.samples.size() );
    for( size_t i = 0; i < record.samples.size(); ++i ) {
        result.samples.push_back( convert_to_pool_sample( record.samples[i] ));
    }

    return result;
}

#ifdef GENESIS_HTSLIB

PoolSampleSet convert_to_pool_samples( VcfRecord const& record )
{
    // Error check.
    if( ! record.has_format( "AD" )) {
        throw std::runtime_error(
            "Cannot convert VcfRecord to PoolSampleSet, as the VcfRecord does not have "
            "the required FORMAT field 'AD'"
        );
    }

    // Get all variants (REF and ALT), and check them. We manually add deletion here if ALT == ".",
    // as this is not part of the variants provided from htslib.
    auto vars = record.get_variants();
    if( vars.size() == 1 ) {
        assert( record.get_alternatives().empty() );
        vars.push_back( "." );
    }
    for( auto const& var : vars ) {
        if( var.size() != 1 ) {
            throw std::runtime_error(
                "Cannot convert VcfRecord to PoolSampleSet, as one of the VcfRecord REF or ALT "
                "sequences/alleles is not a single nucleotide"
            );
        }
    }
    assert( vars.size() > 1 );
    assert( vars[0].size() == 1 );

    // Prepare common fields of the result.
    // For the reference base, we use the first nucleotide of the first variant (REF);
    // above, we have ensured that this exists and is in fact a single nucleotide only.
    PoolSampleSet result;
    result.chromosome     = record.get_chromosome();
    result.position       = record.get_position();
    result.reference_base = vars[0][0];

    // Process the samples that are present in the VCF record line.
    result.samples.reserve( record.header().get_sample_count() );
    for( auto const& sample_ad : record.get_format_int("AD") ) {
        if( sample_ad.valid_value_count() != vars.size() ) {
            throw std::runtime_error(
                "Invalid VCF Record that contains " + std::to_string( vars.size() ) +
                " REF and ALT sequences/alleles, but its FORMAT field 'AD' only contains " +
                std::to_string( sample_ad.valid_value_count() ) + " entries"
            );
        }

        // Go through all REF and ALT entries and their respective FORMAT 'AD' counts,
        // and sum them up.
        PoolSample sample;
        for( size_t i = 0; i < vars.size(); ++i ) {

            // Get the nucleitide and its count.
            assert( vars[i].size() == 1 );
            char const nt = vars[i][0];
            auto const cnt = sample_ad.get_value_at(i);
            if( cnt < 0 ) {
                throw std::runtime_error(
                    "Invalid VCF Record with FORMAT field 'AD' value < 0 for a sample"
                );
            }

            // Add it to the respective count variable of the sample.
            switch( nt ) {
                case 'a':
                case 'A': {
                    sample.a_count = cnt;
                    break;
                }
                case 'c':
                case 'C': {
                    sample.c_count = cnt;
                    break;
                }
                case 'g':
                case 'G': {
                    sample.g_count = cnt;
                    break;
                }
                case 't':
                case 'T': {
                    sample.t_count = cnt;
                    break;
                }
                case 'n':
                case 'N': {
                    sample.n_count = cnt;
                    break;
                }
                case '.': {
                    sample.d_count = cnt;
                    break;
                }
                default: {
                    throw std::runtime_error(
                        "Invalid VCF Record that contains a REF or ALT sequence/allele with "
                        "invalid nucleitide `" + std::string( 1, nt ) + "` where only `[ACGTN]` "
                        "are allowed"
                    );
                }
            }
        }

        // Done with the sample. Add it to the result.
        result.samples.push_back( sample );
    }

    // Last proof check.
    if( result.samples.size() != record.header().get_sample_count() ) {
        throw std::runtime_error(
            "Invalid VCF Record with number of samples in the record (" +
            std::to_string( result.samples.size() ) + ") not equal to the number of samples given "
            "in the VCF header (" + std::to_string( record.header().get_sample_count() ) + ")"
        );
    }

    return result;
}

#endif // htslib guard

} // namespace population
} // namespace genesis
