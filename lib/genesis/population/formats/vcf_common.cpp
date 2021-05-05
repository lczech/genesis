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
