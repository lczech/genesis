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

#include "genesis/population/filter/variant_filter.hpp"

#include "genesis/population/functions/functions.hpp"
#include "genesis/utils/text/char.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Filter
// =================================================================================================

std::ostream& print_variant_filter_stats(
    std::ostream& os,
    VariantFilterStats const& stats,
    bool verbose
) {
    assert( stats.data.size() == static_cast<size_t>( VariantFilterTag::kEnd ) );

    // We use an explicit loop over the enum values here, which makes sure that we cannot
    // forget about any values in the future. This is a bit inefficient, but we do not expect
    // to call this function more than once.
    for( size_t i = 0; i < static_cast<size_t>( VariantFilterTag::kEnd ); ++i ) {
        switch( static_cast<VariantFilterTag>(i) ) {
            case VariantFilterTag::kPassed: {
                auto const val = stats[VariantFilterTag::kPassed];
                if( val > 0 || verbose ) {
                    os << "Passed: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kMissing: {
                auto const val = stats[VariantFilterTag::kMissing];
                if( val > 0 || verbose ) {
                    os << "Missing: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kNotPassed: {
                auto const val = stats[VariantFilterTag::kNotPassed];
                if( val > 0 || verbose ) {
                    os << "NotPassed: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kInvalid: {
                auto const val = stats[VariantFilterTag::kInvalid];
                if( val > 0 || verbose ) {
                    os << "Invalid: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kMaskedPosition: {
                auto const val = stats[VariantFilterTag::kMaskedPosition];
                if( val > 0 || verbose ) {
                    os << "MaskedPosition: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kMaskedRegion: {
                auto const val = stats[VariantFilterTag::kMaskedRegion];
                if( val > 0 || verbose ) {
                    os << "MaskedRegion: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kNoSamplePassed: {
                auto const val = stats[VariantFilterTag::kNoSamplePassed];
                if( val > 0 || verbose ) {
                    os << "NoSamplePassed: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kNotAllSamplesPassed: {
                auto const val = stats[VariantFilterTag::kNotAllSamplesPassed];
                if( val > 0 || verbose ) {
                    os << "NotAllSamplesPassed: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kEmpty: {
                auto const val = stats[VariantFilterTag::kEmpty];
                if( val > 0 || verbose ) {
                    os << "Empty: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kBelowMinCoverage: {
                auto const val = stats[VariantFilterTag::kBelowMinCoverage];
                if( val > 0 || verbose ) {
                    os << "BelowMinCoverage: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kAboveMaxCoverage: {
                auto const val = stats[VariantFilterTag::kAboveMaxCoverage];
                if( val > 0 || verbose ) {
                    os << "AboveMaxCoverage: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kAboveDeletionsCountLimit: {
                auto const val = stats[VariantFilterTag::kAboveDeletionsCountLimit];
                if( val > 0 || verbose ) {
                    os << "AboveDeletionsCountLimit: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kNotSnp: {
                auto const val = stats[VariantFilterTag::kNotSnp];
                if( val > 0 || verbose ) {
                    os << "NotSnp: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kNotBiallelicSnp: {
                auto const val = stats[VariantFilterTag::kNotBiallelicSnp];
                if( val > 0 || verbose ) {
                    os << "NotBiallelicSnp: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kBelowSnpMinCount: {
                auto const val = stats[VariantFilterTag::kBelowSnpMinCount];
                if( val > 0 || verbose ) {
                    os << "BelowSnpMinCount: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kAboveSnpMaxCount: {
                auto const val = stats[VariantFilterTag::kAboveSnpMaxCount];
                if( val > 0 || verbose ) {
                    os << "AboveSnpMaxCount: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kBelowMinAlleleFreq: {
                auto const val = stats[VariantFilterTag::kBelowMinAlleleFreq];
                if( val > 0 || verbose ) {
                    os << "BelowMinAlleleFreq: " << val << "\n";
                }
                break;
            }
            case VariantFilterTag::kEnd: {
                throw std::domain_error(
                    "Invalid VariantFilterTag with value " + std::to_string(i)
                );
            }
            // Not using default here, as that way, we catch any unhandled enum values.
        }
    }
    return os;
}

std::string print_variant_filter_stats(
    VariantFilterStats const& stats,
    bool verbose
) {
    std::stringstream ss;
    print_variant_filter_stats( ss, stats, verbose );
    return ss.str();
}

} // namespace population
} // namespace genesis
