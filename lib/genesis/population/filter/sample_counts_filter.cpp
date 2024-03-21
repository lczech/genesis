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
#include "genesis/population/functions/functions.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Sample Counts Filter
// =================================================================================================

std::ostream& print_base_counts_filter_stats(
    std::ostream& os,
    BaseCountsFilterStats const& stats,
    bool verbose
) {
    assert( stats.data.size() == static_cast<size_t>( BaseCountsFilterTag::kEnd ) );

    // We use an explicit loop over the enum values here, which makes sure that we cannot
    // forget about any values in the future. This is a bit inefficient, but we do not expect
    // to call this function more than once.
    for( size_t i = 0; i < static_cast<size_t>( BaseCountsFilterTag::kEnd ); ++i ) {
        switch( static_cast<BaseCountsFilterTag>(i) ) {
            case BaseCountsFilterTag::kPassed: {
                auto const val = stats[BaseCountsFilterTag::kPassed];
                if( val > 0 || verbose ) {
                    os << "Passed: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kMissing: {
                auto const val = stats[BaseCountsFilterTag::kMissing];
                if( val > 0 || verbose ) {
                    os << "Missing: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kNotPassed: {
                auto const val = stats[BaseCountsFilterTag::kNotPassed];
                if( val > 0 || verbose ) {
                    os << "NotPassed: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kInvalid: {
                auto const val = stats[BaseCountsFilterTag::kInvalid];
                if( val > 0 || verbose ) {
                    os << "Invalid: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kEmpty: {
                auto const val = stats[BaseCountsFilterTag::kEmpty];
                if( val > 0 || verbose ) {
                    os << "Empty: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kBelowMinCoverage: {
                auto const val = stats[BaseCountsFilterTag::kBelowMinCoverage];
                if( val > 0 || verbose ) {
                    os << "BelowMinCoverage: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kAboveMaxCoverage: {
                auto const val = stats[BaseCountsFilterTag::kAboveMaxCoverage];
                if( val > 0 || verbose ) {
                    os << "AboveMaxCoverage: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kAboveDeletionsCountLimit: {
                auto const val = stats[BaseCountsFilterTag::kAboveDeletionsCountLimit];
                if( val > 0 || verbose ) {
                    os << "AboveDeletionsCountLimit: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kNotSnp: {
                auto const val = stats[BaseCountsFilterTag::kNotSnp];
                if( val > 0 || verbose ) {
                    os << "NotSnp: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kNotBiallelicSnp: {
                auto const val = stats[BaseCountsFilterTag::kNotBiallelicSnp];
                if( val > 0 || verbose ) {
                    os << "NotBiallelicSnp: " << val << "\n";
                }
                break;
            }
            case BaseCountsFilterTag::kEnd: {
                throw std::domain_error(
                    "Invalid BaseCountsFilterTag with value " + std::to_string(i)
                );
            }
            // Not using default here, as that way, we catch any unhandled enum values.
        }
    }
    return os;
}

std::string print_base_counts_filter_stats(
    BaseCountsFilterStats const& stats,
    bool verbose
)  {
    std::stringstream ss;
    print_base_counts_filter_stats( ss, stats, verbose );
    return ss.str();
}

} // namespace population
} // namespace genesis
