#ifndef GENESIS_POPULATION_WINDOW_FUNCTIONS_H_
#define GENESIS_POPULATION_WINDOW_FUNCTIONS_H_

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

#include <cassert>
#include <string>
#include <vector>

#include "genesis/population/window/window.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Helper Functions for Window
// =================================================================================================

/**
 * @brief Position in the genome that is used for reporting when emitting or using a window.
 *
 * See anchor_position() for details.
 */
enum class WindowAnchorType
{
    kIntervalBegin,
    kIntervalEnd,
    kIntervalMidpoint,
    kVariantFirst,
    kVariantLast,
    kVariantMedian,
    kVariantMean,
    kVariantMidpoint
};

/**
 * @brief Get the position in the chromosome reported according to a specific #WindowAnchorType.
 *
 * When a window is filled with data, we need to report the
 * position in the genome at which the window is. There are several ways that this position
 * can be computed. Typically, just the first position of the window is used (that is, for an
 * interval, the beginning of the interval, and for variants, the position of the first variant).
 *
 * However, it might be desirable to report a different position, for example when plotting the
 * results. When using WindowType::kVariants for example, one might want to plot the values
 * computed per window at the midpoint genome position of the variants in that window.
 */
template<class D, class A = EmptyAccumulator>
size_t anchor_position(
    Window<D, A> const& window,
    WindowAnchorType anchor_type = WindowAnchorType::kIntervalBegin
) {
    auto check_entries = [&](){
        if( window.entries().empty() ) {
            throw std::runtime_error(
                "Cannot use empty Window (with no variants/entries) for variant-based anchor "
                "positions. Typically these anchor positions are used with WindowType::kVariants."
            );
        }
    };

    // Calculate the SNP position that we want to output when emitting a window.
    // Some use integer division, which is intended. We don't want the hassle of floating
    // point genomic positions, so we have to do these roundings... But given a large window
    // size, that should probably not matter much.
    switch( anchor_type ) {
        case WindowAnchorType::kIntervalBegin: {
            return window.first_position();
        }
        case WindowAnchorType::kIntervalEnd: {
            return window.last_position();
        }
        case WindowAnchorType::kIntervalMidpoint: {
            return ( window.first_position() + window.last_position() ) / 2;
        }
        case WindowAnchorType::kVariantFirst: {
            check_entries();
            assert( ! window.entries().empty() );
            return window.entries().front().position;
        }
        case WindowAnchorType::kVariantLast: {
            check_entries();
            assert( ! window.entries().empty() );
            return window.entries().back().position;
        }
        case WindowAnchorType::kVariantMedian: {
            check_entries();
            assert( ! window.entries().empty() );
            return window.entries()[ window.entries().size() / 2 ].position;
        }
        case WindowAnchorType::kVariantMean: {
            check_entries();
            assert( ! window.entries().empty() );

            size_t sum = 0;
            for( auto const& e : window.entries() ) {
                sum += e.position;
            }
            return sum / window.entries().size();
        }
        case WindowAnchorType::kVariantMidpoint: {
            check_entries();
            assert( ! window.entries().empty() );
            return ( window.entries().front().position + window.entries().back().position ) / 2;
        }
        default: {
            throw std::runtime_error( "Invalid WindowAnchorType." );
        }
    }
    assert( false );
    return 0;
}

} // namespace population
} // namespace genesis

#endif // include guard
