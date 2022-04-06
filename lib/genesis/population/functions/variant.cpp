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

#include "genesis/population/functions/variant.hpp"

#include "genesis/population/functions/base_counts.hpp"
#include "genesis/utils/io/char.hpp"

#ifdef GENESIS_HTSLIB

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#endif // htslib guard

#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions
// =================================================================================================

BaseCounts total_base_counts( Variant const& variant )
{
    return merge( variant.samples );
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

} // namespace population
} // namespace genesis
