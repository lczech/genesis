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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_common.hpp"

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/vcf_input_stream.hpp"
#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/functions/functions.hpp"
#include "genesis/population/variant.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Typedefs and Enums
// =================================================================================================

// VcfHeaderLine
static_assert(
    static_cast<int>( VcfHeaderLine::kFilter ) == BCF_HL_FLT,
    "Definitions of BCF_HL_FLT in htslib and of VcfHeaderLine::kFilter in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfHeaderLine::kInfo ) == BCF_HL_INFO,
    "Definitions of BCF_HL_INFO in htslib and of VcfHeaderLine::kInfo in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfHeaderLine::kFormat ) == BCF_HL_FMT,
    "Definitions of BCF_HL_FMT in htslib and of VcfHeaderLine::kFormat in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfHeaderLine::kContig ) == BCF_HL_CTG,
    "Definitions of BCF_HL_CTG in htslib and of VcfHeaderLine::kContig in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfHeaderLine::kStructured ) == BCF_HL_STR,
    "Definitions of BCF_HL_STR in htslib and of VcfHeaderLine::kStructured in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfHeaderLine::kGeneric ) == BCF_HL_GEN,
    "Definitions of BCF_HL_GEN in htslib and of VcfHeaderLine::kGeneric in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);

// VcfValueType
static_assert(
    static_cast<int>( VcfValueType::kFlag ) == BCF_HT_FLAG,
    "Definitions of BCF_HT_FLAG in htslib and of VcfValueType::kFlag in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueType::kInteger ) == BCF_HT_INT,
    "Definitions of BCF_HT_INT in htslib and of VcfValueType::kInteger in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueType::kFloat ) == BCF_HT_REAL,
    "Definitions of BCF_HT_REAL in htslib and of VcfValueType::kFloat in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueType::kString ) == BCF_HT_STR,
    "Definitions of BCF_HT_STR in htslib and of VcfValueType::kString in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);

// VcfValueSpecial
static_assert(
    static_cast<int>( VcfValueSpecial::kFixed ) == BCF_VL_FIXED,
    "Definitions of BCF_VL_FIXED in htslib and of VcfValueSpecial::kFixed in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kVariable ) == BCF_VL_VAR,
    "Definitions of BCF_VL_VAR in htslib and of VcfValueSpecial::kVariable in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kAllele ) == BCF_VL_A,
    "Definitions of BCF_VL_A in htslib and of VcfValueSpecial::kAllele in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kGenotype ) == BCF_VL_G,
    "Definitions of BCF_VL_G in htslib and of VcfValueSpecial::kGenotype in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kReference ) == BCF_VL_R,
    "Definitions of BCF_VL_R in htslib and of VcfValueSpecial::kReference in genesis differ. "
    "Please submit a bug report at https://github.com/lczech/genesis/issues"
);

// =================================================================================================
//     Typedef and Enum Helpers
// =================================================================================================

std::string vcf_value_type_to_string( VcfValueType ht_type )
{
    return vcf_value_type_to_string( static_cast<int>( ht_type ));
}

std::string vcf_value_type_to_string( int ht_type )
{
    switch( ht_type ) {
        case BCF_HT_INT: {
            return "Integer";
        }
        case BCF_HT_REAL: {
            return "Float";
        }
        case BCF_HT_STR: {
            return "String";
        }
        case BCF_HT_FLAG: {
            return "Flag";
        }
        default: {
            throw std::domain_error( "Invalid value type provided: " + std::to_string( ht_type ));
        }
    }

    // Cannot happen, but let's satisfy eager compilers.
    assert( false );
    return "Unknown";
}

std::string vcf_value_special_to_string( VcfValueSpecial vl_type_num )
{
    return vcf_value_special_to_string( static_cast<int>( vl_type_num ));
}

std::string vcf_value_special_to_string( int vl_type_num )
{
    switch( vl_type_num ) {
        case BCF_VL_FIXED: {
            return "fixed (n)";
        }
        case BCF_VL_VAR: {
            return "variable (.)";
        }
        case BCF_VL_A: {
            return "allele (A)";
        }
        case BCF_VL_G: {
            return "genotype (G)";
        }
        case BCF_VL_R: {
            return "reference (R)";
        }
        default: {
            throw std::domain_error( "Invalid value number provided: " + std::to_string( vl_type_num ));
        }
    }

    // Cannot happen, but let's satisfy eager compilers.
    assert( false );
    return "Unknown";
}

std::string vcf_hl_type_to_string( int hl_type )
{
    switch( hl_type ) {
        case BCF_HL_FLT:  return "FILTER";
        case BCF_HL_INFO: return "INFO";
        case BCF_HL_FMT:  return "FORMAT";
        case BCF_HL_CTG:  return "CONTIG";
        case BCF_HL_STR:  return "Structured header line";
        case BCF_HL_GEN:  return "Generic header line";
    }
    throw std::invalid_argument( "Invalid header line type: " + std::to_string( hl_type ));
}

// =================================================================================================
//     Conversion Functions
// =================================================================================================

/**
 * @brief Local helper function that returns the REF and ALT chars of a VcfRecord for SNPs.
 *
 * This function expects the @p record to only contain SNP REF and ALT (single nucleotides),
 * and throws when not. It then fills the resulting array with these chars. That is, result[0]
 * is the REF char, result[1] the first ALT char, and so forth.
 *
 * To keep it speedy, we always return an array that is large enough for all `ACGTND`,
 * and return the number of used entries as the second value of the pair.
 */
std::pair<std::array<char, 6>, size_t> get_vcf_record_snp_ref_alt_chars_( VcfRecord const& record )
{
    // Get all variants (REF and ALT), and check them. We manually add deletion here if ALT == ".",
    // as this is not part of the variants provided from htslib.
    // There are only 6 possible single nucleotide variants (ACGTN.), so we save the effort of
    // creating an intermediate vector, and use a fixed size array and a counter instead.
    // Also, we use htslib functions directly on the vcf record to not have to go through
    // string allocations for each alternative.
    record.unpack();
    auto rec_data = record.data();

    // The n_allele count does not include deletions ('.'), meaning that if there is only a single
    // variant, we manually adjust this to also include the deletion.
    // To avoid too much branching, we init the array so that we have all deletions initially,
    // and hence do not need to overwrite in the case that we added that deletion manually
    // to the counter.
    size_t const var_cnt = rec_data->n_allele == 1 ? rec_data->n_allele + 1 : rec_data->n_allele;
    auto vars = std::array<char, 6>{{ '.', '.', '.', '.', '.', '.' }};
    if( var_cnt > 6 ) {
        throw std::runtime_error(
            "Invalid VCF Record that contains a REF or ALT sequence/allele with "
            "invalid nucleitides where only `[ACGTN.]` are allowed, at " +
            record.get_chromosome() + ":" + std::to_string( record.get_position() )
        );
    }

    // Now store all single nucleotide alleles that are in the record
    // (we only count up to the actual number that is there, so that the missing deletion [in case
    // that this record has a deletion] is not touched).
    for( size_t i = 0; i < rec_data->n_allele; ++i ) {
        if( std::strlen( rec_data->d.allele[i] ) != 1 ) {
            throw std::runtime_error(
                "Cannot convert VcfRecord to Variant, as one of the VcfRecord REF or ALT "
                "sequences/alleles is not a single nucleotide (it is not a SNP), at " +
                record.get_chromosome() + ":" + std::to_string( record.get_position() ) +
                ". At the time being, we are not supporting indels and other such variants."
            );
        }
        vars[i] = *rec_data->d.allele[i];
    }

    return { vars, var_cnt };
}

Variant convert_to_variant_as_pool( VcfRecord const& record )
{
    // Error check.
    if( ! record.has_format( "AD" )) {
        throw std::runtime_error(
            "Cannot convert VcfRecord to Variant, as the VcfRecord does not have "
            "the required FORMAT field 'AD' for alleleic depth."
        );
    }

    // Get the ref and alt chars of the SNP.
    auto const snp_chars = get_vcf_record_snp_ref_alt_chars_( record );

    // Prepare common fields of the result.
    // For the reference base, we use the first nucleotide of the first variant (REF);
    // above, we have ensured that this exists and is in fact a single nucleotide only.
    // Same for the alternative base, where we use the first ALT in the record.
    Variant result;
    result.chromosome       = record.get_chromosome();
    result.position         = record.get_position();
    result.reference_base   = snp_chars.first[0];
    result.alternative_base = snp_chars.first[1]; // TODO this is only reasonable for biallelic SNPs

    // Process the samples that are present in the VCF record line.
    result.samples.reserve( record.header().get_sample_count() );
    for( auto const& sample_ad : record.get_format_int("AD") ) {
        if( sample_ad.valid_value_count() > 0 && sample_ad.valid_value_count() != snp_chars.second ) {
            throw std::runtime_error(
                "Invalid VCF Record that contains " + std::to_string( snp_chars.second ) +
                " REF and ALT sequences/alleles, but its FORMAT field 'AD' only contains " +
                std::to_string( sample_ad.valid_value_count() ) + " entries, at " +
                record.get_chromosome() + ":" + std::to_string( record.get_position() )
            );
        }

        // Go through all REF and ALT entries and their respective FORMAT 'AD' counts,
        // and sum them up, storing them in a new BaseCounts instance at the end of the vector.
        result.samples.emplace_back();
        auto& sample = result.samples.back();
        for( size_t i = 0; i < sample_ad.valid_value_count(); ++i ) {

            // Get the nucleotide and its count.
            auto const cnt = sample_ad.get_value_at(i);
            if( cnt < 0 ) {
                throw std::runtime_error(
                    "Invalid VCF Record with FORMAT field 'AD' value < 0 for a sample, at " +
                    record.get_chromosome() + ":" + std::to_string( record.get_position() )
                );
            }

            // Add it to the respective count variable of the sample.
            switch( snp_chars.first[i] ) {
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
                        "invalid nucleitide `" + std::string( 1, snp_chars.first[i] ) +
                        "` where only `[ACGTN.]` are allowed, at " +
                        record.get_chromosome() + ":" + std::to_string( record.get_position() )
                    );
                }
            }
        }
    }

    // Last proof check.
    if( result.samples.size() != record.header().get_sample_count() ) {
        throw std::runtime_error(
            "Invalid VCF Record with number of samples in the record (" +
            std::to_string( result.samples.size() ) + ") not equal to the number of samples given "
            "in the VCF header (" + std::to_string( record.header().get_sample_count() ) + "), at " +
            record.get_chromosome() + ":" + std::to_string( record.get_position() )
        );
    }

    return result;
}

Variant convert_to_variant_as_individuals(
    VcfRecord const& record,
    bool use_allelic_depth
) {
    Variant result;

    // Short solution for when we want to use the AD field:
    // Simply re-use the pool approach, and merge into one BaseCounts.
    if( use_allelic_depth ) {
        result = convert_to_variant_as_pool( record );
        result.samples = std::vector<BaseCounts>{ merge( result.samples ) };
        return result;
    }

    // Here we treat each individual just by counting genotypes.
    record.unpack();

    // Error check.
    if( ! record.has_format( "GT" )) {
        throw std::runtime_error(
            "Cannot convert VcfRecord to Variant, as the VcfRecord does not have "
            "the required FORMAT field 'GT' for genotypes."
        );
    }

    // Get the ref and alt chars of the SNP.
    auto const snp_chars = get_vcf_record_snp_ref_alt_chars_( record );

    // Prepare common fields of the result. Same as convert_to_variant_as_pool(), see there.
    result.chromosome       = record.get_chromosome();
    result.position         = record.get_position();
    result.reference_base   = snp_chars.first[0];
    result.alternative_base = snp_chars.first[1]; // TODO this is only reasonable for biallelic SNPs

    // We merge everything into one sample, representing the individuals as a pool.
    result.samples.resize( 1 );
    auto& sample = result.samples.back();

    // Go through all sample columns of the VCF, examining their GT field.
    for( auto const& sample_gt : record.get_format_genotype() ) {

        // Go through all REF and ALT entries and their respective GT values for the current sample.
        for( size_t i = 0; i < sample_gt.valid_value_count(); ++i ) {

            // Get the genoptype and immediately convert to the index
            // that we can look up in the snp array.
            auto const gt = sample_gt.get_value_at(i).variant_index();

            // If the VCF is not totally messed up, this needs to be within the number of
            // REF and ALT nucleotides (or -1 for deletions); check that.
            if( !( gt < 0 || static_cast<size_t>(gt) < snp_chars.second )) {
                throw std::runtime_error(
                    "Invalid VCF Record that contains an index " + std::to_string( gt ) +
                    " into the genotype list that does not exist, at " +
                    record.get_chromosome() + ":" + std::to_string( record.get_position() )
                );
            }

            // Special case deletion. The genotype value stored in VCF is -1 in that case.
            if( gt < 0 ) {
                ++sample.d_count;
                continue;
            }

            // Use the index to get what nucleotide the genotype is, and increment the count.
            switch( snp_chars.first[gt] ) {
                case 'a':
                case 'A': {
                    ++sample.a_count;
                    break;
                }
                case 'c':
                case 'C': {
                    ++sample.c_count;
                    break;
                }
                case 'g':
                case 'G': {
                    ++sample.g_count;
                    break;
                }
                case 't':
                case 'T': {
                    ++sample.t_count;
                    break;
                }
                case 'n':
                case 'N': {
                    ++sample.n_count;
                    break;
                }
                default: {
                    throw std::runtime_error(
                        "Invalid VCF Record that contains a REF or ALT sequence/allele with "
                        "invalid nucleitide `" + std::string( 1, snp_chars.first[i] ) +
                        "` where only `[ACGTN.]` are allowed, at " +
                        record.get_chromosome() + ":" + std::to_string( record.get_position() )
                    );
                }
            }
        }
    }
    return result;
}

GenomeLocusSet genome_locus_set_from_vcf_file( std::string const& file )
{
    // Simpler version than genome_region_list_from_vcf_file(), as we do not need to keep track
    // of regions here... simply add all the positions individually to the set.
    GenomeLocusSet result;

    // Open and read file, without expecting it to be sorted.
    auto it = VcfInputStream( file, false );
    while( it ) {
        result.add( it.record().get_chromosome(), it.record().get_position() );
        ++it;
    }
    return result;
}

GenomeRegionList genome_region_list_from_vcf_file( std::string const& file )
{
    GenomeRegionList result;
    genome_region_list_from_vcf_file( file, result );
    return result;
}

void genome_region_list_from_vcf_file( std::string const& file, GenomeRegionList& target )
{
    // Prepare bookkeeping. We need the chromosome, the position where we started the current
    // interval, and the position where we are at in the current interval.
    std::string cur_chr;
    size_t beg_pos = 0;
    size_t cur_pos = 0;

    auto insert_ = [&]( std::string const& chr, size_t beg, size_t end ){
        if( chr.empty() ) {
            return;
        }
        assert( beg > 0 && end > 0 );
        assert( beg <= end );

        // We add the interval, using the merge flag,
        // to make sure that even unsorted VCFs produce consecutive, fully merged regions.
        target.add( chr, beg, end, true );
    };

    // Open and read file, without expecting it to be sorted.
    auto it = VcfInputStream( file, false );
    while( it ) {
        if( it.record().get_chromosome() == cur_chr ) {
            // We are still within the same chromosome.

            // If we did not move (can happen if multiple variants are reported at the same position),
            // or moved exactly one position, we are still in the same interval.
            // We could simply use the insert_overlap functionality of the IntervalTree here
            // (via GenomeRegionList.add()), and that would already take care of the merging.
            // But this here is likely faster, as we do not need to always remove and add
            // intervals to the tree for consecutive runs of positions, which is most likely
            // the most common use case (using a sorted VCF file).
            auto const pos = it.record().get_position();
            if( pos == cur_pos || pos == cur_pos + 1 ) {
                cur_pos = pos;
            } else {
                // Otherwise, we are at a new interval, so we need to finish the current one.
                assert( ! cur_chr.empty() );
                insert_( cur_chr, beg_pos, cur_pos );

                // Now set the start of the next interval.
                beg_pos = pos;
                cur_pos = pos;
            }
        } else {
            // We are at a new chromsome.

            // Unless we just started, we add the interval, again using the merge flag.
            insert_( cur_chr, beg_pos, cur_pos );

            // Now set the start of the new interval.
            cur_chr = it.record().get_chromosome();
            beg_pos = it.record().get_position();
            cur_pos = beg_pos;
        }
        ++it;
    }

    // Finally, add the last interval that remains after the file is done.
    insert_( cur_chr, beg_pos, cur_pos );
}

// =================================================================================================
//     VCF Genotype Functions
// =================================================================================================

std::string vcf_genotype_string( std::vector<VcfGenotype> const& genotypes )
{
    std::string result;
    for( size_t i = 0; i < genotypes.size(); ++i ) {
        auto const& g = genotypes[i];

        if( i > 0 ) {
            result += ( g.is_phased() ? "|" : "/" );
        }
        result += ( g.is_missing() ? "." : std::to_string( g.variant_index() ));
    }
    return result;
}

size_t vcf_genotype_sum( std::vector<VcfGenotype> const& genotypes )
{
    size_t result = 0;
    for( auto const& gt : genotypes ) {
        result += static_cast<size_t>( gt.is_alternative() );
    }
    assert( result <= genotypes.size() );
    return result;
}

// =================================================================================================
//     VCF Genotype
// =================================================================================================

int32_t VcfGenotype::variant_index() const
{
    return bcf_gt_allele( genotype_ );
}

bool VcfGenotype::is_reference() const
{
    return bcf_gt_allele( genotype_ ) == 0;
}

bool VcfGenotype::is_alternative() const
{
    return bcf_gt_allele( genotype_ ) > 0;
}

bool VcfGenotype::is_missing() const
{
    return bcf_gt_is_missing( genotype_ );
}

bool VcfGenotype::is_phased() const
{
    return bcf_gt_is_phased( genotype_ );
}

int32_t VcfGenotype::data() const
{
    return genotype_;
}

} // namespace population
} // namespace genesis

#endif // htslib guard
