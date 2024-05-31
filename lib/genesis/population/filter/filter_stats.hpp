#ifndef GENESIS_POPULATION_FILTER_FILTER_STATS_H_
#define GENESIS_POPULATION_FILTER_FILTER_STATS_H_

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

#include "genesis/population/filter/filter_status.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace genesis {
namespace population {

// =================================================================================================
//     Filter Stats
// =================================================================================================

/**
 * @brief Counts of how many entries with a particular Filter Tag occured in some data.
 *
 * This wrapper around an array is a very simple solution to keep track of the number of entries
 * (Variant%s or SampleCounts) that had each filter tag in some data. By simply creating a
 * `std::array` that has as many entries as we have filter tags, we have a slot for every tag.
 * These can then be incremented while streaming through the data, and read and printed later.
 *
 * This wrapper expects the template parameter to be an enum that has `kPassed == 0`, and has
 * `kEnd` as its last value, to determine the end of the enum and indicate the full range of all
 * values. See VariantFilterTag and SampleFilterTag for the two enums we use here.
 */
template<typename FilterTag>
struct FilterStats
{
    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using FilterTagArray = std::array<size_t, static_cast<size_t>( FilterTag::kEnd )>;

    // -------------------------------------------------------------------------
    //     Constructor
    // -------------------------------------------------------------------------

    // Value-initialize the array, so that it is all zero.
    FilterStats()
        : data()
    {}

    // -------------------------------------------------------------------------
    //     Operators and Iteration
    // -------------------------------------------------------------------------

    size_t& operator[] ( FilterTag tag )
    {
        auto const i = static_cast<typename std::underlying_type<FilterTag>::type>(tag);
        if( i >= data.size() ) {
            throw std::invalid_argument( "Invalid filter tag value " + std::to_string(i) );
        }
        return data[ i ];
    }

    size_t operator[] ( FilterTag tag ) const
    {
        auto const i = static_cast<typename std::underlying_type<FilterTag>::type>(tag);
        if( i >= data.size() ) {
            throw std::invalid_argument( "Invalid filter tag value " + std::to_string(i) );
        }
        return data[ i ];
    }

    size_t& operator[] ( size_t index )
    {
        if( index >= data.size() ) {
            throw std::invalid_argument( "Invalid filter tag value " + std::to_string(index) );
        }
        return data[ index ];
    }

    size_t operator[] ( size_t index ) const
    {
        if( index >= data.size() ) {
            throw std::invalid_argument( "Invalid filter tag value " + std::to_string(index) );
        }
        return data[ index ];
    }

    typename FilterTagArray::iterator begin()
    {
        return data.begin();
    }

    typename FilterTagArray::const_iterator begin() const
    {
        return data.begin();
    }

    typename FilterTagArray::iterator end()
    {
        return data.end();
    }

    typename FilterTagArray::const_iterator end() const
    {
        return data.end();
    }

    size_t size() const
    {
        return data.size();
    }

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    size_t sum() const
    {
        size_t s = 0;
        for( auto v : data ) {
            s += v;
        }
        return s;
    }

    size_t sum_failing() const
    {
        // Check that our special 0 case is there and has the right value.
        static_assert(
            static_cast<typename std::underlying_type<FilterTag>::type>(FilterTag::kPassed) == 0,
            "Filter Tag enum value kPassed != 0"
        );

        // Sum up all but the first (passing) value.
        size_t s = 0;
        for( size_t i = 1; i < data.size(); ++i ) {
            s += data[i];
        }
        return s;
    }

    size_t sum_non_passing() const
    {
        return sum_failing();
    }

    void clear()
    {
        data = {};
    }

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

    FilterTagArray data;
};

} // namespace population
} // namespace genesis

#endif // include guard
