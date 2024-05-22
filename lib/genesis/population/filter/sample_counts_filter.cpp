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

// =================================================================================================
//     Stats
// =================================================================================================

std::array<size_t, 3> sample_counts_filter_stats_category_counts(
    SampleCountsFilterStats const& stats
) {
    // We want to make sure that the tags enum is exactly as expected here. In case that we later
    // add other values to that enum, we want to know here, in order to adapt the function
    // accordingly. In the printing function below, we use a loop with a switch statement to be
    // notified of any missing enum values. Here, this would be a bit too inefficient, as we expect
    // this function here to be called once per position or window. Hence, we just statically
    // assert that the last value of the enum has the numerical representation that we expect.
    // If this fails, we know that we are missing a value.
    static_assert(
        static_cast<FilterStatus::IntType>( SampleCountsFilterTag::kEnd ) == 10,
        "SampleCountsFilterTag::kEnd != 10. The enum has values that are not accounted for."
    );

    // Now we can build our result with some confidence, by simply adding up the values
    // to our simple categories / classes.
    auto result = std::array<size_t, 3>{};
    result[0] += stats[ SampleCountsFilterTag::kPassed ];
    result[1] += stats[ SampleCountsFilterTag::kMissing ];
    result[1] += stats[ SampleCountsFilterTag::kNotPassed ];
    result[1] += stats[ SampleCountsFilterTag::kInvalid ];
    result[2] += stats[ SampleCountsFilterTag::kEmpty ];
    result[2] += stats[ SampleCountsFilterTag::kBelowMinCoverage ];
    result[2] += stats[ SampleCountsFilterTag::kAboveMaxCoverage ];
    result[2] += stats[ SampleCountsFilterTag::kAboveDeletionsCountLimit ];
    result[2] += stats[ SampleCountsFilterTag::kNotSnp ];
    result[2] += stats[ SampleCountsFilterTag::kNotBiallelicSnp ];
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
    assert( stats.data.size() == static_cast<size_t>( SampleCountsFilterTag::kEnd ) );

    // We use an explicit loop over the enum values here, which makes sure that we cannot
    // forget about any values in the future. This is a bit inefficient, but we do not expect
    // to call this function more than once.
    // If this fails, the above function also needs to be updated. There, we expect the function
    // to be called many times, and hence do not want this inefficient loop.
    for( size_t i = 0; i < static_cast<size_t>( SampleCountsFilterTag::kEnd ); ++i ) {
        switch( static_cast<SampleCountsFilterTag>(i) ) {
            case SampleCountsFilterTag::kPassed: {
                auto const val = stats[SampleCountsFilterTag::kPassed];
                if( val > 0 || verbose ) {
                    os << "Passed: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kMissing: {
                auto const val = stats[SampleCountsFilterTag::kMissing];
                if( val > 0 || verbose ) {
                    os << "Missing: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kNotPassed: {
                auto const val = stats[SampleCountsFilterTag::kNotPassed];
                if( val > 0 || verbose ) {
                    os << "NotPassed: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kInvalid: {
                auto const val = stats[SampleCountsFilterTag::kInvalid];
                if( val > 0 || verbose ) {
                    os << "Invalid: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kEmpty: {
                auto const val = stats[SampleCountsFilterTag::kEmpty];
                if( val > 0 || verbose ) {
                    os << "Empty: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kBelowMinCoverage: {
                auto const val = stats[SampleCountsFilterTag::kBelowMinCoverage];
                if( val > 0 || verbose ) {
                    os << "BelowMinCoverage: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kAboveMaxCoverage: {
                auto const val = stats[SampleCountsFilterTag::kAboveMaxCoverage];
                if( val > 0 || verbose ) {
                    os << "AboveMaxCoverage: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kAboveDeletionsCountLimit: {
                auto const val = stats[SampleCountsFilterTag::kAboveDeletionsCountLimit];
                if( val > 0 || verbose ) {
                    os << "AboveDeletionsCountLimit: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kNotSnp: {
                auto const val = stats[SampleCountsFilterTag::kNotSnp];
                if( val > 0 || verbose ) {
                    os << "NotSnp: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kNotBiallelicSnp: {
                auto const val = stats[SampleCountsFilterTag::kNotBiallelicSnp];
                if( val > 0 || verbose ) {
                    os << "NotBiallelicSnp: " << val << "\n";
                }
                break;
            }
            case SampleCountsFilterTag::kEnd: {
                throw std::domain_error(
                    "Invalid SampleCountsFilterTag with value " + std::to_string(i)
                );
            }
            // Not using default here, as that way, we catch any unhandled enum values.
        }
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
