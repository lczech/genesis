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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/filter/variant_filter.hpp"

#include "genesis/population/function/functions.hpp"
#include "genesis/utils/text/char.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

namespace genesis {
namespace population {

// We want to make sure that the tags enum is exactly as expected here. In case that we later
// add other values to that enum, we want to know here, in order to adapt all below functions
// accordingly.
static_assert(
    static_cast<FilterStatus::IntType>( VariantFilterTag::kEnd ) == 17,
    "VariantFilterTag::kEnd != 17. The enum has values that are not accounted for."
);
static_assert(
    static_cast<FilterStatus::IntType>( VariantFilterTagCategory::kEnd ) == 6,
    "VariantFilterTagCategory::kEnd != 6. The enum has values that are not accounted for."
);

// =================================================================================================
//     Stats
// =================================================================================================

VariantFilterTagCategory variant_filter_tag_to_category( VariantFilterTag tag )
{
    // Just return the category of the tag.
    switch( tag ) {
        case VariantFilterTag::kPassed:
            return VariantFilterTagCategory::kPassed;
        case VariantFilterTag::kMaskedPosition:
        case VariantFilterTag::kMaskedRegion:
            return VariantFilterTagCategory::kMasked;
        case VariantFilterTag::kMissing:
        case VariantFilterTag::kNotPassed:
        case VariantFilterTag::kInvalid:
            return VariantFilterTagCategory::kMissingInvalid;
        case VariantFilterTag::kNoSamplePassed:
        case VariantFilterTag::kNotAllSamplesPassed:
            return VariantFilterTagCategory::kSamplesFailed;
        case VariantFilterTag::kEmpty:
        case VariantFilterTag::kBelowMinReadDepth:
        case VariantFilterTag::kAboveMaxReadDepth:
        case VariantFilterTag::kAboveDeletionsCountLimit:
            return VariantFilterTagCategory::kNumeric;
        case VariantFilterTag::kNotSnp:
        case VariantFilterTag::kNotBiallelicSnp:
        case VariantFilterTag::kBelowSnpMinCount:
        case VariantFilterTag::kAboveSnpMaxCount:
        case VariantFilterTag::kBelowMinAlleleFreq:
            return VariantFilterTagCategory::kInvariant;
        default: {
            throw std::invalid_argument(
                "Invalid VariantFilterTag: " +
                std::to_string( static_cast<FilterStatus::IntType>( tag ))
            );
        }
    }
    assert( false );

    // Make compilers happy, just in case.
    return VariantFilterTagCategory::kEnd;
}

VariantFilterCategoryStats variant_filter_stats_category_counts(
    VariantFilterStats const& stats
) {
    // assert( stats[ VariantFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( VariantFilterTag::kEnd ) );

    // Build our result, by simply adding up the values to our simple categories / classes.
    VariantFilterCategoryStats result;
    result[VariantFilterTagCategory::kPassed]         += stats[ VariantFilterTag::kPassed ];
    result[VariantFilterTagCategory::kMasked]         += stats[ VariantFilterTag::kMaskedPosition ];
    result[VariantFilterTagCategory::kMasked]         += stats[ VariantFilterTag::kMaskedRegion ];
    result[VariantFilterTagCategory::kMissingInvalid] += stats[ VariantFilterTag::kMissing ];
    result[VariantFilterTagCategory::kMissingInvalid] += stats[ VariantFilterTag::kNotPassed ];
    result[VariantFilterTagCategory::kMissingInvalid] += stats[ VariantFilterTag::kInvalid ];
    result[VariantFilterTagCategory::kSamplesFailed]  += stats[ VariantFilterTag::kNoSamplePassed ];
    result[VariantFilterTagCategory::kSamplesFailed]  += stats[ VariantFilterTag::kNotAllSamplesPassed ];
    result[VariantFilterTagCategory::kNumeric]        += stats[ VariantFilterTag::kEmpty ];
    result[VariantFilterTagCategory::kNumeric]        += stats[ VariantFilterTag::kBelowMinReadDepth ];
    result[VariantFilterTagCategory::kNumeric]        += stats[ VariantFilterTag::kAboveMaxReadDepth ];
    result[VariantFilterTagCategory::kNumeric]        += stats[ VariantFilterTag::kAboveDeletionsCountLimit ];
    result[VariantFilterTagCategory::kInvariant]      += stats[ VariantFilterTag::kNotSnp ];
    result[VariantFilterTagCategory::kInvariant]      += stats[ VariantFilterTag::kNotBiallelicSnp ];
    result[VariantFilterTagCategory::kInvariant]      += stats[ VariantFilterTag::kBelowSnpMinCount ];
    result[VariantFilterTagCategory::kInvariant]      += stats[ VariantFilterTag::kAboveSnpMaxCount ];
    result[VariantFilterTagCategory::kInvariant]      += stats[ VariantFilterTag::kBelowMinAlleleFreq ];
    return result;
}

size_t variant_filter_stats_category_counts(
    VariantFilterStats const& stats, VariantFilterTagCategory category
) {
    // assert( stats[ VariantFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( VariantFilterTag::kEnd ) );

    // Select the requested category and add up their values.
    size_t result = 0;
    switch( category ) {
        case VariantFilterTagCategory::kPassed: {
            result += stats[ VariantFilterTag::kPassed ];
            break;
        }
        case VariantFilterTagCategory::kMasked: {
            result += stats[ VariantFilterTag::kMaskedPosition ];
            result += stats[ VariantFilterTag::kMaskedRegion ];
            break;
        }
        case VariantFilterTagCategory::kMissingInvalid: {
            result += stats[ VariantFilterTag::kMissing ];
            result += stats[ VariantFilterTag::kNotPassed ];
            result += stats[ VariantFilterTag::kInvalid ];
            break;
        }
        case VariantFilterTagCategory::kSamplesFailed: {
            result += stats[ VariantFilterTag::kNoSamplePassed ];
            result += stats[ VariantFilterTag::kNotAllSamplesPassed ];
            break;
        }
        case VariantFilterTagCategory::kNumeric: {
            result += stats[ VariantFilterTag::kEmpty ];
            result += stats[ VariantFilterTag::kBelowMinReadDepth ];
            result += stats[ VariantFilterTag::kAboveMaxReadDepth ];
            result += stats[ VariantFilterTag::kAboveDeletionsCountLimit ];
            break;
        }
        case VariantFilterTagCategory::kInvariant: {
            result += stats[ VariantFilterTag::kNotSnp ];
            result += stats[ VariantFilterTag::kNotBiallelicSnp ];
            result += stats[ VariantFilterTag::kBelowSnpMinCount ];
            result += stats[ VariantFilterTag::kAboveSnpMaxCount ];
            result += stats[ VariantFilterTag::kBelowMinAlleleFreq ];
            break;
        }
        default: {
            throw std::invalid_argument(
                "Invalid VariantFilterTagCategory: " +
                std::to_string( static_cast<FilterStatus::IntType>( category ))
            );
        }
    }
    return result;
}

// =================================================================================================
//     Printing
// =================================================================================================

// --------------------------------------------------------------------------------------
//     Print variant stats
// --------------------------------------------------------------------------------------

std::ostream& print_variant_filter_stats(
    std::ostream& os,
    VariantFilterStats const& stats,
    bool verbose
) {
    // assert( stats[ VariantFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( VariantFilterTag::kEnd ) );

    // Go through all possible enum values and print them
    if( stats[VariantFilterTag::kMaskedPosition] > 0 || verbose ) {
        os << "Masked position:        " << stats[VariantFilterTag::kMaskedPosition] << "\n";
    }
    if( stats[VariantFilterTag::kMaskedRegion] > 0 || verbose ) {
        os << "Masked region:          " << stats[VariantFilterTag::kMaskedRegion] << "\n";
    }
    if( stats[VariantFilterTag::kMissing] > 0 || verbose ) {
        os << "Missing:                " << stats[VariantFilterTag::kMissing] << "\n";
    }
    if( stats[VariantFilterTag::kNotPassed] > 0 || verbose ) {
        os << "Not passed:             " << stats[VariantFilterTag::kNotPassed] << "\n";
    }
    if( stats[VariantFilterTag::kInvalid] > 0 || verbose ) {
        os << "Invalid:                " << stats[VariantFilterTag::kInvalid] << "\n";
    }
    if( stats[VariantFilterTag::kNoSamplePassed] > 0 || verbose ) {
        os << "No sample passed:       " << stats[VariantFilterTag::kNoSamplePassed] << "\n";
    }
    if( stats[VariantFilterTag::kNotAllSamplesPassed] > 0 || verbose ) {
        os << "Not all samples passed: " << stats[VariantFilterTag::kNotAllSamplesPassed] << "\n";
    }
    if( stats[VariantFilterTag::kEmpty] > 0 || verbose ) {
        os << "Empty:                  " << stats[VariantFilterTag::kEmpty] << "\n";
    }
    if( stats[VariantFilterTag::kBelowMinReadDepth] > 0 || verbose ) {
        os << "Below min read depth:   " << stats[VariantFilterTag::kBelowMinReadDepth] << "\n";
    }
    if( stats[VariantFilterTag::kAboveMaxReadDepth] > 0 || verbose ) {
        os << "Above max read depth:   " << stats[VariantFilterTag::kAboveMaxReadDepth] << "\n";
    }
    if( stats[VariantFilterTag::kAboveDeletionsCountLimit] > 0 || verbose ) {
        os << "Above deletions limit:  " << stats[VariantFilterTag::kAboveDeletionsCountLimit] << "\n";
    }
    if( stats[VariantFilterTag::kNotSnp] > 0 || verbose ) {
        os << "Not SNP:                " << stats[VariantFilterTag::kNotSnp] << "\n";
    }
    if( stats[VariantFilterTag::kNotBiallelicSnp] > 0 || verbose ) {
        os << "Not biallelic SNP:      " << stats[VariantFilterTag::kNotBiallelicSnp] << "\n";
    }
    if( stats[VariantFilterTag::kBelowSnpMinCount] > 0 || verbose ) {
        os << "Below SNP min count:    " << stats[VariantFilterTag::kBelowSnpMinCount] << "\n";
    }
    if( stats[VariantFilterTag::kAboveSnpMaxCount] > 0 || verbose ) {
        os << "Above SNP max count:    " << stats[VariantFilterTag::kAboveSnpMaxCount] << "\n";
    }
    if( stats[VariantFilterTag::kBelowMinAlleleFreq] > 0 || verbose ) {
        os << "Below min alele freq:   " << stats[VariantFilterTag::kBelowMinAlleleFreq] << "\n";
    }
    if( stats[VariantFilterTag::kPassed] > 0 || verbose ) {
        os << "Passed:                 " << stats[VariantFilterTag::kPassed] << "\n";
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

// --------------------------------------------------------------------------------------
//     Print category stats
// --------------------------------------------------------------------------------------

std::ostream& print_variant_filter_category_stats(
    std::ostream& os,
    VariantFilterCategoryStats const& stats,
    bool verbose
) {
    // assert( stats[ VariantFilterTagCategory::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( VariantFilterTagCategory::kEnd ) );

    // Go through all possible enum values and print them
    if( stats[VariantFilterTagCategory::kMasked] > 0 || verbose ) {
        os << "Masked:    " << stats[VariantFilterTagCategory::kMasked] << "\n";
    }
    if( stats[VariantFilterTagCategory::kMissingInvalid] > 0 || verbose ) {
        os << "Missing:   " << stats[VariantFilterTagCategory::kMissingInvalid] << "\n";
    }
    if( stats[VariantFilterTagCategory::kSamplesFailed] > 0 || verbose ) {
        os << "Empty:     " << stats[VariantFilterTagCategory::kSamplesFailed] << "\n";
    }
    if( stats[VariantFilterTagCategory::kNumeric] > 0 || verbose ) {
        os << "Numeric:   " << stats[VariantFilterTagCategory::kNumeric] << "\n";
    }
    if( stats[VariantFilterTagCategory::kInvariant] > 0 || verbose ) {
        os << "Invariant: " << stats[VariantFilterTagCategory::kInvariant] << "\n";
    }
    if( stats[VariantFilterTagCategory::kPassed] > 0 || verbose ) {
        os << "Passed:    " << stats[VariantFilterTagCategory::kPassed] << "\n";
    }
    return os;
}

std::string print_variant_filter_category_stats(
    VariantFilterCategoryStats const& stats,
    bool verbose
) {
    std::stringstream ss;
    print_variant_filter_category_stats( ss, stats, verbose );
    return ss.str();
}

} // namespace population
} // namespace genesis
