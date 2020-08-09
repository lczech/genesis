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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_common.hpp"

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

static_assert(
    static_cast<int>( VcfValueType::kFlag ) == BCF_HT_FLAG,
    "genesis and htslib differ in their definition of VCF header type 'Flag'"
);
static_assert(
    static_cast<int>( VcfValueType::kInteger ) == BCF_HT_INT,
    "genesis and htslib differ in their definition of VCF header type 'Integer'"
);
static_assert(
    static_cast<int>( VcfValueType::kFloat ) == BCF_HT_REAL,
    "genesis and htslib differ in their definition of VCF header type 'Float'"
);
static_assert(
    static_cast<int>( VcfValueType::kString ) == BCF_HT_STR,
    "genesis and htslib differ in their definition of VCF header type 'String'"
);

static_assert(
    static_cast<int>( VcfValueSpecial::kFixed ) == BCF_VL_FIXED,
    "genesis and htslib differ in their definition of VCF number 'fixed' (n)"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kVariable ) == BCF_VL_VAR,
    "genesis and htslib differ in their definition of VCF number 'variable' (.)"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kAllele ) == BCF_VL_A,
    "genesis and htslib differ in their definition of VCF number 'allele' (A)"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kGenotype ) == BCF_VL_G,
    "genesis and htslib differ in their definition of VCF number 'genotype' (G)"
);
static_assert(
    static_cast<int>( VcfValueSpecial::kReference ) == BCF_VL_R,
    "genesis and htslib differ in their definition of VCF number 'reference' (R)"
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
