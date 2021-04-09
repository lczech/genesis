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

#include "genesis/population/formats/vcf_format_iterator.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     VCF/BCF Format Helper Constants
// =================================================================================================

VcfFormatHelper::VcfFormatHelper()
{
    // Vector End and Missing constants
    static_assert(
        VcfFormatHelper::bcf_int8_vector_end_  == bcf_int8_vector_end,
        "Definitions of bcf_int8_vector_end in htslib and of VcfFormatHelper::bcf_int8_vector_end_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int16_vector_end_ == bcf_int16_vector_end,
        "Definitions of bcf_int16_vector_end in htslib and of VcfFormatHelper::bcf_int16_vector_end_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int32_vector_end_ == bcf_int32_vector_end,
        "Definitions of bcf_int32_vector_end in htslib and of VcfFormatHelper::bcf_int32_vector_end_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int64_vector_end_ == bcf_int64_vector_end,
        "Definitions of bcf_int64_vector_end in htslib and of VcfFormatHelper::bcf_int64_vector_end_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_str_vector_end_   == bcf_str_vector_end,
        "Definitions of bcf_str_vector_end in htslib and of VcfFormatHelper::bcf_str_vector_end_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int8_missing_     == bcf_int8_missing,
        "Definitions of bcf_int8_missing in htslib and of VcfFormatHelper::bcf_int8_missing_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int16_missing_    == bcf_int16_missing,
        "Definitions of bcf_int16_missing in htslib and of VcfFormatHelper::bcf_int16_missing_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int32_missing_    == bcf_int32_missing,
        "Definitions of bcf_int32_missing in htslib and of VcfFormatHelper::bcf_int32_missing_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_int64_missing_    == bcf_int64_missing,
        "Definitions of bcf_int64_missing in htslib and of VcfFormatHelper::bcf_int64_missing_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
    static_assert(
        VcfFormatHelper::bcf_str_missing_      == bcf_str_missing,
        "Definitions of bcf_str_missing in htslib and of VcfFormatHelper::bcf_str_missing_ "
        "in genesis differ. Please submit a bug report at https://github.com/lczech/genesis/issues"
    );
}

// =================================================================================================
//     VCF/BCF Format Helper Static Helper Functions
// =================================================================================================

int32_t VcfFormatHelper::bcf_hdr_nsamples_( ::bcf_hdr_t const* header )
{
    return bcf_hdr_nsamples( header );
}

std::string VcfFormatHelper::hdr_sample_name_( ::bcf_hdr_t const* header, size_t index )
{
    assert( bcf_hdr_nsamples(header) == header->n[BCF_DT_SAMPLE] );
    auto const smp_cnt = header->n[BCF_DT_SAMPLE];
    if( static_cast<long>( index ) >= smp_cnt ) {
        throw std::invalid_argument(
            "Cannot get sample name for sample at index " + std::to_string(index) +
            ", as the VCF/BCF file only uses " + std::to_string( smp_cnt ) + " samples."
        );
    }
    return header->samples[index];
}

int VcfFormatHelper::bcf_get_format_string_(
    const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, char ***dst, int *ndst
) {
    return bcf_get_format_string( hdr, line, tag, dst, ndst );
}

int VcfFormatHelper::bcf_get_format_values_(
    const bcf_hdr_t *hdr, bcf1_t *line, const char *tag, void **dst, int *ndst, int type
) {
    return bcf_get_format_values( hdr, line, tag, dst, ndst, type);
}

int VcfFormatHelper::bcf_get_genotypes_( const bcf_hdr_t *hdr, bcf1_t *line, void **dst, int *ndst )
{
    return bcf_get_genotypes( hdr, line, dst, ndst );
}

int VcfFormatHelper::bcf_float_is_vector_end_(float f)
{
    return bcf_float_is_vector_end(f);
}

int VcfFormatHelper::bcf_float_is_missing_(float f)
{
    return bcf_float_is_missing(f);
}

} // namespace population
} // namespace genesis

#endif // htslib guard
