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

#include "genesis/population/filter/sample_counts_filter.hpp"

#include "genesis/utils/text/char.hpp"
#include "genesis/population/function/functions.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace population {

// We want to make sure that the tags enum is exactly as expected here. In case that we later
// add other values to that enum, we want to know here, in order to adapt all below functions
// accordingly.
static_assert(
    static_cast<FilterStatus::IntType>( SampleCountsFilterTag::kEnd ) == 10,
    "SampleCountsFilterTag::kEnd != 10. The enum has values that are not accounted for."
);
static_assert(
    static_cast<FilterStatus::IntType>( SampleCountsFilterTagCategory::kEnd ) == 3,
    "SampleCountsFilterTagCategory::kEnd != 3. The enum has values that are not accounted for."
);

// =================================================================================================
//     Stats
// =================================================================================================

SampleCountsFilterTagCategory sample_counts_filter_tag_to_category( SampleCountsFilterTag tag )
{
    // Just return the category of the tag.
    switch( tag ) {
        case SampleCountsFilterTag::kPassed:
            return SampleCountsFilterTagCategory::kPassed;
        case SampleCountsFilterTag::kMissing:
        case SampleCountsFilterTag::kNotPassed:
        case SampleCountsFilterTag::kInvalid:
            return SampleCountsFilterTagCategory::kMissingInvalid;
        case SampleCountsFilterTag::kEmpty:
        case SampleCountsFilterTag::kBelowMinCoverage:
        case SampleCountsFilterTag::kAboveMaxCoverage:
        case SampleCountsFilterTag::kAboveDeletionsCountLimit:
        case SampleCountsFilterTag::kNotSnp:
        case SampleCountsFilterTag::kNotBiallelicSnp:
            return SampleCountsFilterTagCategory::kNumeric;
        default: {
            throw std::invalid_argument(
                "Invalid SampleCountsFilterTag: " +
                std::to_string( static_cast<FilterStatus::IntType>( tag ))
            );
        }
    }
    assert( false );

    // Make compilers happy, just in case.
    return SampleCountsFilterTagCategory::kPassed;
}

SampleCountsFilterCategoryStats sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats
) {
    assert( stats[ SampleCountsFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( SampleCountsFilterTag::kEnd ) );

    // Build our result, by simply adding up the values to our simple categories / classes.
    SampleCountsFilterCategoryStats result;
    result[SampleCountsFilterTagCategory::kPassed] += stats[ SampleCountsFilterTag::kPassed ];
    result[SampleCountsFilterTagCategory::kMissingInvalid] += stats[ SampleCountsFilterTag::kMissing ];
    result[SampleCountsFilterTagCategory::kMissingInvalid] += stats[ SampleCountsFilterTag::kNotPassed ];
    result[SampleCountsFilterTagCategory::kMissingInvalid] += stats[ SampleCountsFilterTag::kInvalid ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kEmpty ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kBelowMinCoverage ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kAboveMaxCoverage ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kAboveDeletionsCountLimit ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kNotSnp ];
    result[SampleCountsFilterTagCategory::kNumeric] += stats[ SampleCountsFilterTag::kNotBiallelicSnp ];
    return result;
}

size_t sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats, SampleCountsFilterTagCategory category
) {
    assert( stats[ SampleCountsFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( SampleCountsFilterTag::kEnd ) );

    // Select the requested category and add up their values.
    size_t result = 0;
    switch( category ) {
        case SampleCountsFilterTagCategory::kPassed: {
            result += stats[ SampleCountsFilterTag::kPassed ];
            break;
        }
        case SampleCountsFilterTagCategory::kMissingInvalid: {
            result += stats[ SampleCountsFilterTag::kMissing ];
            result += stats[ SampleCountsFilterTag::kNotPassed ];
            result += stats[ SampleCountsFilterTag::kInvalid ];
            break;
        }
        case SampleCountsFilterTagCategory::kNumeric: {
            result += stats[ SampleCountsFilterTag::kEmpty ];
            result += stats[ SampleCountsFilterTag::kBelowMinCoverage ];
            result += stats[ SampleCountsFilterTag::kAboveMaxCoverage ];
            result += stats[ SampleCountsFilterTag::kAboveDeletionsCountLimit ];
            result += stats[ SampleCountsFilterTag::kNotSnp ];
            result += stats[ SampleCountsFilterTag::kNotBiallelicSnp ];
            break;
        }
        default: {
            throw std::invalid_argument(
                "Invalid SampleCountsFilterTagCategory: " +
                std::to_string( static_cast<FilterStatus::IntType>( category ))
            );
        }
    }
    return result;
}

// =================================================================================================
//     Printing
// =================================================================================================

std::ostream& print_sample_counts_filter_stats(
    std::ostream& os,
    SampleCountsFilterStats const& stats,
    bool verbose
) {
    // Same checks as above
    static_assert(
        static_cast<FilterStatus::IntType>( SampleCountsFilterTag::kEnd ) == 10,
        "SampleCountsFilterTag::kEnd != 10. The enum has values that are not accounted for."
    );
    assert( stats[ SampleCountsFilterTag::kEnd ] == 0 );
    assert( stats.data.size() == static_cast<size_t>( SampleCountsFilterTag::kEnd ) );

    // Go through all possible enum values and print them
    if( stats[SampleCountsFilterTag::kPassed] > 0 || verbose ) {
        os << "Passed: " << stats[SampleCountsFilterTag::kPassed] << "\n";
    }
    if( stats[SampleCountsFilterTag::kMissing] > 0 || verbose ) {
        os << "Missing: " << stats[SampleCountsFilterTag::kMissing] << "\n";
    }
    if( stats[SampleCountsFilterTag::kNotPassed] > 0 || verbose ) {
        os << "NotPassed: " << stats[SampleCountsFilterTag::kNotPassed] << "\n";
    }
    if( stats[SampleCountsFilterTag::kInvalid] > 0 || verbose ) {
        os << "Invalid: " << stats[SampleCountsFilterTag::kInvalid] << "\n";
    }
    if( stats[SampleCountsFilterTag::kEmpty] > 0 || verbose ) {
        os << "Empty: " << stats[SampleCountsFilterTag::kEmpty] << "\n";
    }
    if( stats[SampleCountsFilterTag::kBelowMinCoverage] > 0 || verbose ) {
        os << "BelowMinCoverage: " << stats[SampleCountsFilterTag::kBelowMinCoverage] << "\n";
    }
    if( stats[SampleCountsFilterTag::kAboveMaxCoverage] > 0 || verbose ) {
        os << "AboveMaxCoverage: " << stats[SampleCountsFilterTag::kAboveMaxCoverage] << "\n";
    }
    if( stats[SampleCountsFilterTag::kAboveDeletionsCountLimit] > 0 || verbose ) {
        os << "AboveDeletionsCountLimit: " << stats[SampleCountsFilterTag::kAboveDeletionsCountLimit] << "\n";
    }
    if( stats[SampleCountsFilterTag::kNotSnp] > 0 || verbose ) {
        os << "NotSnp: " << stats[SampleCountsFilterTag::kNotSnp] << "\n";
    }
    if( stats[SampleCountsFilterTag::kNotBiallelicSnp] > 0 || verbose ) {
        os << "NotBiallelicSnp: " << stats[SampleCountsFilterTag::kNotBiallelicSnp] << "\n";
    }
    return os;
}

std::string print_sample_counts_filter_stats(
    SampleCountsFilterStats const& stats,
    bool verbose
)  {
    std::stringstream ss;
    print_sample_counts_filter_stats( ss, stats, verbose );
    return ss.str();
}

} // namespace population
} // namespace genesis
